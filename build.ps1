$ErrorActionPreference = 'Stop'
$base = 'C:\Users\Gower\Downloads\BojanOS'
$zig = "$env:TEMP\zig_dir\zig-windows-x86_64-0.13.0\zig.exe"
$nasm = "$env:TEMP\nasm\nasm-2.16.03\nasm.exe"
$out = "$base\BojanOS.bin"

$asmFiles = @('src/boot.asm','src/gdt_asm.asm','src/idt_asm.asm')
$cFiles = @('src/gdt.c','src/idt.c','src/vga.c','src/string.c','src/keyboard.c','src/timer.c','src/shell.c','src/kernel.c')
$objs = @()

foreach ($f in $asmFiles) {
    $src = Join-Path $base $f
    $obj = [System.IO.Path]::ChangeExtension($src, '.o')
    & $nasm -f elf32 $src -o $obj
    $objs += $obj
}

foreach ($f in $cFiles) {
    $src = Join-Path $base $f
    $obj = [System.IO.Path]::ChangeExtension($src, '.o')
    & $zig cc -target x86-freestanding -ffreestanding -nostdlib -O2 -Wall -I "$base\include" -c $src -o $obj
    $objs += $obj
}

& $zig cc -target x86-freestanding -ffreestanding -nostdlib -Wl,-T,"$base\linker.ld" -o $out @objs
Write-Output "built: $out"
