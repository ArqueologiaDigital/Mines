/* disk.ld - Linker script for Mines KN5000 (Disk App)
 *
 * Loaded by the App Loader into RAM at 0x208000.
 * The entire binary runs from RAM — no ROM/RAM split.
 *
 * Memory map:
 *   0x208000-0x277FFF: App binary (code + rodata + data) ~448KB max
 *   0x27F000:          Stack top (grows down)
 *   0x207FF0:          CALLER_SAVED_SP (App Loader's SP)
 */

MEMORY {
  APP (rwx) : ORIGIN = 0x208000, LENGTH = 448K
}

ENTRY(_start)

SECTIONS {
  /* Assembly startup + entry point */
  .startup 0x208000 : {
    *(.startup)
  } > APP

  /* Compiled C code */
  .text : {
    *(.text*)
  } > APP

  /* Read-only data (tiles, palette, C constants) */
  .rodata : {
    *(.rodata*)
  } > APP

  /* Initialized C globals (already in place when loaded from disk) */
  .data : {
    __data_start = .;
    *(.data*)
    . = ALIGN(4);
    __data_end = .;
  } > APP

  /* No data copy needed — binary is loaded directly to runtime address */
  __data_load = __data_start;
  __data_size = 0;

  /* Uninitialized C globals (zeroed by startup code) */
  .bss (NOLOAD) : {
    __bss_start = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end = .;
  } > APP

  __bss_size = __bss_end - __bss_start;

  /* Discard unnecessary sections */
  /DISCARD/ : {
    *(.comment)
    *(.note*)
    *(.eh_frame*)
  }
}
