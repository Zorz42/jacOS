#include "filesystem.hpp"
#include "disk/disk.hpp"

bool fs::__FileDescriptor::getFlag(int flag) {
    return ((flags << flag) & 1) != 0;
}

const char* fs::File::getType() {
    return descriptor->type;
}

const char* fs::File::getName() {
    return descriptor->name;
}

unsigned int fs::File::getSize() {
    return descriptor->size;
}

bool fs::File::isDirectory() {
    return descriptor->getFlag(DIRECTORY_FLAG);
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

void fs::File::save(void *ptr) {
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
        
        disk.read(curr_sector, 1, temp);
        unsigned int next_sector = filesystem->getFreeSector();
        filesystem->setSectorBit(next_sector, true);
        *(unsigned int*)(temp + 508) = next_sector;
        disk.write(curr_sector, 1, temp);
        curr_sector = next_sector;
        
        for(int i = 0; i < 512; i++)
            temp->bytes[i] = 0;
            
        for(int i = old_sector_count; i < new_sector_count - 1; i++) {
            unsigned int next_sector = filesystem->getFreeSector();
            filesystem->setSectorBit(next_sector, true);
            *(unsigned int*)(temp + 508) = next_sector;
            disk.write(curr_sector, 1, temp);
            curr_sector = next_sector;
        }
        
        disk.write(curr_sector, 1, temp);
    }
    
    //disk.write(descriptor->sector, 1, descriptor);
    filesystem->flushSectorBits();
    
    delete temp;
}
