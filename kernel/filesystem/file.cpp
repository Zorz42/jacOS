#include "filesystem.hpp"
#include "disk/disk.hpp"
#include "text/text.hpp"

bool fs::__FileDescriptor::getFlag(int flag) {
    return ((flags << flag) & 1) != 0;
}

Array<unsigned char> fs::__FileDescriptor::serializeMetadata() {
    Array<unsigned char> metadata;
    for(int i = 0; i < name.getSize(); i++)
        metadata.push(name[i]);
    metadata.push(0);
    for(int i = 0; i < type.getSize(); i++)
        metadata.push(type[i]);
    metadata.push(0);
    
    metadata.push(size & 0xFF);
    metadata.push((size >> 8) & 0xFF);
    metadata.push((size >> 16) & 0xFF);
    metadata.push((size >> 24) & 0xFF);
    
    metadata.push(flags & 0xFF);
    metadata.push((flags >> 8) & 0xFF);
    
    for(int i = 0; i < sectors.getSize(); i++) {
        metadata.push(sectors[i] & 0xFF);
        metadata.push((sectors[i] >> 8) & 0xFF);
        metadata.push((sectors[i] >> 16) & 0xFF);
        metadata.push((sectors[i] >> 24) & 0xFF);
    }
    
    return metadata;
}

const String& fs::File::getType() {
    return descriptor->type;
}

const String& fs::File::getName() {
    return descriptor->name;
}

unsigned int fs::File::getSize() {
    return descriptor->size;
}

bool fs::File::isDirectory() {
    return descriptor->getFlag(DIRECTORY_FLAG);
}

bool fs::File::exists() {
    return descriptor != nullptr;
}

fs::Directory fs::File::getParentDirectory() {
    return Directory(filesystem, descriptor->parent_directory);
}


void fs::File::load(void *ptr) {
    __Sector* temp = new __Sector;
    unsigned char* iter = (unsigned char*)ptr;
    
    disks::Disk disk = disks::getDisk(filesystem->getDiskId());
    
    unsigned int curr_sector = 0;
    unsigned int bytes_read = 0;
    
    while(bytes_read < getSize()) {
        disk.read(descriptor->sectors[curr_sector], 1, temp->bytes);
        for(int i = 0; i < 512 && bytes_read < getSize(); i++) {
            iter[bytes_read] = temp->bytes[i];
            bytes_read++;
        }
        curr_sector++;
    }
    
    delete temp;
}

void fs::File::save(void *ptr, unsigned int size) {
    resize(size);
    
    __Sector* temp = new __Sector;
    unsigned char* iter = (unsigned char*)ptr;
    
    disks::Disk disk = disks::getDisk(filesystem->getDiskId());
    
    unsigned int curr_sector = 0;
    unsigned int bytes_written = 0;
    
    while(bytes_written < getSize()) {
        for(int i = 0; i < 512 && bytes_written < getSize(); i++) {
            temp->bytes[i] = iter[bytes_written];
            bytes_written++;
        }
        disk.write(descriptor->sectors[curr_sector], 1, temp->bytes);
        
        curr_sector++;
    }
    
    delete temp;
}

void fs::File::resize(unsigned int new_size) {
    if(new_size == getSize())
        return;
    
    unsigned int old_size = getSize();
    unsigned int new_sector_count = (new_size + 511) / 512, old_sector_count = (old_size + 511) / 512;
    descriptor->size = new_size;
    
    disks::Disk disk = disks::getDisk(filesystem->getDiskId());
    
    __Sector* temp = new __Sector;
    
    if(new_sector_count < old_sector_count) {
        while(descriptor->sectors.getSize() > new_sector_count) {
            filesystem->setSectorBit(descriptor->sectors[descriptor->sectors.getSize() - 1], false);
            descriptor->sectors.pop();
        }
    } else if(new_sector_count > old_sector_count) {
        while(descriptor->sectors.getSize() < new_sector_count) {
            unsigned int new_sector = filesystem->getFreeSector();
            filesystem->setSectorBit(new_sector, true);
            descriptor->sectors.push(new_sector);
        }
    }
    
    if(getParentDirectory().descriptor == descriptor)
        filesystem->flushRootMetadata();
    else;
        getParentDirectory().flushMetadata();
    filesystem->flushSectorBits();
    delete temp;
}

bool fs::File::operator==(const File& file) {
    return descriptor == file.descriptor;
}
