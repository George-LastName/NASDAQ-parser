#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H
#include <string>
#include <cstdint>
#include <map>
#include <unordered_map>
#include "MessageTypes.h"

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
    // Stock Info
    Trading_State Get_State(){ return state; }
    void Set_State(char new_state);

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

#endif //ORDER_BOOK_H
