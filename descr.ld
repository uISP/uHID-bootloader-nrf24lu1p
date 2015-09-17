SECTIONS
{
  .data 0x0 : { *(.data) }
  .text : { *(.text) }
  .bss : { *(.bss) }
}