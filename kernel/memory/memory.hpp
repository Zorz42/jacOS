#pragma once

namespace mem {

struct PageHead {
    unsigned int present  : 1;   // Page present in memory
    unsigned int rw       : 1;   // Read-only if clear, readwrite if set
    unsigned int user     : 1;   // Supervisor level only if clear
    unsigned int accessed : 1;   // Has the page been accessed since last refresh?
    unsigned int dirty    : 1;   // Has the page been written to since last refresh?
    unsigned int unused   : 7;   // Amalgamation of unused and reserved bits
    unsigned int frame    : 20;  // Frame address (shifted right 12 bits)
} __attribute__((packed));

struct PageTable {
    PageHead pages[1024];
} __attribute__((packed));

struct PageDirectory {
    PageTable tables[1024];
    unsigned int physical_table_addresses[1024];
    bool tables_allocated[1024];
} __attribute__((packed));

void init();
unsigned int getUsed();
unsigned int getTotal();
unsigned int getFree();

void* alloc(unsigned int size);
void free(void* ptr);

void allocateFrame(PageHead* page_head, bool is_kernel, bool is_writable);
void freeFrame(PageHead* page_head);
void switchPageDirectory(PageDirectory* page_directory);
PageHead* getPage(unsigned int address, PageDirectory* page_directory=nullptr);
void identityMapPage(unsigned int address, bool is_kernel, bool is_writable, PageDirectory* page_directory=nullptr);
unsigned int virtualToPhysicalAddress(unsigned int virtual_address, PageDirectory* page_directory=nullptr);

}
