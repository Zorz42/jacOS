#include "filesystem.hpp"
#include "memory/memory.hpp"
#include "disk/disk.hpp"
#include "text/text.hpp"
#include "qemuDebug/debug.hpp"
#include "ports/ports.hpp"
#include "timer/timer.hpp"

fs::FileSystem *file_system;

bool fs::__FileDescriptor::getFlag(int flag) {
    return ((flags << flag) & 1) != 0;
}

void fs::init() {
    file_system = new fs::FileSystem;
    
    for(int disk_id = 0; disk_id < disks::getNumDisks(); disk_id++)
        if(file_system->mount(disk_id))
            break;
}

fs::FileSystem* fs::getFileSystem() {
    return file_system;
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

fs::__Directory* fs::Directory::getDirectory() {
    return (fs::__Directory*)descriptor;
}

unsigned int fs::Directory::getFileCount() {
    return getDirectory()->files.getSize();
}

fs::File fs::Directory::getFile(unsigned int index) {
    return File(filesystem, getDirectory()->files[index]);
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

void fs::FileSystem::loadDirectory(fs::__Directory* directory) {
    File dir_file = File(this, directory);
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
        
        if(file_descriptor->getFlag(DIRECTORY_FLAG)) {
            __Directory* directory_desc = new __Directory(*file_descriptor);
            delete file_descriptor;
            loadDirectory(directory_desc);
            directory->files.push(directory_desc);
        } else
            directory->files.push(file_descriptor);
    }
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
    
    loadDirectory(&root);
    
    //unsigned int* file_pointers = new unsigned int[512 / sizeof(unsigned int)];
    //disk.read(1 + num_sector_bits, 1, file_pointers);
    
    /*for(int file_index = 0; file_pointers[file_index] != 0; file_index++) {
        __FileHead* file_head = new __FileHead;
        disk.read(file_pointers[file_index], 1, file_head);
        file_head->sector = file_pointers[file_index];
        file_heads.push(file_head);
    }*/
    
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
