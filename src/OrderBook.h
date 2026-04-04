#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H
#include <string>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <deque>
#include <string>
#include "message_types.h"

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
    std::map<std::uint32_t,  std::uint32_t> bids;//  price, num of shares
    std::map<std::uint32_t,  std::uint32_t> asks;//  price, num of shares
    std::unordered_map<std::uint64_t, Order> orders; // order reference, order details.
public:
    Order_Book() {};
    Order_Book(std::string_view stock);
    std::string_view Get_Stock() { return stock_name;}
    // Stock Info
    Trading_State Get_State(){ return state; }
    void Set_State(char new_state);

    // Order Book Updates
    //add Order - A, F
    void Add(const Add_Order_No_MPID* order);
    void Add(const Add_Order_MPID* order);
    void Add(uint64_t ref,
                               char buy_sell_indicator,
                               uint32_t shares,
                               uint32_t price,
                               std::string MPID,
                               bool convert_from_bin);

    // Update bids or asks.
    void Log_Order(const char* indicator, const std::uint32_t* price,  std::uint32_t* share_num);
    //execute Order - E, C
    void Execute(const Order_Executed* order);
    void Execute(const Order_Executed_With_Price* order);
    //cancel - x
    void Cancel();
    //delete - D
    void Delete();
    //replace - U
    void Replace();
};

#endif //ORDER_BOOK_H
