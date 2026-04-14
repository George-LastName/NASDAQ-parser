#pragma once

#include <clickhouse/client.h>
#include <string>
#include <unordered_map>
#include <cstdint>

#include "../OrderBook.h"

// Flush per-price-level deltas for every book that changed since the last call.
void flush_deltas(
    clickhouse::Client& client,
    const std::string& delta_table,
    uint64_t timestamp_ns,
    std::unordered_map<uint16_t, Order_Book>& books,
    bool force_flush = false);

// Write a full top-N snapshot of every initialised book.
void snapshot_all_books(
    clickhouse::Client& client,
    const std::string& snapshot_table,
    uint64_t timestamp_ns,
    const std::unordered_map<uint16_t, Order_Book>& books,
    size_t top_n,
    bool force_flush = false);
