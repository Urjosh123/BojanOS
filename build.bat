@echo off
setlocal EnableDelayedExpansion
set BASE=C:\Users\Gower\Downloads\BojanOS
set ZIG=%TEMP%\zig_dir\zig-windows-x86_64-0.13.0\zig.exe
set NASM=%TEMP%\nasm\nasm-2.16.03\nasm.exe
set LD="C:\Program Files\dotnet\packs\Microsoft.Android.Sdk.Windows\35.0.39\tools\binutils\bin\ld.exe"
set OBJCOPY="C:\Program Files\dotnet\packs\Microsoft.Android.Sdk.Windows\35.0.39\tools\binutils\bin\llvm-objcopy.exe"
set OUT=%BASE%\BojanOS.bin

"%NASM%" -f elf64 "%BASE%\src\boot.asm" -o "%BASE%\src\boot.o"

"%ZIG%" cc -target x86_64-freestanding -ffreestanding -nostdlib -O2 -Wall -fno-builtin -I "%BASE%\include" -c "%BASE%\src\gdt.c" -o "%BASE%\src\gdt.o"
"%ZIG%" cc -target x86_64-freestanding -ffreestanding -nostdlib -O2 -Wall -fno-builtin -I "%BASE%\include" -c "%BASE%\src\idt.c" -o "%BASE%\src\idt.o"
"%ZIG%" cc -target x86_64-freestanding -ffreestanding -nostdlib -O2 -Wall -fno-builtin -I "%BASE%\include" -c "%BASE%\src\vga.c" -o "%BASE%\src\vga.o"
"%ZIG%" cc -target x86_64-freestanding -ffreestanding -nostdlib -O2 -Wall -fno-builtin -I "%BASE%\include" -c "%BASE%\src\string.c" -o "%BASE%\src\string.o"
"%ZIG%" cc -target x86_64-freestanding -ffreestanding -nostdlib -O2 -Wall -fno-builtin -I "%BASE%\include" -c "%BASE%\src\keyboard.c" -o "%BASE%\src\keyboard.o"
"%ZIG%" cc -target x86_64-freestanding -ffreestanding -nostdlib -O2 -Wall -fno-builtin -I "%BASE%\include" -c "%BASE%\src\timer.c" -o "%BASE%\src\timer.o"
"%ZIG%" cc -target x86_64-freestanding -ffreestanding -nostdlib -O2 -Wall -fno-builtin -I "%BASE%\include" -c "%BASE%\src\pic.c" -o "%BASE%\src\pic.o"
"%ZIG%" cc -target x86_64-freestanding -ffreestanding -nostdlib -O2 -Wall -fno-builtin -I "%BASE%\include" -c "%BASE%\src\vfs.c" -o "%BASE%\src\vfs.o"
"%ZIG%" cc -target x86_64-freestanding -ffreestanding -nostdlib -O2 -Wall -fno-builtin -I "%BASE%\include" -c "%BASE%\src\shell.c" -o "%BASE%\src\shell.o"
"%ZIG%" cc -target x86_64-freestanding -ffreestanding -nostdlib -O2 -Wall -fno-builtin -I "%BASE%\include" -c "%BASE%\src\kernel.c" -o "%BASE%\src\kernel.o"

%LD% -nostdlib -T "%BASE%\linker.ld" -o "%OUT%" "%BASE%\src\boot.o" "%BASE%\src\gdt.o" "%BASE%\src\idt.o" "%BASE%\src\vga.o" "%BASE%\src\string.o" "%BASE%\src\keyboard.o" "%BASE%\src\timer.o" "%BASE%\src\pic.o" "%BASE%\src\vfs.o" "%BASE%\src\shell.o" "%BASE%\src\kernel.o"

%OBJCOPY% -O binary "%OUT%" "%BASE%\BojanOS.flat"

"%NASM%" -f bin "%BASE%\src\bootloader.asm" -o "%BASE%\bootloader.bin"

copy /b "%BASE%\bootloader.bin" + "%BASE%\BojanOS.flat" "%BASE%\BojanOS.img"

py "%BASE%\makeimg.py"

echo built: %OUT%
echo image: %BASE%\BojanOS.img
endlocal