#include <cstdlib>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sys/mman.h>
#include <sys/stat.h>

int main(int argc, char* argv[]){
    int rdonly_file;
    off_t file_size;
    struct stat file_stats;

    if (argc != 2) {
        std::cout << "Second argument must be file path/filename." << std::endl;
        exit(EXIT_FAILURE);
    }

    const char* filepath = argv[1];

    rdonly_file = open(filepath, O_RDONLY);
    if (rdonly_file == -1){
        std::cout << "Failed to open file" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(fstat(rdonly_file, &file_stats) == -1){
        std::cout << "Failed to get file size." << std::endl;
        close(rdonly_file);
        exit(EXIT_FAILURE);
    }

    file_size = file_stats.st_size;
    std::cout << "Size of selected file (bytes): " << file_size << std::endl;

    std::uint8_t* mapped_file = static_cast<std::uint8_t*>(mmap((caddr_t)0, file_size, PROT_READ, MAP_SHARED, rdonly_file, 0));
    if(mapped_file == MAP_FAILED){
        std::cout << "Failed to mmap file.\n" << std::endl;
        close(rdonly_file);
        exit(EXIT_FAILURE);
    }
    close(rdonly_file);

    std::cout << std::hex;
    for(off_t i = 0; i < 16; i++){
        //std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(mapped_file[i]) << " ";
        std::cout << static_cast<int>(mapped_file[i]) << " ";
        //std::cout << mapped_file[i] << " ";
    }
    std::cout << std::endl;

    for(off_t j = 15; j >= 0; j--){
        //std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(mapped_file[file_size-1-j]) << " ";
        std::cout << static_cast<int>(mapped_file[file_size-1-j]) << " ";
        //std::cout << mapped_file[file_size-1-j] << " ";
    }
    std::cout << std::endl;
    std::cout << std::dec;
    for(off_t offset = 0; offset < file_size; offset++){
        std::cout << static_cast<int>(mapped_file[offset]) + static_cast<int>(mapped_file[offset+1]);
        break;
    }

    std::cout << std::endl;

    if(munmap(mapped_file, file_size) == -1){
        std::cout << "Failed to munmap." << std::endl;
    }

    std::cout << "DONE." << std::endl;
    return 0;
}
