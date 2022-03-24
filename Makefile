# Change this if your cross-compiler is somewhere else
CC = i386-elf-gcc
LD = i386-elf-ld

PROGRAM_NAME = test-program

KERNEL_SOURCES = $(shell find kernel -type f -name '*.cpp') $(shell find library -type f -name '*.cpp') $(shell find kernel -type f -name '*.asm')
KERNEL_HEADERS = $(shell find kernel -type f -name '*.hpp') $(shell find library -type f -name '*.hpp')
_KERNEL_OBJ = ${KERNEL_SOURCES:.cpp=.o}
KERNEL_OBJ = build/kernel/entry/entry.o $(filter-out build/kernel/entry/entry.o, $(addprefix build/, ${_KERNEL_OBJ:.asm=.o}))

BOOT_SOURCES = $(shell find boot -type f -name '*.asm')

PROGRAM_SOURCES = $(shell find ${PROGRAM_NAME} -type f -name '*.cpp') $(shell find program -type f -name '*.cpp')
PROGRAM_HEADERS = $(shell find ${PROGRAM_NAME} -type f -name '*.hpp') $(shell find program -type f -name '*.hpp')
PROGRAM_OBJ = build/program/entry.o $(filter-out build/program/entry.o, $(addprefix build/, ${PROGRAM_SOURCES:.cpp=.o}))

# default flags
CFLAGS = -std=gnu++17 -fno-exceptions -ffreestanding -O2 -Ikernel/ -isystem program/lib/ -isystem library

.PHONY: run clean

run: os-image.bin ${PROGRAM_NAME}.img filesystem.img Makefile
	qemu-system-x86_64 -fda os-image.bin -drive file=filesystem.img,format=raw -m 2G -serial file:debug.log # for now it doesnt work without floppy boot drive, since it assumes floppy disk geometry
	
	
os-image.bin: build/kernel.bin build/bootsect.bin Makefile
	cat build/bootsect.bin build/kernel.bin > os-image.bin

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
build/kernel.bin: ${KERNEL_OBJ} kernel/link.ld ${KERNEL_HEADERS}
	${LD} -o $@ -Tkernel/link.ld ${KERNEL_OBJ} --oformat binary

# Generic rules for wildcards
# To make an object, always compile from its .cpp
build/%.o: %.cpp ${KERNEL_HEADERS}
	@mkdir -p $(@D)
	${CC} ${CFLAGS} -c $< -o $@

build/%.o: %.asm
	@mkdir -p $(@D)
	nasm $< -f elf -o $@

# rule for bootsector
build/bootsect.bin: $(BOOT_SOURCES)
	echo %define KERNEL_SECTORS_SIZE $$(($$(stat -f%z build/kernel.bin) / 512 + 1)) > boot/kernel_size.asm
	nasm boot/bootsect.asm -f bin -o $@
	
filesystem.img:
	python3 filesystem-builder.py

# rule for custom program
${PROGRAM_NAME}.img: ${PROGRAM_OBJ} program/link.ld
	${LD} -Tprogram/link.ld -o $@ ${PROGRAM_OBJ} --oformat binary

clean:
	rm -rf build
	rm -f os-image.bin
