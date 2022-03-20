#include "graphics/gfx.hpp"
#include "memory/memory.hpp"
#include "vesa/vesa.hpp"
#include "qemuDebug/debug.hpp"
#include "interrupts/interrupts.hpp"
#include "text/text.hpp"
#include "timer/timer.hpp"
#include "keyboard/keyboard.hpp"
#include "disk/disk.hpp"
#include "gdt/gdt.hpp"
#include "filesystem/filesystem.hpp"

static void switchToUserMode() {
    asm volatile("cli");
    asm volatile("mov $0x23, %ax");
    asm volatile("mov %ax, %ds");
    asm volatile("mov %ax, %es");
    asm volatile("mov %ax, %fs");
    asm volatile("mov %ax, %gs");
    
    asm volatile("mov %esp, %eax");
    asm volatile("pushl $0x23");
    asm volatile("pushl %eax");
    asm volatile("pushf");
    
    asm volatile("pop %eax");
    asm volatile("or $0x200, %eax");
    asm volatile("push %eax");
    
    asm volatile("pushl $0x1B");
    asm volatile("push $1f");
    asm volatile("iret");
    asm volatile("1:");
}

void kernelMain() {
    gdt::init();
    interrupts::init();
    
    mem::init();
    vesa::init();
    text::init();
    
    timer::init(1000);
    keyboard::init();
    
    disks::init();
    fs::init();
    
    switchToUserMode();
    
    debug::out << "Kernel initialized" << debug::endl;
    
    
    for(int i = 0; i < fs::getFileSystem()->getFileCount(); i++) {
        fs::File file = fs::getFileSystem()->getFile(i);
        text::out << "File name: " << file.getName() << ", size: " << file.getSize() << ", type: " << file.getType() << text::endl;
        char* file_data = (char*)mem::alloc(file.getSize());
        
        file.load(file_data);
        
        for(int i = 0; i < file.getSize() && i < 200; i++) {
            text::out << file_data[i];
            text::flush();
        }
        
        text::out << text::endl;
        
        mem::free(file_data);
    }
    
    
    disks::Disk program_disk = disks::getDisk(0);
    
    for(int i = 0; i < program_disk.size / 8 + 1; i++)
        mem::allocateFrame(mem::getPage(0x100000 + i * 0x1000), false, true);
    
    program_disk.read(0, program_disk.size, (void*)0x100000);
    
    typedef int (*call_module_t)(void);
    call_module_t program = (call_module_t)0x100000;
    
    int exit_code = program();
    
    for(int i = 0; i < program_disk.size / 8 + 1; i++)
        mem::freeFrame(mem::getPage(0x100000 + i * 0x1000));
    
    asm volatile("mov $0, %eax");
    asm volatile("int $0x40");
}
