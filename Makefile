CC = i686-elf-gcc
AS = nasm
LD = i686-elf-ld
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Iinclude
LDFLAGS = -nostdlib -T linker.ld
OBJS = src/boot.o src/gdt_asm.o src/idt_asm.o src/gdt.o src/idt.o src/vga.o src/string.o src/keyboard.o src/timer.o src/shell.o src/kernel.o

all: BojanOS.bin

BojanOS.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

iso: BojanOS.bin
	cp BojanOS.bin iso/boot/
	grub-mkrescue -o BojanOS.iso iso

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) -f elf32 $< -o $@

clean:
	rm -f $(OBJS) BojanOS.bin BojanOS.iso

run: BojanOS.bin
	qemu-system-i386 -kernel BojanOS.bin
