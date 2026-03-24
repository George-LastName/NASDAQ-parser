#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H
#include <string>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <deque>

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
    // char MPID[4];
};

class Order_Book {
private:
    char stock[8];
    Trading_State state;
    std::map<std::uint32_t,  std::uint64_t> bids;//  price, num of shares
    std::map<std::uint32_t,  std::uint64_t> asks;//  price, num of shares
    std::unordered_map<std::uint64_t, Order> orders; // order reference, order details.
public:
    Order_Book();
    //add Order - A, F
    void Add();
    //execute Order - E, C
    void Execute();
    //cancel - x
    void Cancel();
    //delete - D
    void Delete();
    //replace - U
    void Replace();


};

#endif //ORDER_BOOK_H
