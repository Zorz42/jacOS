#pragma once
#include "array/array.hpp"

#define DIRECTORY_FLAG 0

namespace fs {

struct __FileDescriptor {
    char *name, *type;
    unsigned int sector, size;
    unsigned int flags: 16;
    bool getFlag(int flag);
};

struct __Directory : __FileDescriptor {
    __Directory(__FileDescriptor& file_descriptor) : __FileDescriptor(file_descriptor) {}
    __Directory() = default;
    Array<__FileDescriptor*> files;
};

struct __Sector {
    char bytes[512];
};

class File;
class Directory;

class FileSystem {
    unsigned int disk_id;
    unsigned char* sector_bits;
    unsigned int num_sector_bits;
    unsigned int sectors_taken;
    __Directory root;
    
    void loadDirectory(__Directory* directory);
public:
    bool mount(unsigned int disk_id_);
    unsigned int getDiskId();
    Directory getRootDirectory();
    
    void setSectorBit(unsigned int sector_index, bool value);
    bool getSectorBit(unsigned int sector_index);
    unsigned int getFreeSector();
    void flushSectorBits();
    
    unsigned int getSectorsTaken();
};

class File {
    friend Directory;
    FileSystem* filesystem;
    __FileDescriptor* descriptor;
public:
    File(FileSystem* filesystem, __FileDescriptor* descriptor) : filesystem(filesystem), descriptor(descriptor) {}
    File() : filesystem(nullptr), descriptor(nullptr) {}
    
    const char* getType();
    const char* getName();
    unsigned int getSize();
    void resize(unsigned int new_size);
    
    void load(void* ptr);
    void save(void* ptr);
    
    bool isDirectory();
};

class Directory : public File {
    __Directory* getDirectory();
public:
    Directory(FileSystem* filesystem, __Directory* descriptor) : File(filesystem, descriptor) {}
    Directory(File& file) : File(file) {}
    
    unsigned int getFileCount();
    File getFile(unsigned int index);
};

void init();
FileSystem* getFileSystem();

};
