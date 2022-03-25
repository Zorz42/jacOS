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
    
    metadata.push(sector & 0xFF);
    metadata.push((sector >> 8) & 0xFF);
    metadata.push((sector >> 16) & 0xFF);
    metadata.push((sector >> 24) & 0xFF);
    
    metadata.push(flags & 0xFF);
    metadata.push((flags >> 8) & 0xFF);
    
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
    
    unsigned int next_sector = descriptor->sector;
    unsigned int bytes_read = 0;
    
    while(bytes_read < getSize()) {
        disk.read(next_sector, 1, temp->bytes);
        next_sector = *(unsigned int*)(temp->bytes + 508);
        for(int i = 0; i < 508 && bytes_read < getSize(); i++) {
            iter[bytes_read] = temp->bytes[i];
            bytes_read++;
        }
    }
    
    delete temp;
}

void fs::File::save(void *ptr, unsigned int size) {
    resize(size);
    
    __Sector* temp = new __Sector;
    unsigned char* iter = (unsigned char*)ptr;
    
    disks::Disk disk = disks::getDisk(filesystem->getDiskId());
    
    unsigned int next_sector = descriptor->sector;
    unsigned int bytes_written = 0;
    
    while(bytes_written < getSize()) {
        disk.read(next_sector, 1, temp->bytes);
        for(int i = 0; i < 508 && bytes_written < getSize(); i++) {
            temp->bytes[i] = iter[bytes_written];
            bytes_written++;
        }
        disk.write(next_sector, 1, temp->bytes);
        
        next_sector = *(unsigned int*)(temp->bytes + 508);
    }
    
    delete temp;
}

void fs::File::resize(unsigned int new_size) {
    if(new_size == getSize())
        return;
    
    unsigned int old_size = getSize();
    unsigned int new_sector_count = (new_size + 507) / 508, old_sector_count = (old_size + 507) / 508;
    descriptor->size = new_size;
    
    disks::Disk disk = disks::getDisk(filesystem->getDiskId());
    
    __Sector* temp = new __Sector;
    
    if(new_sector_count < old_sector_count) {
        unsigned int curr_sector = descriptor->sector;
        for(int i = 0; i < old_sector_count; i++) {
            if(i >= new_sector_count)
                filesystem->setSectorBit(curr_sector, false);
            
            disk.read(curr_sector, 1, temp->bytes);
            curr_sector = *(unsigned int*)(temp->bytes + 508);
        }
    } else if(new_sector_count > old_sector_count) {
        unsigned int curr_sector = descriptor->sector;
        for(int i = 0; i < old_sector_count - 1; i++) {
            disk.read(curr_sector, 1, temp->bytes);
            curr_sector = *(unsigned int*)(temp->bytes + 508);
        }
        
        disk.read(curr_sector, 1, temp->bytes);
        unsigned int next_sector = filesystem->getFreeSector();
        filesystem->setSectorBit(next_sector, true);
        *(unsigned int*)(temp->bytes + 508) = next_sector;
        disk.write(curr_sector, 1, temp->bytes);
        curr_sector = next_sector;
        
        for(int i = 0; i < 512; i++)
            temp->bytes[i] = 0;
            
        for(int i = old_sector_count; i < new_sector_count - 1; i++) {
            unsigned int next_sector = filesystem->getFreeSector();
            filesystem->setSectorBit(next_sector, true);
            *(unsigned int*)(temp->bytes + 508) = next_sector;
            disk.write(curr_sector, 1, temp->bytes);
            curr_sector = next_sector;
        }
        
        disk.write(curr_sector, 1, temp->bytes);
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
