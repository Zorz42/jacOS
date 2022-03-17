#include "gdt.hpp"

struct GdtEntry {
    unsigned short limit_low;           // The lower 16 bits of the limit.
    unsigned short base_low;            // The lower 16 bits of the base.
    unsigned char  base_middle;         // The next 8 bits of the base.
    unsigned char  access;              // Access flags, determine what ring this segment can be used in.
    unsigned char  granularity;
    unsigned char  base_high;           // The last 8 bits of the base.
} __attribute__((packed));

struct GdtDescriptor {
    unsigned short limit;               // The upper 16 bits of all selector limits.
    unsigned int base;                  // The address of the first gdt_entry_t struct.
} __attribute__((packed));

struct TssEntry {
    unsigned int prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
    unsigned int esp0;       // The stack pointer to load when we change to kernel mode.
    unsigned int ss0;        // The stack segment to load when we change to kernel mode.
    unsigned int esp1;       // Unused...
    unsigned int ss1;
    unsigned int esp2;
    unsigned int ss2;
    unsigned int cr3;
    unsigned int eip;
    unsigned int eflags;
    unsigned int eax;
    unsigned int ecx;
    unsigned int edx;
    unsigned int ebx;
    unsigned int esp;
    unsigned int ebp;
    unsigned int esi;
    unsigned int edi;
    unsigned int es;         // The value to load into ES when we change to kernel mode.
    unsigned int cs;         // The value to load into CS when we change to kernel mode.
    unsigned int ss;         // The value to load into SS when we change to kernel mode.
    unsigned int ds;         // The value to load into DS when we change to kernel mode.
    unsigned int fs;         // The value to load into FS when we change to kernel mode.
    unsigned int gs;         // The value to load into GS when we change to kernel mode.
    unsigned int ldt;        // Unused...
    unsigned short trap;
    unsigned short iomap_base;
} __attribute__((packed));

static GdtEntry createGdtEntry(unsigned int base, unsigned int limit, unsigned char access, unsigned char gran) {
    GdtEntry gdt_entry;
    gdt_entry.base_low    = (base & 0xFFFF);
    gdt_entry.base_middle = (base >> 16) & 0xFF;
    gdt_entry.base_high   = (base >> 24) & 0xFF;

    gdt_entry.limit_low   = (limit & 0xFFFF);
    gdt_entry.granularity = (limit >> 16) & 0x0F;
    
    gdt_entry.granularity |= gran & 0xF0;
    gdt_entry.access      = access;
    return gdt_entry;
}

extern "C" void gdt_flush(unsigned int ptr);
extern "C" void tss_flush();

#define NUM_ENTRIES 6

static GdtEntry gdt_entries[NUM_ENTRIES];
static GdtDescriptor gdt_descriptor;
static TssEntry tss_entry;

static void writeTss(unsigned int num, unsigned short ss0, unsigned int esp0) {
    // Firstly, let's compute the base and limit of our entry into the GDT.
    unsigned int base = (unsigned int) &tss_entry;
    unsigned int limit = base + sizeof(tss_entry);
    
    // Now, add our TSS descriptor's address to the GDT.
    gdt_entries[num] = createGdtEntry(base, limit, 0xE9, 0x00);

    // Ensure the descriptor is initially zero.
    //memset(&tss_entry, 0, sizeof(tss_entry));

    tss_entry.ss0  = ss0;  // Set the kernel stack segment.
    tss_entry.esp0 = esp0; // Set the kernel stack pointer.
    
    // Here we set the cs, ss, ds, es, fs and gs entries in the TSS. These specify what
    // segments should be loaded when the processor switches to kernel mode. Therefore
    // they are just our normal kernel code/data segments - 0x08 and 0x10 respectively,
    // but with the last two bits set, making 0x0b and 0x13. The setting of these bits
    // sets the RPL (requested privilege level) to 3, meaning that this TSS can be used
    // to switch to kernel mode from ring 3.
    tss_entry.cs = 0x0b;
    tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x13;
}

void gdt::init() {
    gdt_descriptor.limit = (sizeof(GdtEntry) * NUM_ENTRIES) - 1;
    gdt_descriptor.base  = (unsigned int)&gdt_entries;
    
    gdt_entries[0] = createGdtEntry(0, 0, 0, 0);                // Null segment
    gdt_entries[1] = createGdtEntry(0, 0xFFFFFFFF, 0b10011010, 0xCF); // Code segment
    gdt_entries[2] = createGdtEntry(0, 0xFFFFFFFF, 0b10010010, 0xCF); // Data segment
    gdt_entries[3] = createGdtEntry(0, 0xFFFFFFFF, 0b11111010, 0xCF); // User code segment
    gdt_entries[4] = createGdtEntry(0, 0xFFFFFFFF, 0b11110010, 0xCF); // User data segment
    writeTss(5, 0x10, 0x7c00);
    
    gdt_flush((unsigned int)&gdt_descriptor);
    tss_flush();
}
