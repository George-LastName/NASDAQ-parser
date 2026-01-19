#include <bits/types/locale_t.h>
#include <cstdint>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include "message_types.h"

void system_message_decode(std::uint8_t* filePtr, off_t& offset);
void stock_directory_decode(std::uint8_t* filePtr, off_t& offset);
void parse_message(std::uint8_t* filePtr, off_t& offset);
void parse_message2(uint8_t* filePtr, off_t& offset);

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

    off_t file_size = file_stats.st_size;

    std::uint8_t* mapped_file = static_cast<std::uint8_t*>(mmap((caddr_t)0, file_size, PROT_READ, MAP_SHARED, rdonly_file, 0));
    if(mapped_file == MAP_FAILED){
        std::cout << "Failed to mmap file.\n" << std::endl;
        close(rdonly_file);
        exit(EXIT_FAILURE);
    }
    close(rdonly_file);

    off_t offset = 0;
    while (offset < file_size) {
    //for(int num = 0; num < 40; num++){
        uint16_t message_l = __builtin_bswap16(*reinterpret_cast<const uint16_t*>(mapped_file + offset));
        offset += 2;

        auto* header = reinterpret_cast<ITCH_Header*>(mapped_file + offset);

        /* TODO: For SIMULATION, check if able to send message, if not wait until able.
         *  while(sim_time > header->timestamp){
         *      wait;
         *  }
         */

        switch (header->type) {
            case 'S': {
                auto* Mess = reinterpret_cast<const Sys_Event*>(mapped_file + offset);
                if (Mess->header.type != 'S'){
                    std::cout << "ERROR: Types do not match! S != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'R': {
                auto* Mess = reinterpret_cast<const Stock_Dir*>(mapped_file + offset);
                if (Mess->header.type != 'R'){
                    std::cout << "ERROR: Types do not match! R != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'H': {
                auto* Mess = reinterpret_cast<Stock_Trading_Action*>(mapped_file + offset);
                if (Mess->header.type != 'H'){
                    std::cout << "ERROR: Types do not match! H != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'Y': {
                auto* Mess = reinterpret_cast<Reg_Sho_Restriction*>(mapped_file + offset);

                if (Mess->header.type != 'Y'){
                    std::cout << "ERROR: Types do not match! Y != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'L': {
                auto* Mess = reinterpret_cast<Market_Participant_Position*>(mapped_file + offset);
                if (Mess->header.type != 'L'){
                    std::cout << "ERROR: Types do not match! L != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'V': {
                auto* Mess = reinterpret_cast<MWCB_Decline_Level*>(mapped_file + offset);
                if (Mess->header.type != 'V'){
                    std::cout << "ERROR: Types do not match! V != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'W': {
                auto* Mess = reinterpret_cast<MWCB_Status*>(mapped_file + offset);
                if (Mess->header.type != 'W'){
                    std::cout << "ERROR: Types do not match! W != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'K': {
                auto* Mess = reinterpret_cast<Quoting_Period_Update*>(mapped_file + offset);
                if (Mess->header.type != 'K'){
                    std::cout << "ERROR: Types do not match! K != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'J': {
                auto* Mess = reinterpret_cast<LULD_Auction_Collar*>(mapped_file + offset);
                if (Mess->header.type != 'J'){
                    std::cout << "ERROR: Types do not match! J != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'h': {
                auto* Mess = reinterpret_cast<Operational_Halt*>(mapped_file + offset);
                if (Mess->header.type != 'h'){
                    std::cout << "ERROR: Types do not match! h != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'A': {
                auto* Mess = reinterpret_cast<Add_Order_No_MPID*>(mapped_file + offset);
                if (Mess->header.type != 'A'){
                    std::cout << "ERROR: Types do not match! A != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'F': {
                auto* Mess = reinterpret_cast<Add_Order_MPID*>(mapped_file + offset);
                if (Mess->header.type != 'F'){
                    std::cout << "ERROR: Types do not match! F != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'E': {
                auto* Mess = reinterpret_cast<Order_Executed*>(mapped_file + offset);
                if (Mess->header.type != 'E'){
                    std::cout << "ERROR: Types do not match! E != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'C': {
                auto* Mess = reinterpret_cast<Order_Executed_With_Price*>(mapped_file + offset);
                if (Mess->header.type != 'C'){
                    std::cout << "ERROR: Types do not match! C != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'X': {
                auto* Mess = reinterpret_cast<Order_Cancel*>(mapped_file + offset);
                if (Mess->header.type != 'X'){
                    std::cout << "ERROR: Types do not match! X != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'D': {
                auto* Mess = reinterpret_cast<Order_Delete*>(mapped_file + offset);
                if (Mess->header.type != 'D'){
                    std::cout << "ERROR: Types do not match! D != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'U': {
                auto* Mess = reinterpret_cast<Order_Replace*>(mapped_file + offset);
                if (Mess->header.type != 'U'){
                    std::cout << "ERROR: Types do not match! U != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'P': {
                auto* Mess = reinterpret_cast<Trade_Non_Cross*>(mapped_file + offset);
                if (Mess->header.type != 'P'){
                    std::cout << "ERROR: Types do not match! P != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'Q': {
                auto* Mess = reinterpret_cast<Cross_Trade*>(mapped_file + offset);
                if (Mess->header.type != 'Q'){
                    std::cout << "ERROR: Types do not match! Q != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'B': {
                auto* Mess = reinterpret_cast<Broken_Trade*>(mapped_file + offset);
                if (Mess->header.type != 'B'){
                    std::cout << "ERROR: Types do not match! B != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'I': {
                auto* Mess = reinterpret_cast<NOII*>(mapped_file + offset);
                if (Mess->header.type != 'I'){
                    std::cout << "ERROR: Types do not match! I != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'N': {
                auto* Mess = reinterpret_cast<RPII*>(mapped_file + offset);
                if (Mess->header.type != 'N'){
                    std::cout << "ERROR: Types do not match! N != " << Mess->header.type << "\n";
                }
                break;
            }
            case 'O': {
                auto* Mess = reinterpret_cast<DLCR_Price_Discovery*>(mapped_file + offset);
                if (Mess->header.type != 'O'){
                    std::cout << "ERROR: Types do not match! O != " << Mess->header.type << "\n";
                }
                break;
            }
            default:{
                std::cout << "ERROR: Unknown Message Type! : " << mapped_file[offset] << std::endl;
                break;
            }
        }



        parse_message(mapped_file, offset);
        offset += message_l;
    }

    if(munmap(mapped_file, file_size) == -1){
        std::cout << "Failed to munmap." << std::endl;
    }

    //std::cout << "\nDONE." << std::endl;
    return 0;
}
/*
void parse_message(uint8_t* filePtr, off_t& offset){

    char type = static_cast<char>(filePtr[offset]);
    auto ptr = filePtr + offset;

    switch (type) {
        case 'S': {
            auto* Mess = reinterpret_cast<const Sys_Event*>(ptr);
            break;
        }
        case 'R': {
            auto* Mess = reinterpret_cast<const Stock_Dir*>(ptr);
            break;
        }
        case 'H': {
            auto* Mess = reinterpret_cast<Stock_Trading_Action*>(ptr);
            break;
        }
        case 'Y': {
            auto* Mess = reinterpret_cast<Reg_Sho_Restriction*>(ptr);
            break;
        }
        case 'L': {
            auto* Mess = reinterpret_cast<Market_Participant_Position*>(ptr);
            break;
        }
        case 'V': {
            auto* Mess = reinterpret_cast<MWCB_Decline_Level*>(ptr);
            break;
        }
        case 'W': {
            auto* Mess = reinterpret_cast<MWCB_Status*>(ptr);
            break;
        }
        case 'K': {
            auto* Mess = reinterpret_cast<Quoting_Period_Update*>(ptr);
            break;
        }
        case 'J': {
            auto* Mess = reinterpret_cast<LULD_Auction_Collar*>(ptr);
            break;
        }
        case 'h': {
            auto* Mess = reinterpret_cast<Operational_Halt*>(ptr);
            break;
        }
        case 'A': {
            auto* Mess = reinterpret_cast<Add_Order_No_MPID*>(ptr);
            break;
        }
        case 'F': {
            auto* Mess = reinterpret_cast<Add_Order_MPID*>(ptr);
            break;
        }
        case 'E': {
            auto* Mess = reinterpret_cast<Order_Executed*>(ptr);
            break;
        }
        case 'C': {
            auto* Mess = reinterpret_cast<Order_Executed_With_Price*>(ptr);
            break;
        }
        case 'X': {
            auto* Mess = reinterpret_cast<Order_Cancel*>(ptr);
            break;
        }
        case 'D': {
            auto* Mess = reinterpret_cast<Order_Delete*>(ptr);
            break;
        }
        case 'U': {
            auto* Mess = reinterpret_cast<Order_Replace*>(ptr);
            break;
        }
        case 'P': {
            auto* Mess = reinterpret_cast<Trade_Non_Cross*>(ptr);
            break;
        }
        case 'Q': {
            auto* Mess = reinterpret_cast<Cross_Trade*>(ptr);
            break;
        }
        case 'B': {
            auto* Mess = reinterpret_cast<Broken_Trade*>(ptr);
            break;
        }
        case 'I': {
            auto* Mess = reinterpret_cast<NOII*>(ptr);
            break;
        }
        case 'N': {
            auto* Mess = reinterpret_cast<RPII*>(ptr);
            break;
        }
        case 'O': {
            auto* Mess = reinterpret_cast<DLCR_Price_Discovery*>(ptr);
            break;
        }
        default:{
            std::cout << "ERROR: Unknown Message Type! : " << filePtr[offset] << std::endl;
            break;
        }
    }
}*/
