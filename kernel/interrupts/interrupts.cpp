#include "interrupts.hpp"
#include "ports/ports.hpp"
#include "text/text.hpp"
#include "qemuDebug/debug.hpp"

static interrupts::IrqHandler irq_handlers[256];
static interrupts::SyscallHandler syscall_handlers[256];
static int syscall_handlers_size = 0;

struct IdtGate {
    unsigned short low_offset;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short high_offset;
} __attribute__((packed));

struct IdtRegister {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

#define IDT_ENTRIES 256
static IdtGate idt[IDT_ENTRIES];
static IdtRegister idt_reg;

static void setIdtGate(int n, unsigned int handler, bool user=false) {
    idt[n].low_offset = handler & 0xFFFF;
    idt[n].sel = 0x08;
    idt[n].always0 = 0;
    idt[n].flags = 0b10001110;
    if(user)
        idt[n].flags |= 0b01100000;
    idt[n].high_offset = (handler >> 16) & 0xFFFF;
}

extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();

extern "C" void irq0();
extern "C" void irq1();
extern "C" void irq2();
extern "C" void irq3();
extern "C" void irq4();
extern "C" void irq5();
extern "C" void irq6();
extern "C" void irq7();
extern "C" void irq8();
extern "C" void irq9();
extern "C" void irq10();
extern "C" void irq11();
extern "C" void irq12();
extern "C" void irq13();
extern "C" void irq14();
extern "C" void irq15();

extern "C" void systemCall();

unsigned int syscallShutdown(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    ports::wordOut(0x604, 0x2000);
    return 0;
}

void interrupts::init() {
    debug::out << "Mapping interrupt service routine functions" << debug::endl;
    const unsigned int isr_arr[32] = {(unsigned int)isr0, (unsigned int)isr1, (unsigned int)isr2, (unsigned int)isr3, (unsigned int)isr4, (unsigned int)isr5, (unsigned int)isr6, (unsigned int)isr7, (unsigned int)isr8, (unsigned int)isr9, (unsigned int)isr10, (unsigned int)isr11, (unsigned int)isr12, (unsigned int)isr13, (unsigned int)isr14, (unsigned int)isr15, (unsigned int)isr16, (unsigned int)isr17, (unsigned int)isr18, (unsigned int)isr19, (unsigned int)isr20, (unsigned int)isr21, (unsigned int)isr22, (unsigned int)isr23, (unsigned int)isr24, (unsigned int)isr25, (unsigned int)isr26, (unsigned int)isr27, (unsigned int)isr28, (unsigned int)isr29, (unsigned int)isr30, (unsigned int)isr31};
    for(int i = 0; i < 32; i++)
        setIdtGate(i, isr_arr[i]);
    
    debug::out << "Remapping programmable interrupt controllers" << debug::endl;
    ports::byteOut(0x20, 0x11);
    ports::byteOut(0xA0, 0x11);
    ports::byteOut(0x21, 0x20);
    ports::byteOut(0xA1, 0x28);
    ports::byteOut(0x21, 0x04);
    ports::byteOut(0xA1, 0x02);
    ports::byteOut(0x21, 0x01);
    ports::byteOut(0xA1, 0x01);
    ports::byteOut(0x21, 0x0);
    ports::byteOut(0xA1, 0x0); 
    
    debug::out << "Mapping interrupt request functions" << debug::endl;
    const unsigned int irq_arr[16] = {(unsigned int)irq0, (unsigned int)irq1, (unsigned int)irq2, (unsigned int)irq3, (unsigned int)irq4, (unsigned int)irq5, (unsigned int)irq6, (unsigned int)irq7, (unsigned int)irq8, (unsigned int)irq9, (unsigned int)irq10, (unsigned int)irq11, (unsigned int)irq12, (unsigned int)irq13, (unsigned int)irq14, (unsigned int)irq15};
    for(int i = 0; i < 16; i++)
        setIdtGate(i + 32, irq_arr[i]);
    
    setIdtGate(0x40, (unsigned int)systemCall, /*user*/true);
    
    debug::out << "Setting up interrupt descriptor table" << debug::endl;
    idt_reg.base = (unsigned int) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(IdtGate) - 1;
    asm volatile("lidtl (%0)" : : "r" (&idt_reg));
    
    debug::out << "Enabling interruptions" << debug::endl;
    asm volatile("sti");
    
    interrupts::registerSyscallHandler(&syscallShutdown, "shutdown");
}

void interrupts::registerIrqHandler(unsigned char n, IrqHandler handler) {
    irq_handlers[n] = handler;
}

static const char *exception_messages[] = {
    "Division By Zero",               // 0x00
    "Debug",                          // 0x01
    "Non Maskable Interrupt",         // 0x02
    "Breakpoint",                     // 0x03
    "Into Detected Overflow",         // 0x04
    "Out of Bounds",                  // 0x05
    "Invalid Opcode",                 // 0x06
    "No Coprocessor",                 // 0x07
    
    "Double Fault",                   // 0x08
    "Coprocessor Segment Overrun",    // 0x09
    "Bad TSS",                        // 0x0A
    "Segment Not Present",            // 0x0B
    "Stack Fault",                    // 0x0C
    "General Protection Fault",       // 0x0D
    "Page Fault",                     // 0x0E
    "Unknown Interrupt",              // 0x0F
    
    "Coprocessor Fault",              // 0x10
    "Alignment Check",                // 0x11
    "Machine Check",                  // 0x12
    "Heap Corruption",                // 0x13
    "Frame not free on identity map", // 0x14
    "Freeing Invalid Memory",         // 0x15
    "Freeing Freed Memory",           // 0x16
    "Cannot Find Free Region",        // 0x17
    
    "Ran out of free frames",         // 0x18
    "Ran out of syscall handlers",    // 0x19
    "Reserved",                       // 0x1a
    "Reserved",                       // 0x1b
    "Reserved",                       // 0x1c
    "Reserved",                       // 0x1d
    "Reserved",                       // 0x1e
    "Reserved"                        // 0x1f
};

extern "C" void irqHandler(Registers registers) {
    if(registers.int_no >= 40)
        ports::byteOut(0xA0, 0x20);
    ports::byteOut(0x20, 0x20);

    if (irq_handlers[registers.int_no] != 0) {
        interrupts::IrqHandler handler = irq_handlers[registers.int_no];
        handler(registers);
    }
}

extern "C" void isrHandler(Registers registers) {
    text::out << "Received interrupt: " << text::hex << registers.int_no << ": " << exception_messages[registers.int_no] << text::endl;
    if(registers.int_no == 0xE) { // Paging fault
        unsigned int faulty_address;
        asm volatile("mov %%cr2, %0" : "=r"(faulty_address));
        text::out << "Faulty address: " << text::hex << faulty_address << text::endl;
    }
    while(true)
        asm volatile("hlt");
}

extern "C" int __attribute__((optimize("O0"))) systemCallHandler(Registers registers) {
    unsigned int func = registers.eax;
    unsigned int arg1 = registers.ebx;
    unsigned int arg2 = registers.ecx;
    unsigned int arg3 = registers.edx;
    
    //text::out << "Syscall " << func << ", " << arg1 << ", " << arg2 << ", " << arg3 << text::endl;
    
    if(func >= syscall_handlers_size) {
        debug::out << "Ignoring unknown system call with code: " << debug::hex << func << debug::endl;
        return 0;
    }
    
    interrupts::SyscallHandler handler = syscall_handlers[func];
    return handler(arg1, arg2, arg3);
}

int interrupts::registerSyscallHandler(SyscallHandler handler, const char* name) {
    if(syscall_handlers_size == 256)
        asm volatile("int $0x19");
    debug::out << "Mapping syscall '" << name << "' on code " << syscall_handlers_size << debug::endl;
    syscall_handlers[syscall_handlers_size] = handler;
    syscall_handlers_size++;
    return syscall_handlers_size - 1;
}
