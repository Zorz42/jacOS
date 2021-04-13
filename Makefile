C_SOURCES = $(shell find kernel -type f -name '*.c')
HEADERS = $(shell find kernel -type f -name '*.h')
# Nice syntax for file extension replacement
OBJ = $(addprefix build/, ${C_SOURCES:.c=.o})

# Change this if your cross-compiler is somewhere else
#CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
#GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
#LD = /usr/local/i386elfgcc/bin/i386-elf-ld
CC = i386-elf-gcc
GDB = i386-elf-gdb
LD = i386-elf-ld

# -g: Use debugging symbols in gcc
#CFLAGS = -g
CFLAGS = -std=c11 -ffreestanding -Ofast

#KERNEL_SIZE = $(($(stat -f%z build/kernel.bin) / 512 + 1))
#KERNEL_SIZE = $(stat -f%z build/kernel.bin)

# First rule is run by default
os-image.bin: build build/bootsect.bin build/kernel.bin
	cat build/bootsect.bin build/kernel.bin > os-image.bin

build:
	mkdir build

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
build/kernel.bin: ${OBJ} build/kernel/entry/kernel_entry.o build/bootsect.bin
	${LD} -o $@ -Ttext 0x1000 build/kernel/entry/kernel_entry.o ${OBJ} --oformat binary

# Used for debugging purposes
build/kernel.elf: build/kernel_entry.o ${OBJ}
	${LD} -o $@ -Ttext 0x1000 $^ 

run: os-image.bin
	qemu-system-x86_64 -fda os-image.bin -m 2048

# Open the connection to qemu and load our kernel-object file with symbols
debug: os-image.bin kernel.elf
	qemu-system-x86_64 -s -m 2048 -fda os-image.bin &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .c
build/kernel/%.o: kernel/%.c ${HEADERS}
	@mkdir -p $(@D)
	${CC} ${CFLAGS} -c $< -o $@

# rule for kernel entry
build/kernel/entry/kernel_entry.o: kernel/entry/kernel_entry.asm
	nasm $< -f elf -o $@

# rule for bootsector
build/bootsect.bin: boot/bootsect.asm build/kernel.bin
	export KERNEL_SIZE=$$(($$(stat -f%z build/kernel.bin) / 512 + 1)) && \
	echo %define KERNEL_SECTORS_SIZE $$KERNEL_SIZE > boot/kernel_size.asm
	nasm boot/bootsect.asm -f bin -o $@

clean:
	rm -rf build
	rm -f os-image.bin
