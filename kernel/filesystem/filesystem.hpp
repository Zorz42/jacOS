#pragma once
#include "array/array.hpp"

namespace fs {

struct __FileHead {
    char type[12], name[490];
    unsigned int flags: 16;
    unsigned int size;
    unsigned int data_sector;
    // not on disk
    unsigned int sector;
} __attribute__((packed));

class File;

class FileSystem {
    unsigned int disk_id;
    unsigned char* sector_bits;
    unsigned int* file_pointers;
    unsigned int num_sector_bits;
    unsigned int sectors_taken;
    Array<__FileHead> file_heads;
public:
    bool mount(unsigned int disk_id_);
    File getFile(unsigned int index);
    unsigned int getFileCount();
    unsigned int getDiskId();
    
    void setSectorBit(unsigned int sector_index, bool value);
    bool getSectorBit(unsigned int sector_index);
    unsigned int getFreeSector();
    void flushSectorBits();
    
    unsigned int getSectorsTaken();
};

class File {
    FileSystem* filesystem;
    __FileHead* head;
public:
    File(FileSystem* filesystem, __FileHead* head) : filesystem(filesystem), head(head) {}
    File() : filesystem(nullptr), head(nullptr) {}
    
    const char* getType();
    const char* getName();
    unsigned int getSize();
    void resize(unsigned int new_size);
    
    void load(void* ptr);
    void save(void* ptr);
};

void init();
FileSystem* getFileSystem();

};
