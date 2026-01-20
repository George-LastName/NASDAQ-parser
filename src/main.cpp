#include <bits/types/locale_t.h>
#include <cstdint>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include "message_types.h"

static inline void parse_message(uint8_t* ptr){

    const auto* header = reinterpret_cast<ITCH_Header*>(ptr);

    /* TODO: For SIMULATION, check if able to send message, if not wait until able.
     *  while(sim_time > header->timestamp){
     *      wait;
     *  }
     */

    switch (header->type) {
        case 'S': {
            auto* Mess = reinterpret_cast<const Sys_Event*>(ptr);
            if (Mess->header.type != 'S'){
                std::cout << "ERROR: Types do not match! S != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'R': {
            auto* Mess = reinterpret_cast<const Stock_Dir*>(ptr);
            if (Mess->header.type != 'R'){
                std::cout << "ERROR: Types do not match! R != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'H': {
            auto* Mess = reinterpret_cast<Stock_Trading_Action*>(ptr);
            if (Mess->header.type != 'H'){
                std::cout << "ERROR: Types do not match! H != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'Y': {
            auto* Mess = reinterpret_cast<Reg_Sho_Restriction*>(ptr);

            if (Mess->header.type != 'Y'){
                std::cout << "ERROR: Types do not match! Y != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'L': {
            auto* Mess = reinterpret_cast<Market_Participant_Position*>(ptr);
            if (Mess->header.type != 'L'){
                std::cout << "ERROR: Types do not match! L != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'V': {
            auto* Mess = reinterpret_cast<MWCB_Decline_Level*>(ptr);
            if (Mess->header.type != 'V'){
                std::cout << "ERROR: Types do not match! V != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'W': {
            auto* Mess = reinterpret_cast<MWCB_Status*>(ptr);
            if (Mess->header.type != 'W'){
                std::cout << "ERROR: Types do not match! W != " << Mess->header.type << "\n";
            }
                break;
        }
        case 'K': {
            auto* Mess = reinterpret_cast<Quoting_Period_Update*>(ptr);
            if (Mess->header.type != 'K'){
                std::cout << "ERROR: Types do not match! K != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'J': {
            auto* Mess = reinterpret_cast<LULD_Auction_Collar*>(ptr);
            if (Mess->header.type != 'J'){
                std::cout << "ERROR: Types do not match! J != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'h': {
            auto* Mess = reinterpret_cast<Operational_Halt*>(ptr);
            if (Mess->header.type != 'h'){
                std::cout << "ERROR: Types do not match! h != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'A': {
            auto* Mess = reinterpret_cast<Add_Order_No_MPID*>(ptr);
            if (Mess->header.type != 'A'){
                std::cout << "ERROR: Types do not match! A != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'F': {
            auto* Mess = reinterpret_cast<Add_Order_MPID*>(ptr);
            if (Mess->header.type != 'F'){
                std::cout << "ERROR: Types do not match! F != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'E': {
            auto* Mess = reinterpret_cast<Order_Executed*>(ptr);
            if (Mess->header.type != 'E'){
                std::cout << "ERROR: Types do not match! E != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'C': {
            auto* Mess = reinterpret_cast<Order_Executed_With_Price*>(ptr);
            if (Mess->header.type != 'C'){
                std::cout << "ERROR: Types do not match! C != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'X': {
            auto* Mess = reinterpret_cast<Order_Cancel*>(ptr);
            if (Mess->header.type != 'X'){
                std::cout << "ERROR: Types do not match! X != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'D': {
            auto* Mess = reinterpret_cast<Order_Delete*>(ptr);
            if (Mess->header.type != 'D'){
                std::cout << "ERROR: Types do not match! D != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'U': {
            auto* Mess = reinterpret_cast<Order_Replace*>(ptr);
            if (Mess->header.type != 'U'){
                std::cout << "ERROR: Types do not match! U != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'P': {
            auto* Mess = reinterpret_cast<Trade_Non_Cross*>(ptr);
            if (Mess->header.type != 'P'){
                std::cout << "ERROR: Types do not match! P != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'Q': {
            auto* Mess = reinterpret_cast<Cross_Trade*>(ptr);
            if (Mess->header.type != 'Q'){
                std::cout << "ERROR: Types do not match! Q != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'B': {
            auto* Mess = reinterpret_cast<Broken_Trade*>(ptr);
            if (Mess->header.type != 'B'){
                std::cout << "ERROR: Types do not match! B != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'I': {
            auto* Mess = reinterpret_cast<NOII*>(ptr);
            if (Mess->header.type != 'I'){
                std::cout << "ERROR: Types do not match! I != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'N': {
            auto* Mess = reinterpret_cast<RPII*>(ptr);
            if (Mess->header.type != 'N'){
                std::cout << "ERROR: Types do not match! N != " << Mess->header.type << "\n";
            }
            break;
        }
        case 'O': {
            auto* Mess = reinterpret_cast<DLCR_Price_Discovery*>(ptr);
            if (Mess->header.type != 'O'){
                std::cout << "ERROR: Types do not match! O != " << Mess->header.type << "\n";
            }
            break;
        }
        default:{
            std::cout << "ERROR: Unknown Message Type! : " << ptr[0] << std::endl;
            break;
        }
    }
}

int main(int argc, char* argv[]){

    if (argc != 2) {
        std::cout << "Second argument must be file path/filename." << std::endl;
        exit(EXIT_FAILURE);
    }

    const char* filepath = argv[1];

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

    std::uint8_t* mapped_file = reinterpret_cast<std::uint8_t*>(mmap((caddr_t)0, file_size, PROT_READ, MAP_SHARED, rdonly_file, 0));
    if(mapped_file == MAP_FAILED){
        std::cout << "Failed to mmap file.\n" << std::endl;
        close(rdonly_file);
        exit(EXIT_FAILURE);
    }
    close(rdonly_file);

    std::uint8_t* filePtr = mapped_file;
    const std::uint8_t* file_end = mapped_file + file_size;

    while (filePtr < file_end) {
    //for(int num = 0; num < 40; num++){
        const uint16_t message_l = __builtin_bswap16(*reinterpret_cast<const uint16_t*>(filePtr));
        filePtr += 2;

        parse_message(filePtr);
        filePtr += message_l;
    }

    if(munmap(mapped_file, file_size) == -1){
        std::cout << "Failed to munmap." << std::endl;
    }

    return 0;
}
