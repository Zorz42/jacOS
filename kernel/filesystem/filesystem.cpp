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

void fs::File::resize(unsigned int new_size) {
    unsigned int old_size = getSize();
    unsigned int new_sector_count = new_size / 508, old_sector_count = old_size / 508;
    head->size = new_size;
    
    unsigned char* temp = (unsigned char*)mem::alloc(512);
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
    
    unsigned char* first_sector = (unsigned char*)mem::alloc(512);
    disk.read(0, 1, first_sector);
    bool is_mountable = *(unsigned int*)(first_sector + 508) == 0xAABBCCDD;
    mem::free(first_sector);
    
    if(!is_mountable)
        return false;
    
    int num_sector_bits = (disk.size - 1) / 8 / 512 + 1;
    
    unsigned char* sector_bits_ = (unsigned char*)mem::alloc(512 * num_sector_bits);
    disk.read(1, num_sector_bits, sector_bits_);
    
    sector_bits = sector_bits_;
    
    file_pointers = (unsigned int*)mem::alloc(512);
    disk.read(1 + num_sector_bits, 1, file_pointers);
    
    file_count = 0;
    while(file_pointers[file_count] != 0)
        file_count++;
    
    file_heads = (__FileHead*)mem::alloc(file_count * sizeof(__FileHead));
    
    for(int file_index = 0; file_index < file_count; file_index++)
        disk.read(file_pointers[file_index], 1, &file_heads[file_index]);
    
    return true;
}

void fs::FileSystem::setSectorBit(unsigned int sector_index, bool value) {
    int byte = sector_index / 8;
    if(value)
        sector_bits[byte] |= 1 << (sector_index % 8);
    else
        sector_bits[byte] &= ~(1 << (sector_index % 8));
}

bool fs::FileSystem::getSectorBit(unsigned int sector_index) {
    int byte = sector_index / 8;
    return (sector_bits[byte] >> (sector_index % 8)) & 1 != 0;
}

unsigned int fs::FileSystem::getFreeSector() {
    disks::Disk disk = disks::getDisk(disk_id);
    for(int i = 0; i < disk.size / 8; i++)
        if(sector_bits[i] != 0xFF)
            for(int j = 0; j < 8; j++)
                if((sector_bits[i] >> j) & 1 == 0)
                    return i * 8 + j;
    return -1;
}
