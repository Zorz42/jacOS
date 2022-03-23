#pragma once
#include "array/array.hpp"

#define DIRECTORY_FLAG 0

namespace fs {

class FileSystem;
struct __Directory;

struct __FileDescriptor {
    __Directory* parent_directory;
    char *name, *type;
    unsigned int sector, size;
    unsigned int flags: 16;
    bool getFlag(int flag);
    Array<unsigned char> serializeMetadata();
};

struct __Directory : __FileDescriptor {
    __Directory(__FileDescriptor& file_descriptor) : __FileDescriptor(file_descriptor) {}
    __Directory() = default;
    Array<__FileDescriptor*> files;
    void load(FileSystem* filesystem);
};

struct __Sector {
    char bytes[512];
};

class File;
class Directory;

class FileSystem {
    friend File;
    unsigned int disk_id;
    unsigned char* sector_bits;
    unsigned int num_sector_bits;
    unsigned int sectors_taken;
    __Directory root;
    
    void flushRootMetadata();
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
    
    bool operator==(const File& file);
    
    const char* getType();
    const char* getName();
    unsigned int getSize();
    void resize(unsigned int new_size);
    Directory getParentDirectory();
    
    void load(void* ptr);
    void save(void* ptr);
    
    bool isDirectory();
    bool exists();
};

class Directory : public File {
    friend File;
    __Directory* getDirectory();
    void flushMetadata();
public:
    Directory(FileSystem* filesystem, __Directory* descriptor) : File(filesystem, descriptor) {}
    Directory(File& file) : File(file) {}
    
    unsigned int getFileCount();
    File getFile(unsigned int index);
    File getFile(const char* name);
    void removeFile(unsigned int index);
    void removeFile(const char* name);
    void removeFile(fs::File file);
};

void init();
FileSystem* getFileSystem();

};
