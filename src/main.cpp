#include <cstdint>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <span>
#include <string>

void system_message_decode(std::uint8_t* filePtr, off_t& offset);
void stock_directory_decode(std::uint8_t* filePtr, off_t& offset);
void parse_message(std::uint8_t* filePtr, off_t& offset);

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
    std::cout << "Size of selected file (bytes): " << file_size << std::endl;

    std::uint8_t* mapped_file = static_cast<std::uint8_t*>(mmap((caddr_t)0, file_size, PROT_READ, MAP_SHARED, rdonly_file, 0));
    if(mapped_file == MAP_FAILED){
        std::cout << "Failed to mmap file.\n" << std::endl;
        close(rdonly_file);
        exit(EXIT_FAILURE);
    }
    close(rdonly_file);

    off_t offset = 0;
    uint16_t message_l;

    //while (offset < file_size) {
    for(int num = 0; num < 2; num++){
        message_l = (static_cast<uint16_t>(mapped_file[offset]) << 8) | mapped_file[offset+1];

        std::cout << std::hex;
        offset += 2;

        for(off_t m = 0; m < message_l; m++){
            if (m==1 || m==3 || m==5 || m==11){
                //std::cout << " | ";
            }
            std::cout << static_cast<uint16_t>(mapped_file[offset+m]) << " ";
        }
        std::cout << "\n";
        parse_message(mapped_file, offset);
        std::cout << std::endl;
        //offset += (message_l -11);
    }
    std::cout << std::dec;

    if(munmap(mapped_file, file_size) == -1){
        std::cout << "Failed to munmap." << std::endl;
    }

    std::cout << "DONE." << std::endl;
    return 0;
}

void parse_message(std::uint8_t* filePtr, off_t& offset){
    // Decode Common Message Start

    // Name: Offset, Length.
    // Message Type: 0, 1
    std::cout << std::dec;
    char type = static_cast<char>(filePtr[offset]);
    std::cout << type << ", ";

    // Stock Locate: 1, 2
    int stk_loc = (static_cast<uint16_t>(filePtr[offset+1]) << 8) | filePtr[offset+2];
    std::cout << stk_loc << ", ";

    // Tracking Num: 3, 2
    int trk_num = (static_cast<uint16_t>(filePtr[offset+3]) << 8) | filePtr[offset+4];
    std::cout << trk_num << ", ";

    // Timestamp: 5, 6
    size_t tsmp_offs = 5;
    size_t tsmp_size = 6;
    std::uint64_t timestamp = 0;
    auto time_span = std::span(filePtr + offset+tsmp_offs, tsmp_size);

    for(std::uint8_t t :time_span){
        timestamp = (timestamp << 8) | t;
    }
    offset += 11;
    std::cout << timestamp;
    std::cout << ", ";

    switch(type){
        case 'S' : system_message_decode(filePtr, offset); break;
        case 'R' : stock_directory_decode(filePtr, offset); break;
        default: std::cout << "Unknown Message Type\n"; exit(EXIT_FAILURE); break;
    }
    std::cout << " ";
}

void system_message_decode(std::uint8_t* filePtr, off_t& offset){
    char event_code = static_cast<char>(filePtr[offset++]);
    std::cout << event_code;
}

void stock_directory_decode(std::uint8_t* filePtr, off_t& offset){
    std::uint8_t i = 0;
    for(; i<8 ; i++){
        if(filePtr[offset+i] == ' ') {break;}
    }
    std::string stock_sym(filePtr + offset, filePtr + offset + i);
    std::cout <<stock_sym << ", ";
    //offset += 8;

    char market_cat = filePtr[offset+8];
    std::cout <<market_cat << ", ";

    char fin_stat_indi = filePtr[offset+9];
    std::cout <<fin_stat_indi << ", ";

    std::uint64_t round_lot_size = 0;
    for(i = 0; i < 4; i++){
        round_lot_size = (round_lot_size << 8) | static_cast<std::uint8_t>(filePtr[offset+10+i]);
    }
    std::cout <<round_lot_size << ", ";

    char round_lots_only = filePtr[offset+14];
    std::cout <<round_lots_only << ", ";

    char issue_class = filePtr[offset+15];
    std::cout <<issue_class << ", ";

    for(i = 0; i<2 ; i++){
        if(filePtr[offset+16+i] == ' ') {break;}
    }
    std::string issue_sub(filePtr + offset+16, filePtr + offset + i+16);
    std::cout <<issue_sub << ", ";
}
