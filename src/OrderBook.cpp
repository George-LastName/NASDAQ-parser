#include "OrderBook.h"
#include <cstdlib>
#include <iostream>

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
void Order_Book::Add(const char &mess_type){
    // log order.
    if(mess_type == 'A'){
        std::cout << "A, ";
    }
    else if (mess_type == 'F') {
        std::cout << "F, ";
    }
    // add to either bids or asks.
}
