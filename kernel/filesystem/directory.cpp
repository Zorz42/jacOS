#include "filesystem.hpp"
#include "text/text.hpp"

fs::__Directory* fs::Directory::getDirectory() {
    return (fs::__Directory*)descriptor;
}

unsigned int fs::Directory::getFileCount() {
    return getDirectory()->files.getSize();
}

fs::File fs::Directory::getFile(unsigned int index) {
    return File(filesystem, getDirectory()->files[index]);
}

fs::File fs::Directory::getFile(const String& name) {
    for(int i = 0; i < getFileCount(); i++) {
        File file = getFile(i);
        if(file.getName() == name) {
            return file;
        }
    }
    return File();
}

void fs::__Directory::load(fs::FileSystem* filesystem) {
    File dir_file = File(filesystem, this);
    unsigned char* dir_data = new unsigned char[dir_file.getSize()];
    dir_file.load(dir_data);
    
    for(int i = 0; i < dir_file.getSize();) {
        __FileDescriptor* file_descriptor = new __FileDescriptor;
        
        file_descriptor->name = (char*)&dir_data[i];
        i += file_descriptor->name.getSize() + 1;
        
        file_descriptor->type = (char*)&dir_data[i];
        i += file_descriptor->type.getSize() + 1;
        
        file_descriptor->size = *(unsigned int*)&dir_data[i];
        i += 4;
        
        file_descriptor->flags = *(unsigned short*)&dir_data[i];
        i += 2;
        
        for(int j = 0; j < (file_descriptor->size + 511) / 512; j++) {
            file_descriptor->sectors.push(*(unsigned int*)&dir_data[i]);
            i += 4;
        }
        
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
    for(int i = 0; i < getFileCount(); i++) {
        metadata.insert(getDirectory()->files[i]->serializeMetadata(), metadata.getSize());
    }
    save(&metadata[0], metadata.getSize());
}

void fs::Directory::removeFile(unsigned int index) {
    fs::File file = getFile(index);
    file.resize(0);
    delete getDirectory()->files[index];
    getDirectory()->files.erase(index);
    flushMetadata();
}

void fs::Directory::removeFile(const String& name) {
    for(int i = 0; i < getFileCount(); i++) {
        File file = getFile(i);
        if(file.getName() == name)
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

void fs::Directory::createFile(const String& name, const String& type) {
    File file = getFile(name);
    if(file.exists())
        return;
    
    __FileDescriptor* new_file = new __FileDescriptor();
    getDirectory()->files.push(new_file);
    
    new_file->parent_directory = getDirectory();
    new_file->name = name;
    new_file->type = type;
    new_file->flags = 0;
    
    fs::File new_file_obj(filesystem, new_file);
    new_file_obj.resize(0);
    
    flushMetadata();
}
