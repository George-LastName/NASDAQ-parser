
#include <cstdlib>
#include <iostream>
#include <string>

#include "OrderBook.h"
#include "MessageTypes.h"

Order_Book::Order_Book(std::string_view stock){
    state = Trading_State::Halt;
    stock_name = stock;
}

// Capture the post-mutation total at a price level into the delta map.
// If the level no longer exists in the book, records 0 (level removed).
void Order_Book::record_delta(char side_indicator, std::uint32_t price) {
    const auto& side = (side_indicator == 'B') ? bids : asks;
    auto&     deltas = (side_indicator == 'B') ? bid_deltas : ask_deltas;
    const auto it = side.find(price);
    deltas[price] = (it != side.end()) ? it->second : 0;
}

void Order_Book::Set_State(char new_state){
    switch (new_state){
        case 'H': { state = Trading_State::Halt;      break; }
        case 'P': { state = Trading_State::Paused;    break; }
        case 'Q': { state = Trading_State::Quotation; break; }
        case 'T': { state = Trading_State::Trading;   break; }
        default:{
            std::cout << "Unknown Trading State for " << stock_name << " of " << new_state << "\n";
            exit(0);
        }
    }
    // State changes don't affect price levels — no delta recorded.
}

template<typename T>
void Order_Book::Add(const T* order){
    Order new_order;

    if constexpr (std::is_same_v<T, Add_Order_MPID>) {
        new_order.MPID = std::string(order->attribution, 4);
    }
    new_order.indicator = order->buy_sell_indicator;
    new_order.shares    = ntohl(order->shares);
    new_order.price     = ntohl(order->price);

    orders.insert({order->order_reference_number, new_order});

    if(new_order.indicator != 'B' && new_order.indicator != 'S'){
        std::cout << "Invalid Side for " << stock_name << " : |" << new_order.indicator << "|\n";
        exit(0);
    }

    auto& side = (new_order.indicator == 'B') ? bids : asks;
    side[new_order.price] += new_order.shares;
    record_delta(new_order.indicator, new_order.price);
}

template void Order_Book::Add(const Add_Order_MPID* order);
template void Order_Book::Add(const Add_Order_No_MPID* order);

BookSnapshot Order_Book::GetSnapshot(size_t top_n) const {
    BookSnapshot snap;

    // Top N bids: highest prices first (reverse-iterate the ascending map)
    size_t count = 0;
    for (auto it = bids.rbegin(); it != bids.rend() && count < top_n; ++it, ++count) {
        snap.bid_prices.push_back(it->first);
        snap.bid_shares.push_back(it->second);
    }

    // Top N asks: lowest prices first (forward-iterate)
    count = 0;
    for (auto it = asks.begin(); it != asks.end() && count < top_n; ++it, ++count) {
        snap.ask_prices.push_back(it->first);
        snap.ask_shares.push_back(it->second);
    }

    return snap;
}

void Order_Book::Execute(const Order_Executed* order){
    auto it = orders.find(order->order_reference_number);
    if(it == orders.end()){
        std::cout << "Order reference not found for execution: " << order->order_reference_number << "\n";
        return;
    }

    Order& exec_order = it->second;
    uint32_t executed_shares = ntohl(order->executed_shares);

    if(executed_shares > exec_order.shares){
        std::cout << "Executed shares exceed order shares for " << stock_name << "\n";
        return;
    }

    auto& side = (exec_order.indicator == 'B') ? bids : asks;
    side[exec_order.price] -= executed_shares;
    if(side[exec_order.price] == 0){
        side.erase(exec_order.price);
    }
    record_delta(exec_order.indicator, exec_order.price);

    exec_order.shares -= executed_shares;
    if(exec_order.shares == 0){
        orders.erase(it);
    }
}

void Order_Book::Execute(const Order_Executed_With_Price* order){
    auto it = orders.find(order->order_reference_number);
    if(it == orders.end()){
        std::cout << "Order reference not found for execution with price: " << order->order_reference_number << "\n";
        return;
    }

    Order& exec_order = it->second;
    uint32_t executed_shares = ntohl(order->executed_shares);

    if(executed_shares > exec_order.shares){
        std::cout << "Executed shares exceed order shares for " << stock_name << "\n";
        return;
    }

    auto& side = (exec_order.indicator == 'B') ? bids : asks;
    side[exec_order.price] -= executed_shares;
    if(side[exec_order.price] == 0){
        side.erase(exec_order.price);
    }
    record_delta(exec_order.indicator, exec_order.price);

    exec_order.shares -= executed_shares;
    if(exec_order.shares == 0){
        orders.erase(it);
    }
}

void Order_Book::Cancel(const Order_Cancel* order){
    auto it = orders.find(order->order_reference_number);
    if(it == orders.end()){
        std::cout << "Order reference not found for cancellation: " << order->order_reference_number << "\n";
        return;
    }

    Order& cancel_order = it->second;
    uint32_t canceled_shares = ntohl(order->canceled_shares);

    if(canceled_shares > cancel_order.shares){
        std::cout << "Canceled shares exceed order shares for " << stock_name << "\n";
        return;
    }

    auto& side = (cancel_order.indicator == 'B') ? bids : asks;
    side[cancel_order.price] -= canceled_shares;
    if(side[cancel_order.price] == 0){
        side.erase(cancel_order.price);
    }
    record_delta(cancel_order.indicator, cancel_order.price);

    cancel_order.shares -= canceled_shares;
    if(cancel_order.shares == 0){
        orders.erase(it);
    }
}

void Order_Book::Delete(const Order_Delete* order){
    auto it = orders.find(order->order_reference_number);
    if(it == orders.end()){
        std::cout << "Order reference not found for deletion: " << order->order_reference_number << "\n";
        return;
    }

    Order& delete_order = it->second;

    auto& side = (delete_order.indicator == 'B') ? bids : asks;
    side[delete_order.price] -= delete_order.shares;
    if(side[delete_order.price] == 0){
        side.erase(delete_order.price);
    }
    record_delta(delete_order.indicator, delete_order.price);

    orders.erase(it);
}

void Order_Book::Replace(const Order_Replace* order){
    auto it = orders.find(order->original_order_reference_number);
    if(it == orders.end()){
        std::cout << "Original order reference not found for replacement: " << order->original_order_reference_number << "\n";
        return;
    }

    Order& old_order = it->second;

    auto& side = (old_order.indicator == 'B') ? bids : asks;
    side[old_order.price] -= old_order.shares;
    if(side[old_order.price] == 0){
        side.erase(old_order.price);
    }
    record_delta(old_order.indicator, old_order.price);

    Order new_order;
    new_order.indicator = old_order.indicator;
    new_order.shares    = ntohl(order->shares);
    new_order.price     = ntohl(order->price);
    new_order.MPID      = old_order.MPID;

    orders.erase(it);
    orders.insert({order->new_order_reference_number, new_order});

    side[new_order.price] += new_order.shares;
    record_delta(new_order.indicator, new_order.price);
}
