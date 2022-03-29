#pragma once
#include <library>

#define DIRECTORY_FLAG 0

namespace fs {

class FileSystem;
struct __Directory;

struct __FileDescriptor {
    __Directory* parent_directory;
    String name, type;
    unsigned int size;
    unsigned int flags: 16;
    Array<unsigned int> sectors;
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
    
    void resize(unsigned int new_size);
public:
    File(FileSystem* filesystem, __FileDescriptor* descriptor) : filesystem(filesystem), descriptor(descriptor) {}
    File() : filesystem(nullptr), descriptor(nullptr) {}
    
    bool operator==(const File& file);
    
    const String& getType();
    const String& getName();
    unsigned int getSize();
    Directory getParentDirectory();
    
    Array<unsigned char> read();
    void write(const Array<unsigned char>& array);
    
    bool isDirectory();
    bool exists();
};

class Directory : public File {
    friend File moveFile(String original_path, String new_path);
    friend File;
    __Directory* getDirectory();
    void flushMetadata();
public:
    Directory(FileSystem* filesystem, __Directory* descriptor) : File(filesystem, descriptor) {}
    Directory(File& file) : File(file) {}
    
    unsigned int getFileCount();
    File getFile(unsigned int index);
    File getFile(const String& name);
    void removeFile(unsigned int index);
    void removeFile(const String& name);
    void removeFile(fs::File file);
    
    void createFile(const String& name, const String& type);
};

void init();
FileSystem* getFileSystem();

File openFile(String path);
void deleteFile(const String& path);
File createFile(String path, const String& file_type);
File copyFile(const String& original_path, const String& copy_path);
File moveFile(String original_path, String new_path);

};
