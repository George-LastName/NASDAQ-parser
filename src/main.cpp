#include <cstdint> // uintX_t
#include <cstdlib>
#include <unistd.h> // close
#include <fcntl.h> // O_RDONLY

#include <iostream> // cout et. all
#include <string>

#include <sys/mman.h> // PROT_READ et. all & munmap
#include <sys/stat.h> // fstat

#include <unordered_set>
#include <unordered_map>

// Clickhouse (DB)
#include <clickhouse/client.h>

#include "MessageTypes.h" // Messages
#include "OrderBook.h"

#define HEADER_LENGTH 11

enum class Market_State {
    START_DAY,
    START_SYSTEM,
    START_MARKET,
    END_MARKET,
    END_SYSTEM,
    END_DAY
};

int counts[256] = {0};
std::unordered_map<std::string, std::array<int, 256>> stock_messages;
std::unordered_map<uint16_t, Order_Book> stock_books;

static inline void parse_message(uint8_t* ptr){

    const auto* Header = reinterpret_cast<ITCH_Header*>(ptr);
    ptr += HEADER_LENGTH;

    /* TODO: For SIMULATION, check if able to send message, if not wait until able.
     *  while(sim_time > header->timestamp){
     *      wait;
     *  }
     */

    counts[Header->type]++;
    auto& stock = stock_books[Header->get_locate()];

    switch (Header->type) {
        // [[X]] helps complier order the jump table for better efficiency.
        [[unlikely]] case 'S': {
            auto* Mess = reinterpret_cast<const Sys_Event*>(ptr);
            std::cout << Header->get_time_from_mid() << ": " << Mess->event_code << "\n";
            break;
        }
        case 'R': {
            auto* Mess = reinterpret_cast<const Stock_Dir*>(ptr);
            stock = Order_Book(std::string_view(Mess->stock, 8));
            break;
        }
        case 'H': {
            auto* Mess = reinterpret_cast<Stock_Trading_Action*>(ptr);
            stock.Set_State(Mess->trading_state);
            break;
        }
        case 'Y': {
            // auto* Mess = reinterpret_cast<Reg_Sho_Restriction*>(ptr);
            break;
        }
        case 'L': {
            // auto* Mess = reinterpret_cast<Market_Participant_Position*>(ptr);
            break;
        }
        [[unlikely]] case 'V': {
            // auto* Mess = reinterpret_cast<MWCB_Decline_Level*>(ptr);
            break;
        }
        [[unlikely]] case 'W': {
            // auto* Mess = reinterpret_cast<MWCB_Status*>(ptr);
                break;
        }
        [[unlikely]] case 'K': {
            // auto* Mess = reinterpret_cast<Quoting_Period_Update*>(ptr);
            break;
        }
        [[unlikely]] case 'J': {
            // auto* Mess = reinterpret_cast<LULD_Auction_Collar*>(ptr);
            break;
        }
        [[unlikely]] case 'h': {
            // auto* Mess = reinterpret_cast<Operational_Halt*>(ptr);
            break;
        }
        [[likely]] case 'A': {
            auto* Mess = reinterpret_cast<Add_Order_No_MPID*>(ptr);
            stock.Add(Mess);
            break;
        }
        case 'F': {
            auto* Mess = reinterpret_cast<Add_Order_MPID*>(ptr);
            stock.Add(Mess);
            break;
        }
        case 'E': {
            auto* Mess =  reinterpret_cast<Order_Executed*>(ptr);
            stock.Execute(Mess);
            break;
        }
        case 'C': {
            auto* Mess = reinterpret_cast<Order_Executed_With_Price*>(ptr);
            stock.Execute(Mess);
            break;
        }
        case 'X': {
            auto* Mess = reinterpret_cast<Order_Cancel*>(ptr);
            stock.Cancel(Mess);
            break;
        }
        [[likely]] case 'D': {
            auto* Mess = reinterpret_cast<Order_Delete*>(ptr);
            stock.Delete(Mess);
            break;
        }
        [[likely]] case 'U': {
            auto* Mess = reinterpret_cast<Order_Replace*>(ptr);
            stock.Replace(Mess);
            break;
        }
        case 'P': {
            // auto* Mess = reinterpret_cast<Trade_Non_Cross*>(ptr);
            break;
        }
        case 'Q': {
            // auto* Mess = reinterpret_cast<Cross_Trade*>(ptr);
            break;
        }
        [[unlikely]] case 'B': {
            // auto* Mess = reinterpret_cast<Broken_Trade*>(ptr);
            break;
        }
        case 'I': {
            // auto* Mess = reinterpret_cast<NOII*>(ptr);
            break;
        }
        [[unlikely]] case 'N': {
            // auto* Mess = reinterpret_cast<RPII*>(ptr);
            break;
        }
        [[unlikely]] case 'O': {
            // auto* Mess = reinterpret_cast<DLCR_Price_Discovery*>(ptr);
            break;
        }
        default:{
            std::cout << "ERROR: Unknown Message Type! : " << ptr[0] << std::endl;
            break;
        }
    }
}

int main(int argc, char* argv[]){

    // Check file path has been provided
    if (argc != 2) {
        std::cout << "Second argument must be file path/filename." << std::endl;
        exit(EXIT_FAILURE);
    }

    const char* filepath = argv[1];

    // Open file and get file size
    int rdonly_file = open(filepath, O_RDONLY);
    if (rdonly_file == -1){
        std::cout << "Failed to open file" << std::endl;
        exit(EXIT_FAILURE);
    }

    struct stat file_stats;
    if(fstat(rdonly_file, &file_stats) == -1){
        std::cout << "Failed to get file size." << std::endl;
        close(rdonly_file);
        exit(EXIT_FAILURE);
    }

    const off_t file_size = file_stats.st_size;

    // Get pointer to start of provided file
    std::uint8_t* mapped_file = reinterpret_cast<std::uint8_t*>(mmap((caddr_t)0, file_size, PROT_READ, MAP_SHARED, rdonly_file, 0));
    if(mapped_file == MAP_FAILED){
        std::cout << "Failed to mmap file.\n" << std::endl;
        close(rdonly_file);
        exit(EXIT_FAILURE);
    }
    close(rdonly_file);


    // clickhouse::Client client{clickhouse::ClientOptions().SetHost("localhost")};




    std::uint8_t* filePtr = mapped_file;
    const std::uint8_t* file_end = mapped_file + file_size;

    // Loop through file
    while (filePtr < file_end) {
        const uint16_t message_l = ntohs(*reinterpret_cast<const uint16_t*>(filePtr));
        filePtr += 2;

        parse_message(filePtr);
        filePtr += message_l;
    }

    std::cout << "\n";
    long sum = 0;
    for (int i = 0; i < 256; i++){
        if (counts[i] != 0) std::cout << static_cast<char>(i) << ": " << counts[i] << "\n";
        sum += counts[i];
    }

    std::cout << "\n";

    if(munmap(mapped_file, file_size) == -1){
        std::cout << "Failed to munmap." << std::endl;
    }

    return 0;
}
