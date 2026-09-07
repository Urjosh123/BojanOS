SECTIONS
{
    . = 0x7C00;
    .text : { *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) }
    /DISCARD/ : { *(.note*) *(.comment*) *(.eh_frame*) }
}
