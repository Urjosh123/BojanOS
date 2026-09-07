# BojanOS

A complete 64-bit x86 operating system written entirely from scratch:
its own BIOS/CD bootloader, its own long-mode kernel, and its own shell.
No GRUB, no external libraries, no third-party kernel code.

## What you get

* **Own CD bootloader** (`src/cdboot.S`) — 16-bit El Torito no-emulation
  boot image, e820 memory map, A20 enable, unreal-mode CD reading, jump
  to the 64-bit kernel.
* **Own long-mode bring-up** (`src/boot.S`) — 4-level identity paging,
  GDT, EFER.LME, transition into 64-bit long mode.
* **Own kernel** — GDT, IDT, PIC remapping, PIT timer (IRQ0), PS/2
  keyboard with interrupt-driven scancode buffer (IRQ1), CMOS RTC, VGA
  text console, serial (COM1) output, heap allocator, in-RAM filesystem.
* **Own shell** with real working commands, pipelines and redirection.

## Running it in VirtualBox

1. Start VirtualBox → **New**
2. Name: `BojanOS`, Type: **Other**, Version: **Other/Unknown (64-bit)**
3. Memory: 64 MB or more is fine
4. Hard disk: you do **not** need one — skip creating a virtual hard disk
5. Open the VM's **Settings → Storage**
6. Click the **Empty** optical drive under the IDE controller, click the
   disc icon → **Choose a disk file…** and select **`BojanOS.iso`**
7. **Start** the VM

That's it — the OS boots straight from the ISO into the shell.

You can also run it in QEMU:

```
qemu-system-x86_64 -cdrom BojanOS.iso
```

## Shell features

Commands (all genuinely implemented, no fake output):

```
help about ls ll dir cd pwd mkdir rmdir rm touch cp mv
cat head tail wc sort uniq diff grep find tree stat
echo printf rev nl tee basename dirname df du free
date cal uptime sleep seq factor history
uname hostname whoami id lscpu clear
true false test [ exit reboot poweroff
```

Operators:

```
> out.txt      redirect output to a file
>> out.txt     append output
< in.txt       feed a file as input
cmd1 | cmd2    pipe (works for whole pipelines, e.g. cat f | sort | uniq | wc -l)
;              command separator
&&  ||         conditional execution
$?             last exit status
~              /root
```

Examples:

```
echo hello world
seq 1 100 | sort -r | head -5
cat readme.txt | grep kernel | wc -l
mkdir /tmp/work && cd /tmp/work && touch notes.txt && ls -l
[ -f /etc/hostname ] && echo present
factor 123456
diff /etc/hostname /etc/motd
```

## Building

Requires `gcc`, `ld`, `objcopy` (binutils) and Python 3.

```
make
```

produces `BojanOS.iso`.

The ISO is built by `mkiso.py` (a small ISO9660 + El Torito image writer
also written from scratch) so no `xorriso`/`grub-mkrescue` is needed.

Host-side logic tests run without an emulator:

```
make test
```

## Source layout

```
src/boot.S        32-bit entry: multiboot headers, paging, long mode
src/cdboot.S      16-bit CD/BIOS bootloader
src/isr.S         interrupt stubs
src/misc.S        lidt/lgdt helpers
src/kernel.c      kmain, bring-up
src/idt.c         IDT setup, interrupt dispatch
src/pic.c         8259 PIC remap
src/timer.c       PIT driver
src/keyboard.c    PS/2 keyboard driver
src/rtc.c         CMOS clock
src/vga.c         text screen
src/serial.c      COM1 output
src/kheap.c       kernel heap
src/vfs.c         in-memory filesystem
src/shell.c       shell, pipes, redirection, history
src/cmds.c        command implementations
src/gdt.c         64-bit GDT
src/string.c      freestanding libc subset
mkiso.py          ISO9660/El Torito image writer
tools/host_test.c host-side test suite
```
