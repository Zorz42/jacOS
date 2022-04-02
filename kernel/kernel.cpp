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
#include <library>

static void switchToUserMode(unsigned int jump_to) {
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
    asm volatile("push %0" : : "r"(jump_to));
    asm volatile("iret");
}

void printDirectory(fs::Directory directory, int offset) {
    out << endl;
    for(int j = 0; j < offset; j++)
        out << "    ";
    out << directory.getName() << "/" << endl;
    for(int i = 0; i < directory.getFileCount(); i++) {
        fs::File file = directory.getFile(i);
        if(!file.isDirectory()) {
            for(int j = 0; j < offset + 1; j++)
                out << "    ";
            out << file.getSize() << " " << file.getName() << " " << file.getType() << endl;
        } else {
            fs::Directory dir = file;
            printDirectory(dir, offset + 1);
        }
    }
    out << endl;
}

void kernelMain() {
    gdt::init();
    interrupts::init();
    
    mem::init();
    vesa::init();
    initText();
    
    timer::init(1000);
    keyboard::init();
    
    disks::init();
    fs::init();
    
    debug::out << "Kernel initialized" << debug::endl;
    
    fs::Directory root_directory = fs::getFileSystem()->getRootDirectory();
    
    out << fs::getFileSystem()->getSectorsTaken() << "/" << disks::getDisk(fs::getFileSystem()->getDiskId()).size << endl;
    
    printDirectory(root_directory, 0);
    
    fs::File program_file = fs::openFile("jshell");
    
    for(int i = 0; i < program_file.getSize() / 0x1000 + 1; i++)
        mem::allocateFrame(mem::getPage(0x110000 + i * 0x1000), false, true);
    
    Array<unsigned char> program_data = program_file.read();
    for(int i = 0; i < program_data.getSize(); i++)
        *(unsigned char*)(0x110000 + i) = program_data[i];
    
    switchToUserMode(0x110000);
    
    for(int i = 0; i < program_file.getSize() / 0x1000 + 1; i++)
        mem::freeFrame(mem::getPage(0x110000 + i * 0x1000));
    
    asm volatile("mov $0, %eax");
    asm volatile("int $0x40");
}
