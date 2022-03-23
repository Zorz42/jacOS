#include "filesystem.hpp"
#include "disk/disk.hpp"

fs::FileSystem *file_system;

void fs::init() {
    file_system = new fs::FileSystem;
    
    for(int disk_id = 0; disk_id < disks::getNumDisks(); disk_id++)
        if(file_system->mount(disk_id))
            break;
}

fs::FileSystem* fs::getFileSystem() {
    return file_system;
}

fs::Directory fs::FileSystem::getRootDirectory() {
    return Directory(this, &root);
}

unsigned int fs::FileSystem::getDiskId() {
    return disk_id;
}

unsigned int fs::FileSystem::getSectorsTaken() {
    return sectors_taken;
}

bool fs::FileSystem::mount(unsigned int disk_id_) {
    disk_id = disk_id_;
    disks::Disk disk = disks::getDisk(disk_id);
    
    __Sector* first_sector = new __Sector;
    disk.read(0, 1, first_sector->bytes);
    bool is_mountable = *(unsigned int*)(first_sector->bytes + 508) == 0xAABBCCDD;
    unsigned int root_sector = *(unsigned int*)&first_sector->bytes[0], root_size = *(unsigned int*)&first_sector->bytes[4];
    delete first_sector;
    
    if(!is_mountable)
        return false;
    
    num_sector_bits = (disk.size - 1) / 8 / 512 + 1;
    
    unsigned char* sector_bits_ = new unsigned char[512 * num_sector_bits];
    disk.read(1, num_sector_bits, sector_bits_);
    
    sector_bits = sector_bits_;
    sectors_taken = 0;
    for(int i = 0; i < disk.size; i++)
        if(getSectorBit(i))
            sectors_taken++;
    
    root.name = new char[1];
    root.name[0] = 0;
    
    root.type = new char[1];
    root.type[0] = 0;
    
    root.sector = root_sector;
    root.size = root_size;
    
    root.flags = 0;
    
    root.load(this);
    
    return true;
}

void fs::FileSystem::setSectorBit(unsigned int sector_index, bool value) {
    int byte = sector_index / 8;
    if(value != getSectorBit(sector_index)) {
        if(value) {
            sectors_taken++;
            sector_bits[byte] |= 1 << (sector_index % 8);
        } else {
            sectors_taken--;
            sector_bits[byte] &= ~(1 << (sector_index % 8));
        }
    }
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
                if(((sector_bits[i] >> j) & 1) == 0)
                    return i * 8 + j;
    return -1;
}

void fs::FileSystem::flushSectorBits() {
    disks::Disk disk = disks::getDisk(disk_id);
    disk.write(1, num_sector_bits, sector_bits);
}
