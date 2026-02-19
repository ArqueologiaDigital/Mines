/* kn5000.ld - Linker script for Mines KN5000 (HDAE5000 Extension ROM)
 *
 * Memory map:
 *   ROM: 0x280000-0x2FFFFF (512KB extension ROM)
 *   RAM: 0x200000-0x202FFF (12KB extension DRAM)
 *
 * Layout:
 *   .startup at 0x280000: XAPR header + assembly glue (startup.o .text)
 *   .text    at 0x281000: Compiled C code (all_c.o .text)
 *   .rodata  follows:     Tile data, palette, icon, C const data
 *   .data    at 0x200310: Initialized C globals (loaded from ROM at startup)
 *   .bss     at 0x201000: Uninitialized C globals (zeroed at startup)
 */

MEMORY {
  ROM (rx)  : ORIGIN = 0x280000, LENGTH = 512K
  RAM (rwx) : ORIGIN = 0x200000, LENGTH = 12K
}

SECTIONS {
  /* Assembly startup code: XAPR header, handlers, palette routines, etc. */
  .startup 0x280000 : {
    startup.o(.text)
  } > ROM

  /* Compiled C code */
  .text 0x281000 : {
    all_c.o(.text)
  } > ROM

  /* Read-only data: tiles, palette, icon from startup.o + C const data */
  .rodata : {
    *(.rodata*)
  } > ROM

  /* Initialized C data: stored in ROM, copied to RAM at startup.
   * ALIGN(4) ensures Copy_C_Data can use 4-byte copies. */
  .data 0x200310 : {
    __data_start = .;
    all_c.o(.data)
    . = ALIGN(4);
    __data_end = .;
  } > RAM AT > ROM

  __data_load = LOADADDR(.data);
  __data_size = __data_end - __data_start;

  /* Uninitialized C data: zeroed at startup.
   * ALIGN(4) ensures Clear_C_BSS can use 4-byte stores. */
  .bss 0x201000 : {
    __bss_start = .;
    all_c.o(.bss)
    . = ALIGN(4);
    __bss_end = .;
  } > RAM

  __bss_size = __bss_end - __bss_start;

  /* Discard debug and other unnecessary sections */
  /DISCARD/ : {
    *(.comment)
    *(.note*)
    *(.eh_frame*)
  }
}
