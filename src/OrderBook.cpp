#include "OrderBook.h"
#include "message_types.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

Order_Book::Order_Book(std::string_view stock){
    state = Trading_State::Halt;
    stock_name = stock;
}

void Order_Book::Set_State(char new_state){
    switch (new_state){
        case 'H': {
            state = Trading_State::Halt;
            break;
        }
        case 'P': {
            state = Trading_State::Paused;
            break;
        }
        case 'Q': {
            state = Trading_State::Quotation;
            break;
        }
        case 'T': {
            state = Trading_State::Trading;
            break;
        }
        default:{
            std::cout << "Unknown Trading State for " << Order_Book::Get_Stock() << " of " << new_state << "\n";
            exit(1);
        }
    }
}

// Handles Adding trades to the Order Book. A/F
void Order_Book::Add(const Add_Order_No_MPID* order){
    // log order.
    // add to either bids or asks.
    // Order new_order;
    // new_order.indicator = order->buy_sell_indicator;
    // new_order.shares    = ntohl(order->shares);
    // new_order.price     = ntohl(order->price);
    // orders.insert({order->order_reference_number, new_order});
    // Order_Book::Log_Order(&new_order.indicator, &new_order.price, &new_order.shares);
    Order_Book::Add(order->order_reference_number, order->buy_sell_indicator, order->shares, order->price, "", true);
}

void Order_Book::Add(const Add_Order_MPID* order){
    // Order new_order;
    // new_order.indicator = order->buy_sell_indicator;
    // new_order.shares    = order->shares;
    // new_order.price     = order->price;
    // new_order.MPID      = std::string_view(order->attribution, 4);
    // orders.insert({order->order_reference_number, new_order});
    // Order_Book::Log_Order(&new_order.indicator, &new_order.price, &new_order.shares);
    Order_Book::Add(order->order_reference_number, order->buy_sell_indicator, order->shares, order->price, order->attribution, true);
}

void Order_Book::Add( uint64_t ref,
                            char buy_sell_indicator,
                            uint32_t shares,
                            uint32_t price,
                            std::string MPID,
                            bool convert_from_bin){
    Order new_order;
    new_order.indicator = buy_sell_indicator;
    new_order.shares    = convert_from_bin ? ntohl(shares) : shares;
    new_order.price     = convert_from_bin ? ntohl(price)  : shares;
    if(MPID != "") new_order.MPID = MPID;

    orders.insert({ref, new_order});
    Order_Book::Log_Order(&new_order.indicator, &new_order.price, &new_order.shares);
}
// Update bids or asks
void Order_Book::Log_Order(const char* indicator, const std::uint32_t* price,  std::uint32_t* share_num){
    if(*indicator == 'B'){
        bids[*price] += *share_num;
        if(bids.size() == 10){
            std::cout << Order_Book::Get_Stock() << " |\n";
            for (const auto& [key, value] : bids){
                std::cout << '[' << key/10000.0 << "] = " << value << "; ";
            }
            std::cout << "\n";
            exit(1);
        }
    } else {
        asks[*price] += *share_num;
    }
}
