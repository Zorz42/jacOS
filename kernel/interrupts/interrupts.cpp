#include "interrupts.hpp"
#include "ports/ports.hpp"
#include "text/text.hpp"

static interrupts::Handler interrupt_handlers[256];

struct IdtGate {
    u16 low_offset;
    u16 sel;
    u8 always0;
    u8 flags;
    u16 high_offset;
} __attribute__((packed));

struct IdtRegister {
    u16 limit;
    u32 base;
} __attribute__((packed));

#define IDT_ENTRIES 256
static IdtGate idt[IDT_ENTRIES];
static IdtRegister idt_reg;

static void setIdtGate(int n, u32 handler) {
    idt[n].low_offset = low_16(handler);
    idt[n].sel = 0x08;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E;
    idt[n].high_offset = high_16(handler);
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

void interrupts::init() {
    const u32 isr_arr[32] = {(u32)isr0, (u32)isr1, (u32)isr2, (u32)isr3, (u32)isr4, (u32)isr5, (u32)isr6, (u32)isr7, (u32)isr8, (u32)isr9, (u32)isr10, (u32)isr11, (u32)isr12, (u32)isr13, (u32)isr14, (u32)isr15, (u32)isr16, (u32)isr17, (u32)isr18, (u32)isr19, (u32)isr20, (u32)isr21, (u32)isr22, (u32)isr23, (u32)isr24, (u32)isr25, (u32)isr26, (u32)isr27, (u32)isr28, (u32)isr29, (u32)isr30, (u32)isr31};
    
    for(int i = 0; i < 32; i++)
        setIdtGate(i, isr_arr[i]);

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
    
    const u32 irq_arr[16] = {(u32)irq0, (u32)irq1, (u32)irq2, (u32)irq3, (u32)irq4, (u32)irq5, (u32)irq6, (u32)irq7, (u32)irq8, (u32)irq9, (u32)irq10, (u32)irq11, (u32)irq12, (u32)irq13, (u32)irq14, (u32)irq15};
    for(int i = 0; i < 16; i++)
        setIdtGate(i + 32, irq_arr[i]);
    
    idt_reg.base = (u32) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(IdtGate) - 1;
    asm volatile("lidtl (%0)" : : "r" (&idt_reg));
    
    // enable interruptions
    asm volatile("sti");
}

void interrupts::registerHandler(u8 n, Handler handler) {
    interrupt_handlers[n] = handler;
}

extern "C" void irqHandler(Registers registers) {
    if(registers.int_no >= 40)
        ports::byteOut(0xA0, 0x20);
    ports::byteOut(0x20, 0x20);

    if (interrupt_handlers[registers.int_no] != 0) {
        interrupts::Handler handler = interrupt_handlers[registers.int_no];
        handler(registers);
    }
}

static const char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Heap Corruption",
    "Out of Heap Memory",
    "Freeing Invalid Memory",
    "Freeing Freed Memory",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

extern "C" void isrHandler(Registers r) {
    text::cout << "received interrupt: " << r.int_no << text::endl << exception_messages[r.int_no] << text::endl;
    while(true)
        asm volatile("hlt");
}
