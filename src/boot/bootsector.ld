SECTIONS
{
    . = 0x7C00;
    .text : { *(.text) }
    .data : { *(.data) }
}