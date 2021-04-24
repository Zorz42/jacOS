C_SOURCES = $(shell find kernel -type f -name '*.cpp')
HEADERS = $(shell find kernel -type f -name '*.h')
# entry must be first
OBJ = build/kernel/entry/entry.o $(filter-out build/kernel/entry/entry.o, $(addprefix build/, ${C_SOURCES:.cpp=.o}))

# Change this if your cross-compiler is somewhere else
CC = i386-elf-gcc
LD = i386-elf-ld

# default flags
OPTIMISATION = -O0
CFLAGS = -std=gnu++17 -ffreestanding $(OPTIMISATION) -Ikernel/
KERNEL_OFFSET = 0x1000
 
.PHONY: run clean test-program/test-program.img

run: os-image.bin test-program/test-program.img
	qemu-system-x86_64 -fda os-image.bin -drive format=raw,file=test-program/test-program.img -m 2048

os-image.bin: build/bootsect.bin build/kernel.bin
	cat build/bootsect.bin build/kernel.bin > os-image.bin

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
build/kernel.bin: ${OBJ} build/kernel/cpu/interrupt.o
	${LD} -o $@ -Ttext $(KERNEL_OFFSET) ${OBJ} build/kernel/cpu/interrupt.o --oformat binary

# Generic rules for wildcards
# To make an object, always compile from its .cpp
build/kernel/%.o: kernel/%.cpp ${HEADERS}
	@mkdir -p $(@D)
	${CC} ${CFLAGS} -c $< -o $@

# rule for kernel entry
build/kernel/cpu/interrupt.o: kernel/cpu/interrupt.asm
	nasm $< -f elf -o $@


# rule for bootsector
build/bootsect.bin: boot/bootsect.asm build/kernel.bin
	echo %define KERNEL_SECTORS_SIZE $$(($$(stat -f%z build/kernel.bin) / 512 + 1)) > boot/kernel_size.asm
	nasm boot/bootsect.asm -f bin -o $@


test-program/test-program.img:
	@cd test-program && make test-program.img

clean:
	rm -rf build
	rm -f os-image.bin
