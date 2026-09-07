CC = gcc
LD = ld
OBJCOPY = objcopy

CFLAGS = -std=gnu11 -ffreestanding -fno-stack-protector -fno-pic \
         -fno-pie -mgeneral-regs-only -mno-red-zone -mcmodel=kernel \
         -Wall -Wextra -Wno-unused-parameter -O2 -Iinclude
ASFLAGS =
LDFLAGS = -nostdlib -static -T linker.ld -z max-page-size=0x1000 \
          --no-dynamic-linker -z noexecstack

COBJS = build/kernel.o build/string.o build/vga.o build/serial.o \
        build/kheap.o build/vfs.o build/pic.o build/timer.o build/rtc.o \
        build/keyboard.o build/idt.o build/multiboot.o build/shell.o \
        build/cmds.o build/gdt.o
AOBJS = build/boot.o build/isr.o build/misc.o

all: BojanOS.iso

build:
	mkdir -p build

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build/boot.o: src/boot.S | build
	$(CC) -ffreestanding -c $< -o $@

build/isr.o: src/isr.S | build
	$(CC) -ffreestanding -c $< -o $@

build/misc.o: src/misc.S | build
	$(CC) -ffreestanding -c $< -o $@

build/cdboot.o: src/cdboot.S | build
	$(CC) -ffreestanding -m32 -c $< -o $@

build/cdboot.elf: build/cdboot.o cdboot.ld
	$(LD) -m elf_i386 -T cdboot.ld -o $@ $<

build/cdboot.bin: build/cdboot.elf
	$(OBJCOPY) -O binary $< $@

BojanOS.elf: $(COBJS) $(AOBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(AOBJS) $(COBJS)

BojanOS.bin: BojanOS.elf
	$(OBJCOPY) -O binary $< $@

BojanOS.iso: BojanOS.bin build/cdboot.bin
	python3 mkiso.py BojanOS.bin build/cdboot.bin BojanOS.iso

run: BojanOS.elf
	qemu-system-x86_64 -kernel BojanOS.elf -serial stdio -append selftest -display none || true

qemu: BojanOS.iso
	qemu-system-x86_64 -cdrom BojanOS.iso -serial stdio -display none || true

clean:
	rm -rf build hbuild BojanOS.elf BojanOS.bin BojanOS.iso

test:
	mkdir -p hbuild
	$(CC) -std=gnu11 -DUNIT_TEST -O0 -g -Iinclude -Wno-unused -c src/string.c -o hbuild/string.o
	$(CC) -std=gnu11 -DUNIT_TEST -O0 -g -Iinclude -Wno-unused -c src/kheap.c -o hbuild/kheap.o
	$(CC) -std=gnu11 -DUNIT_TEST -O0 -g -Iinclude -Wno-unused -c src/vfs.c -o hbuild/vfs.o
	$(CC) -std=gnu11 -DUNIT_TEST -O0 -g -Iinclude -Wno-unused -c src/shell.c -o hbuild/shell.o
	$(CC) -std=gnu11 -DUNIT_TEST -O0 -g -Iinclude -Wno-unused -c src/cmds.c -o hbuild/cmds.o
	$(CC) -std=gnu11 -DUNIT_TEST -O0 -g -Iinclude -Wno-unused tools/host_test.c \
		hbuild/string.o hbuild/kheap.o hbuild/vfs.o hbuild/shell.o hbuild/cmds.o -o hbuild/host_test
	./hbuild/host_test

.PHONY: all clean run qemu test
