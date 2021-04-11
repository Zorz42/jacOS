C_SOURCES = $(wildcard kernel/*.c)
HEADERS = $(wildcard kernel/*.h)
# Nice syntax for file extension replacement
OBJ_FILES = ${C_SOURCES:.c=.o}
OBJ = $(addprefix build/, ${OBJ_FILES})

# Change this if your cross-compiler is somewhere else
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
# -g: Use debugging symbols in gcc
#CFLAGS = -g
CFLAGS = -std=c11 -ffreestanding -O0 # -O1 also works

# First rule is run by default
os-image.bin: build build/kernel build/bootsect.bin build/kernel.bin
	cat build/bootsect.bin build/kernel.bin > os-image.bin

build:
	mkdir build

build/kernel:
	mkdir build/kernel

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
build/kernel.bin: build/kernel_entry.o ${OBJ}
	/usr/local/i386elfgcc/bin/i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

# Used for debugging purposes
build/kernel.elf: build/kernel_entry.o ${OBJ}
	/usr/local/i386elfgcc/bin/i386-elf-ld -o $@ -Ttext 0x1000 $^ 

run: os-image.bin
	qemu-system-i386 -fda os-image.bin

# Open the connection to qemu and load our kernel-object file with symbols
debug: os-image.bin kernel.elf
	qemu-system-i386 -s -fda os-image.bin &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .c
build/kernel/%.o: kernel/%.c ${HEADERS}
	${CC} ${CFLAGS} -c $< -o $@

# rule for kernel entry
build/kernel_entry.o: boot/kernel_entry.asm
	nasm $< -f elf -o $@

# rule for bootsector
build/bootsect.bin: boot/bootsect.asm
	nasm $< -f bin -o $@

clean:
	rm -rf build