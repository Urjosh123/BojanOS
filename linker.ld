ENTRY(_start)
SECTIONS
{
    . = 1M;
    .text ALIGN(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }
    .rodata ALIGN(4K) : ALIGN(4K)
    {
        *(.rodata)
    }
    .data ALIGN(4K) : ALIGN(4K)
    {
        *(.data)
    }
    .bss ALIGN(4K) : ALIGN(4K)
    {
        bss_start = .;
        *(COMMON)
        *(.bss)
        bss_end = .;
    }
}
