#include "filesystem.hpp"

static bool strcmp(const char* a, const char* b) {
    for(int i = 0; a[i] != 0 || b[i] != 0; i++)
        if(a[i] != b[i])
            return false;
    return true;
}

fs::__Directory* fs::Directory::getDirectory() {
    return (fs::__Directory*)descriptor;
}

unsigned int fs::Directory::getFileCount() {
    return getDirectory()->files.getSize();
}

fs::File fs::Directory::getFile(unsigned int index) {
    return File(filesystem, getDirectory()->files[index]);
}

fs::File fs::Directory::getFile(const char* name) {
    for(int i = 0; i < getFileCount(); i++) {
        File file = getFile(i);
        if(strcmp(file.getName(), name))
            return file;
    }
    return File();
}

void fs::__Directory::load(fs::FileSystem* filesystem) {
    File dir_file = File(filesystem, this);
    unsigned char* dir_data = new unsigned char[dir_file.getSize()];
    dir_file.load(dir_data);
    
    for(int i = 0; i < dir_file.getSize();) {
        __FileDescriptor* file_descriptor = new __FileDescriptor;
        
        int name_len = 0;
        while(dir_data[i + name_len] != 0)
            name_len++;
        file_descriptor->name = new char[name_len + 1];
        for(int j = 0; j <= name_len; j++)
            file_descriptor->name[j] = dir_data[i + j];
        i += name_len + 1;
        
        int type_len = 0;
        while(dir_data[i + type_len] != 0)
            type_len++;
        file_descriptor->type = new char[type_len + 1];
        for(int j = 0; j <= type_len; j++)
            file_descriptor->type[j] = dir_data[i + j];
        i += type_len + 1;
        
        file_descriptor->size = *(unsigned int*)&dir_data[i];
        i += 4;
        file_descriptor->sector = *(unsigned int*)&dir_data[i];
        i += 4;
        file_descriptor->flags = *(unsigned short*)&dir_data[i];
        i += 2;
        
        file_descriptor->parent_directory = this;
        
        if(file_descriptor->getFlag(DIRECTORY_FLAG)) {
            __Directory* directory_desc = new __Directory(*file_descriptor);
            delete file_descriptor;
            directory_desc->load(filesystem);
            files.push(directory_desc);
        } else
            files.push(file_descriptor);
    }
}

void fs::Directory::flushMetadata() {
    Array<unsigned char> metadata;
    for(int i = 0; i < getFileCount(); i++)
        metadata.insert(getDirectory()->files[i]->serializeMetadata(), metadata.getSize());
    save(&metadata[0], metadata.getSize());
}

void fs::Directory::removeFile(unsigned int index) {
    fs::File file = getFile(index);
    file.resize(0);
    delete getDirectory()->files[index];
    getDirectory()->files.erase(index);
    flushMetadata();
}

void fs::Directory::removeFile(const char* name) {
    for(int i = 0; i < getFileCount(); i++) {
        File file = getFile(i);
        if(strcmp(file.getName(), name))
            removeFile(i);
    }
}

void fs::Directory::removeFile(fs::File file) {
    for(int i = 0; i < getFileCount(); i++) {
        File file_ = getFile(i);
        if(file_ == file)
            removeFile(i);
    }
}
