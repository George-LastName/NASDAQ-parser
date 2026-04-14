#include "ClickhouseOrderBook.h"

#include <clickhouse/client.h>
#include <clickhouse/columns/array.h>
#include <clickhouse/columns/lowcardinality.h>
#include <clickhouse/columns/numeric.h>
#include <clickhouse/columns/string.h>

#include <unordered_map>
#include <cstdint>
#include <iostream>
#include <string>
#include <chrono>


// Flush per-price-level deltas for every book that changed since the last call.
// Each row: (timestamp_ns, stock_id, stock_name, side, price, shares).
// shares == 0 means the price level was removed from the book.
// Deltas within a 1-second window collapse to their final state — only the net
// result at each price level is written, never intermediate states.
void flush_deltas(
    clickhouse::Client& client,
    const std::string& delta_table,
    uint64_t timestamp_ns,
    std::unordered_map<uint16_t, Order_Book>& books,
    bool force_flush)
{
    static auto col_timestamp  = std::make_shared<clickhouse::ColumnUInt64>();
    static auto col_stock_id   = std::make_shared<clickhouse::ColumnUInt16>();
    static auto col_stock_name = std::make_shared<clickhouse::ColumnLowCardinalityT<clickhouse::ColumnString>>();
    static auto col_side       = std::make_shared<clickhouse::ColumnLowCardinalityT<clickhouse::ColumnString>>();
    static auto col_price      = std::make_shared<clickhouse::ColumnUInt32>();
    static auto col_shares     = std::make_shared<clickhouse::ColumnUInt32>();
    static int buffered_rounds = 0;

    static constexpr int FLUSH_EVERY = 100; // flush every 100 delta rounds (~100s of data)

    for (auto& [stock_id, book] : books) {
        if (!book.IsInitialised() || !book.HasDeltas()) continue;

        const std::string_view name = book.GetName();

        for (const auto& [price, shares] : book.GetBidDeltas()) {
            col_timestamp->Append(timestamp_ns);
            col_stock_id->Append(stock_id);
            col_stock_name->Append(name);
            col_side->Append(std::string_view("B", 1));
            col_price->Append(price);
            col_shares->Append(shares);
        }
        for (const auto& [price, shares] : book.GetAskDeltas()) {
            col_timestamp->Append(timestamp_ns);
            col_stock_id->Append(stock_id);
            col_stock_name->Append(name);
            col_side->Append(std::string_view("S", 1));
            col_price->Append(price);
            col_shares->Append(shares);
        }

        book.ClearDeltas();
    }

    buffered_rounds++;
    if (!force_flush && buffered_rounds < FLUSH_EVERY) return;
    if (col_timestamp->Size() == 0) return;

    clickhouse::Block block;
    block.AppendColumn("timestamp_ns", col_timestamp);
    block.AppendColumn("stock_id",     col_stock_id);
    block.AppendColumn("stock_name",   col_stock_name);
    block.AppendColumn("side",         col_side);
    block.AppendColumn("price",        col_price);
    block.AppendColumn("shares",       col_shares);

    client.Insert(delta_table, block);
    //std::cout << "Flushed " << buffered_rounds << " delta rounds, rows: " << col_timestamp->Size() << "\n";

    col_timestamp  = std::make_shared<clickhouse::ColumnUInt64>();
    col_stock_id   = std::make_shared<clickhouse::ColumnUInt16>();
    col_stock_name = std::make_shared<clickhouse::ColumnLowCardinalityT<clickhouse::ColumnString>>();
    col_side       = std::make_shared<clickhouse::ColumnLowCardinalityT<clickhouse::ColumnString>>();
    col_price      = std::make_shared<clickhouse::ColumnUInt32>();
    col_shares     = std::make_shared<clickhouse::ColumnUInt32>();
    buffered_rounds = 0;
}

// Write a full top-N snapshot of every initialised book.
// Runs at a lower frequency (every 60 s) to provide a recovery baseline so
// consumers do not need to replay the entire delta stream from midnight.
void snapshot_all_books(
    clickhouse::Client& client,
    const std::string& snapshot_table,
    uint64_t timestamp_ns,
    const std::unordered_map<uint16_t, Order_Book>& books,
    size_t top_n,
    bool force_flush)
{
    static auto col_stock_id   = std::make_shared<clickhouse::ColumnUInt16>();
    static auto col_stock_name = std::make_shared<clickhouse::ColumnLowCardinalityT<clickhouse::ColumnString>>();
    static auto col_timestamp  = std::make_shared<clickhouse::ColumnUInt64>();
    static auto col_bid_prices = std::make_shared<clickhouse::ColumnArrayT<clickhouse::ColumnUInt32>>();
    static auto col_bid_shares = std::make_shared<clickhouse::ColumnArrayT<clickhouse::ColumnUInt32>>();
    static auto col_ask_prices = std::make_shared<clickhouse::ColumnArrayT<clickhouse::ColumnUInt32>>();
    static auto col_ask_shares = std::make_shared<clickhouse::ColumnArrayT<clickhouse::ColumnUInt32>>();
    static int buffered_snapshots = 0;

    static constexpr int FLUSH_EVERY = 10; // flush every 10 snapshots (~10 min of data)

    for (const auto& [stock_id, book] : books) {
        if (!book.IsInitialised()) continue;

        const auto snap = book.GetSnapshot(top_n);
        col_stock_id->Append(stock_id);
        col_stock_name->Append(std::string_view(book.GetName()));
        col_timestamp->Append(timestamp_ns);
        col_bid_prices->Append(snap.bid_prices);
        col_bid_shares->Append(snap.bid_shares);
        col_ask_prices->Append(snap.ask_prices);
        col_ask_shares->Append(snap.ask_shares);
    }

    buffered_snapshots++;
    if (!force_flush && buffered_snapshots < FLUSH_EVERY) return;
    if (col_stock_id->Size() == 0) return;

    clickhouse::Block block;
    block.AppendColumn("stock_id",     col_stock_id);
    block.AppendColumn("stock_name",   col_stock_name);
    block.AppendColumn("timestamp_ns", col_timestamp);
    block.AppendColumn("bid_prices",   col_bid_prices);
    block.AppendColumn("bid_shares",   col_bid_shares);
    block.AppendColumn("ask_prices",   col_ask_prices);
    block.AppendColumn("ask_shares",   col_ask_shares);

    client.Insert(snapshot_table, block);
    std::chrono::nanoseconds duration(timestamp_ns);
    std::chrono::hh_mm_ss real_time{duration};
    std::cout << real_time <<  ": Flushed " << buffered_snapshots << " snapshots, rows: " << col_stock_id->Size() << "\n";

    col_stock_id   = std::make_shared<clickhouse::ColumnUInt16>();
    col_stock_name = std::make_shared<clickhouse::ColumnLowCardinalityT<clickhouse::ColumnString>>();
    col_timestamp  = std::make_shared<clickhouse::ColumnUInt64>();
    col_bid_prices = std::make_shared<clickhouse::ColumnArrayT<clickhouse::ColumnUInt32>>();
    col_bid_shares = std::make_shared<clickhouse::ColumnArrayT<clickhouse::ColumnUInt32>>();
    col_ask_prices = std::make_shared<clickhouse::ColumnArrayT<clickhouse::ColumnUInt32>>();
    col_ask_shares = std::make_shared<clickhouse::ColumnArrayT<clickhouse::ColumnUInt32>>();
    buffered_snapshots = 0;
}
