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
#include <library.hpp>

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

void printDirectory(fs::Directory directory, int offset) {
    text::out << text::endl;
    for(int j = 0; j < offset; j++)
        text::out << "    ";
    text::out << directory.getName() << "/" << text::endl;
    for(int i = 0; i < directory.getFileCount(); i++) {
        fs::File file = directory.getFile(i);
        if(!file.isDirectory()) {
            for(int j = 0; j < offset + 1; j++)
                text::out << "    ";
            text::out << file.getSize() << " " << file.getName() << " " << file.getType() << text::endl;
            
            /*char* file_data = new char[file.getSize()];
            
            file.load(file_data);
            
            for(int i = 0; i < file.getSize() && i < 200; i++) {
                text::out << file_data[i];
            }
            
            text::out << text::endl << text::endl;
            
            delete file_data;*/
        } else {
            fs::Directory dir = file;
            printDirectory(dir, offset + 1);
        }
    }
    text::out << text::endl;
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
    
    fs::Directory root_directory = fs::getFileSystem()->getRootDirectory();
    
    text::out << fs::getFileSystem()->getSectorsTaken() << "/" << disks::getDisk(fs::getFileSystem()->getDiskId()).size << text::endl;
    
    fs::deleteFile("file2");
    fs::createFile("completely_new_file", "txt");
    fs::copyFile("file1", "file3");
    
    printDirectory(root_directory, 0);
    
    fs::File program_file = fs::openFile("jshell");
    
    for(int i = 0; i < program_file.getSize() / 0x1000 + 1; i++)
        mem::allocateFrame(mem::getPage(0x100000 + i * 0x1000), false, true);
    
    Array<unsigned char> program_data = program_file.read();
    for(int i = 0; i < program_data.getSize(); i++)
        *(unsigned char*)(0x100000 + i) = program_data[i];
    
    typedef int (*CallModule)(void);
    CallModule program = (CallModule)0x100000;
    
    program();
    
    for(int i = 0; i < program_file.getSize() / 0x1000 + 1; i++)
        mem::freeFrame(mem::getPage(0x100000 + i * 0x1000));
    
    asm volatile("mov $0, %eax");
    asm volatile("int $0x40");
}
