#ifndef DEBUG_H
#define DEBUG_H

#ifdef USE_DEBUG_MODE

#include "msx2.h"
#include "common.h"

/* debugdevice modes */
#define DEBUGDEVICE_ASCII   0x63 // or 0x23 for line feed mode version
#define DEBUGDEVICE_INT     0x62
#define DEBUGDEVICE_BIN     0x61
#define DEBUGDEVICE_HEX     0x60

uint8_t _debug_mappings[] = { DEBUGDEVICE_HEX, DEBUGDEVICE_INT, DEBUGDEVICE_BIN };

uint8_t _stored_debug_mode = DEBUGDEVICE_INT; /* default */


/* configure debugdevice */
void _set_debugdevice_mode(uint8_t mode) __z88dk_fastcall
{
    mode;
    __asm
        ld a, l
        out (#0x2e), a
    __endasm;
}


/* send value to debug device */
void _out2f(int8_t value) __z88dk_fastcall
{
    value;
    __asm
        ld a, l
        out (#0x2f), a
    __endasm;
}


void debug_mode(uint8_t mode)
{
    if (mode < DEBUG_INVALID) {
        _stored_debug_mode = _debug_mappings[mode];
    }
}


void debug_msg(char* msg)
{
    _set_debugdevice_mode(DEBUGDEVICE_ASCII);

    for (int i = 0; msg[i] != 0; ++i) {
        _out2f(msg[i]);
    }
}


void debug(char* msg, uint8_t value)
{
    debug_msg(msg);
    _set_debugdevice_mode(_stored_debug_mode);
    _out2f(value);
    debug_msg("\n");
}


/* pause debug device (by tcl script) */
void debug_break()
{
    __asm
        ld a, #0xff
        out (#0x2e), a
    __endasm;
}

#else

typedef int make_iso_compilers_happy;

#endif // USE_DEBUG_MODE

#endif // DEBUG_H
