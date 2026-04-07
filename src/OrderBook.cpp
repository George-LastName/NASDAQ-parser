
#include <cstdlib>
#include <iostream>
#include <string>

#include "OrderBook.h"
#include "message_types.h"

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
            std::cout << "Unknown Trading State for " << stock_name << " of " << new_state << "\n";
            exit(0);
        }
    }
}

template<typename T>
void Order_Book::Add(const T& order){
    Order new_order;

    if constexpr (std::is_same_v<T, Add_Order_MPID>) {
        new_order.MPID = std::string(order.attribution, 4);
    }
    new_order.indicator = order.buy_sell_indicator;
    new_order.shares    = ntohl(order.shares);
    new_order.price     = ntohl(order.price);

    orders.insert({order.order_reference_number, new_order});

    if(new_order.indicator != 'B' && new_order.indicator != 'S'){
        std::cout << "Invalid Side for " << stock_name << " : |" << new_order.indicator << "|\n";
        exit(0);
    }

    auto& side = (new_order.indicator == 'B') ? bids : asks;
    side[new_order.price] += new_order.shares;
}

template void Order_Book::Add(const Add_Order_MPID& order);
template void Order_Book::Add(const Add_Order_No_MPID& order);
