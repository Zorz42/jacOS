#include "filesystem.hpp"
#include "memory/memory.hpp"
#include "disk/disk.hpp"
#include "text/text.hpp"
#include "qemuDebug/debug.hpp"
#include "ports/ports.hpp"
#include "timer/timer.hpp"

fs::FileSystem file_system;

void fs::init() {
    for(int disk_id = 0; disk_id < disks::getNumDisks(); disk_id++)
        if(file_system.mount(disk_id))
            break;
}

fs::FileSystem* fs::getFileSystem() {
    return &file_system;
}

const char* fs::File::getType() {
    return head->type;
}

const char* fs::File::getName() {
    return head->name;
}

unsigned int fs::File::getSize() {
    return head->size;
}

void fs::File::load(void *ptr) {
    unsigned char* temp = (unsigned char*)mem::alloc(512);
    unsigned char* iter = (unsigned char*)ptr;
    
    disks::Disk disk = disks::getDisk(filesystem->getDiskId());
    
    unsigned int next_sector = head->data_sector;
    unsigned int bytes_read = 0;
    
    while(bytes_read < getSize()) {
        disk.read(next_sector, 1, temp);
        next_sector = *(unsigned int*)(temp + 508);
        for(int i = 0; i < 508 && bytes_read < getSize(); i++) {
            iter[bytes_read] = temp[i];
            bytes_read++;
        }
    }
    
    mem::free(temp);
}

void fs::File::save(void *ptr) {
    unsigned char* temp = (unsigned char*)mem::alloc(512);
    unsigned char* iter = (unsigned char*)ptr;
    
    disks::Disk disk = disks::getDisk(filesystem->getDiskId());
    
    unsigned int next_sector = head->data_sector;
    unsigned int bytes_written = 0;
    
    while(bytes_written < getSize()) {
        int time = timer::getTicks();
        while(timer::getTicks() == time); // wait 1 ms
        
        disk.read(next_sector, 1, temp);
        for(int i = 0; i < 508 && bytes_written < getSize(); i++) {
            temp[i] = iter[bytes_written];
            bytes_written++;
        }
        disk.write(next_sector, 1, temp);
        
        next_sector = *(unsigned int*)(temp + 508);
    }
    
    mem::free(temp);
}

fs::File fs::FileSystem::getFile(unsigned int index) {
    return File(this, &file_heads[index]);
}

unsigned int fs::FileSystem::getFileCount() {
    return file_count;
}

unsigned int fs::FileSystem::getDiskId() {
    return disk_id;
}

bool fs::FileSystem::mount(unsigned int disk_id_) {
    disk_id = disk_id_;
    disks::Disk disk = disks::getDisk(disk_id);
    int num_sector_bits = (disk.size - 1) / 8 / 508 + 1;
    
    void* sector_bits_ = mem::alloc(512 * num_sector_bits);
    disk.read(0, num_sector_bits, sector_bits_);
    
    if(*(unsigned int*)((unsigned char*)sector_bits_ + 508) != 0xAABBCCDD) {
        mem::free(sector_bits_);
        return false;
    }
    
    sector_bits = sector_bits;
    
    file_pointers = (unsigned int*)mem::alloc(512);
    disk.read(num_sector_bits, 1, file_pointers);
    
    file_count = 0;
    while(file_pointers[file_count] != 0)
        file_count++;
    
    file_heads = (__FileHead*)mem::alloc(file_count * sizeof(__FileHead));
    
    for(int file_index = 0; file_index < file_count; file_index++)
        disk.read(file_pointers[file_index], 1, &file_heads[file_index]);
    
    
    return true;
}
