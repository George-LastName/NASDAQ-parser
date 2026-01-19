#include <bits/types/locale_t.h>
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

        // Timestamp: 5, 6
        size_t tsmp_offs = 5;
        size_t tsmp_size = 6;
        std::uint64_t timestamp = 0;
        auto time_span = std::span(mapped_file + offset+tsmp_offs, tsmp_size);

        for(std::uint8_t t :time_span){
            timestamp = (timestamp << 8) | t;
        }
        std::cout << timestamp << std::endl;

        parse_message2(mapped_file, offset);
        std::cout << std::endl;
        offset += (message_l);
    }
    std::cout << std::dec;

    if(munmap(mapped_file, file_size) == -1){
        std::cout << "Failed to munmap." << std::endl;
    }

    std::cout << "DONE." << std::endl;
    return 0;
}

struct [[gnu::packed]] ITCH_Header {
    char type;
    uint16_t locate;
    uint16_t track_num;
    uint8_t timestamp[6];

    uint64_t get_timestamp() const{
        uint64_t ti_st = 0;
        for(uint8_t t : timestamp){
            ti_st = (ti_st << 8) | t;
        }
        return ti_st;
    }
};

struct [[gnu::packed]] Sys_Event {
    ITCH_Header header;

    char event_code;
};

struct [[gnu::packed]] Stock_Dir {
    ITCH_Header header;

    char stock[8];
    char market_cat;
    char fin_stat_ind;
    uint32_t rnd_lot_size;
    char rnd_lots_only;
    char issue_class;
    char issue_sub[2];
    char auth;
    char srt_thr_ind;
    char IPO_flg;
    char LULD_ref;
    char ETP_flg;
    uint32_t ETP_lev;
    char inv_ind;
};

void parse_message2(uint8_t* filePtr, off_t& offset){
    //
    char type = static_cast<char>(filePtr[offset]);
    auto* Mess;
    switch (type) {
        case 'S': {
            Mess = reinterpret_cast<const Sys_Event*>(filePtr+offset);
            std::cout << Mess->header.get_timestamp() << std::endl;
            break;
        }
        case 'R': {
            Mess = reinterpret_cast<const Stock_Dir*>(filePtr+offset);
            std::cout << Mess->header.get_timestamp() << std::endl;
            std::cout << Mess->stock << std::endl;
            std::cout << Mess->market_cat << std::endl;
            break;
        }
        default:{
            std::cout << "ERROR: Unknown Message Type!" << std::endl;
            break;
        }
    }
}
