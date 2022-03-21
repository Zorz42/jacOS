#pragma once

namespace fs {

struct __FileHead {
    char type[12], name[490];
    unsigned int flags: 16;
    unsigned int size;
    unsigned int data_sector;
} __attribute__((packed));

class File;

class FileSystem {
    unsigned int disk_id;
    unsigned char* sector_bits;
    unsigned int* file_pointers;
    __FileHead *file_heads;
    unsigned int file_count;
    
    void setSectorBit(unsigned int sector_index, bool value);
    bool getSectorBit(unsigned int sector_index);
    unsigned int getFreeSector();
public:
    bool mount(unsigned int disk_id_);
    File getFile(unsigned int index);
    unsigned int getFileCount();
    unsigned int getDiskId();
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
    
    void load(void* ptr);
    void save(void* ptr);
};

void init();
FileSystem* getFileSystem();

};
