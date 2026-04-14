#pragma once
#include <string>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <vector>
#include "MessageTypes.h"

struct BookSnapshot {
    std::vector<uint32_t> bid_prices;
    std::vector<uint32_t> bid_shares;
    std::vector<uint32_t> ask_prices;
    std::vector<uint32_t> ask_shares;
};

enum Trading_State {
    Halt,
    Paused,
    Quotation,
    Trading
};

struct Order {
    // std::uint64_t reference;
    char indicator; // Should be either B or S
    uint32_t shares;
    // char stock[8];
    uint32_t price;
    std::string MPID;
};

class Order_Book {
private:
    std::string stock_name; // Need, Stock Locate used for stock finding, more consistent.
    Trading_State state;
    std::map<std::uint32_t, std::uint32_t> bids;   // price → total shares (buy side)
    std::map<std::uint32_t, std::uint32_t> asks;   // price → total shares (sell side)
    std::unordered_map<std::uint64_t, Order> orders; // order reference → order details

    // Per-price-level changes since the last delta flush.
    // Value = new total shares at that level; 0 means the level was removed.
    std::map<std::uint32_t, std::uint32_t> bid_deltas;
    std::map<std::uint32_t, std::uint32_t> ask_deltas;

    // Record the post-mutation state of a price level into the appropriate delta map.
    void record_delta(char side_indicator, std::uint32_t price);

public:
    Order_Book() {};
    Order_Book(std::string_view stock);
    // Stock Info
    const std::string& GetName() const { return stock_name; }
    bool IsInitialised() const { return !stock_name.empty(); }
    Trading_State Get_State(){ return state; }
    void Set_State(char new_state);
    // Delta access
    bool HasDeltas() const { return !bid_deltas.empty() || !ask_deltas.empty(); }
    const std::map<std::uint32_t, std::uint32_t>& GetBidDeltas() const { return bid_deltas; }
    const std::map<std::uint32_t, std::uint32_t>& GetAskDeltas() const { return ask_deltas; }
    void ClearDeltas() { bid_deltas.clear(); ask_deltas.clear(); }
    // Snapshot
    BookSnapshot GetSnapshot(size_t top_n) const;

    // Order Book Updates
    //add Order - A, F
    template<typename T>
    void Add(const T* order);

    //execute Order - E, C
    void Execute(const Order_Executed* order);
    void Execute(const Order_Executed_With_Price* order);
    //cancel - X
    void Cancel(const Order_Cancel* order);
    //delete - D
    void Delete(const Order_Delete* order);
    //replace - U
    void Replace(const Order_Replace* order);
};
