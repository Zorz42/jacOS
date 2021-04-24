C_SOURCES = $(shell find kernel -type f -name '*.cpp')
HEADERS = $(shell find kernel -type f -name '*.h')
# Nice syntax for file extension replacement
OBJ = $(addprefix build/, ${C_SOURCES:.cpp=.o})

# Change this if your cross-compiler is somewhere else
#CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
#GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
#LD = /usr/local/i386elfgcc/bin/i386-elf-ld
CC = i386-elf-gcc
GDB = i386-elf-gdb
LD = i386-elf-ld

# -g: Use debugging symbols in gcc
#CFLAGS = -g
OPTIMISATION = -O0
CFLAGS = -std=gnu++17 -ffreestanding $(OPTIMISATION) -Ikernel/
KERNEL_OFFSET = 0x1000

#KERNEL_SIZE = $(($(stat -f%z build/kernel.bin) / 512 + 1))
#KERNEL_SIZE = $(stat -f%z build/kernel.bin)

# First rule is run by default
os-image.bin: build build/bootsect.bin #build/kernel.bin
	cat build/bootsect.bin build/kernel.bin > os-image.bin

build:
	mkdir build

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
build/kernel.bin: ${OBJ} build/kernel/entry/kernel_entry.o build/kernel/cpu/interrupt.o
	${LD} -o $@ -Ttext $(KERNEL_OFFSET) build/kernel/entry/kernel_entry.o build/kernel/cpu/interrupt.o ${OBJ} --oformat binary

# Used for debugging purposes
#build/kernel.elf: build/kernel_entry.o ${OBJ}
#	${LD} -o $@ -Ttext $(KERNEL_OFFSET) $^

run: os-image.bin test-program/test-program.img
	qemu-system-x86_64 -fda os-image.bin -m 2048 -drive format=raw,file=test-program/test-program.img

# Open the connection to qemu and load our kernel-object file with symbols
# NOT SUPPORTED
#debug: os-image.bin kernel.elf
#	qemu-system-x86_64 -s -m 2048 -fda os-image.bin &
#	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .cpp
build/kernel/%.o: kernel/%.cpp ${HEADERS}
	@mkdir -p $(@D)
	${CC} ${CFLAGS} -c $< -o $@

# rule for kernel entry
build/kernel/entry/kernel_entry.o: kernel/entry/kernel_entry.asm
	nasm $< -f elf -o $@
build/kernel/cpu/interrupt.o: kernel/cpu/interrupt.asm
	nasm $< -f elf -o $@


# rule for bootsector
build/bootsect.bin: boot/bootsect.asm build/kernel.bin
	echo %define KERNEL_SECTORS_SIZE $$(($$(stat -f%z build/kernel.bin) / 512 + 1)) > boot/kernel_size.asm
	nasm boot/bootsect.asm -f bin -o $@

.PHONY: test-program/test-program.img
test-program/test-program.img:
	cd test-program && make test-program.img

clean:
	rm -rf build
	rm -f os-image.bin
