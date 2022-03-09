#pragma once
#include "types.hpp"

namespace mem {

struct PageHead {
    u32 present  : 1;   // Page present in memory
    u32 rw       : 1;   // Read-only if clear, readwrite if set
    u32 user     : 1;   // Supervisor level only if clear
    u32 accessed : 1;   // Has the page been accessed since last refresh?
    u32 dirty    : 1;   // Has the page been written to since last refresh?
    u32 unused   : 7;   // Amalgamation of unused and reserved bits
    u32 frame    : 20;  // Frame address (shifted right 12 bits)
} __attribute__((packed));

struct PageTable {
    PageHead pages[1024];
} __attribute__((packed));

struct PageDirectory {
    PageTable tables[1024];
    u32 physical_table_addresses[1024];
    bool tables_allocated[1024];
} __attribute__((packed));

void init();
unsigned int getUsed();
unsigned int getTotal();
unsigned int getFree();

void* alloc(u32 size);
void free(void* ptr);

void allocateFrame(PageHead* page_head, bool is_kernel, bool is_writable);
void freeFrame(PageHead* page_head);
void switchPageDirectory(PageDirectory* page_directory);
PageHead* getPage(unsigned int address, PageDirectory* page_directory=nullptr);
void identityMapPage(unsigned int address, bool is_kernel, bool is_writable, PageDirectory* page_directory=nullptr);
unsigned int virtualToPhysicalAddress(unsigned int virtual_address, PageDirectory* page_directory=nullptr);

}
