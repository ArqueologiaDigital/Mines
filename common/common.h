/* common.h: generic game functions (separated for easier reuse) */
#ifndef COMMON_H
#define COMMON_H


#include <stdint.h>


#define MINE_INPUT_IGNORED 0
#define MINE_INPUT_LEFT       (1<<0)
#define MINE_INPUT_RIGHT      (1<<1)
#define MINE_INPUT_UP         (1<<2)
#define MINE_INPUT_DOWN       (1<<3)
#define MINE_INPUT_OPEN       (1<<4)
#define MINE_INPUT_FLAG       (1<<5)
#define MINE_INPUT_OPEN_BLOCK (1<<6)
#define MINE_INPUT_QUIT       (1<<7)

#define KEYBOARD           0
#define JOY_1              1
#define JOY_2              2
#define JOY_3              3
#define JOY_4              4

/**
 * Compatibility macro for unused function parameter warnings or errors.
 * If you need it, just put -DUNUSED_MACRO on CFLAGS.
 */
#ifdef UNUSED_MACRO
#define UNUSED(x) x;
#else
#define UNUSED(x)
#endif /* UNUSED_MACRO */

/**
 * [Required] Platform-specific initialisation code
 *
 * Implementation details
 * ----------------------
 *
 * A place to properly initialise graphics mode and sound settings, move tiles
 * and sprites from ROM to RAM etc.
 */
void platform_init();

/**
 * [Required] Platform-specific shutdown code
 *
 * Implementation details
 * ----------------------
 *
 * A place to properly change back to text mode, free previous memory
 * allocations etc.
 */
void platform_shutdown();

/**
 * [Required] Reads input from controller or keyboard, including all the buttons
 * and pad directions and returns immediately. `source` identifies the input
 * source.
 *
 * Output is defined by the bit mask:
 *
 * | Name                  | Bit |
 * | ---                   | --- |
 * | MINE_INPUT_IGNORED    | -   |
 * | MINE_INPUT_LEFT       | 0   |
 * | MINE_INPUT_RIGHT      | 1   |
 * | MINE_INPUT_UP         | 2   |
 * | MINE_INPUT_DOWN       | 3   |
 * | MINE_INPUT_OPEN       | 4   |
 * | MINE_INPUT_OPEN_BLOCK | 5   |
 *
 * See [input_select](#input_select) for possible `source` values.
 *
 * Implementation details
 * ----------------------
 *
 * **Fire1** and **fire2** buttons on the keyboard should be mapped to whatever
 * the platform defines as the default mapping for games that use the keyboard.
 *
 * Function should not be buffered, so combinations of keys are not processed
 * and keys and buttons can be shadowed by other buttons with higher priority.
 *
 * Function is **non-blocking**.
 */
uint8_t input_read(uint8_t source);

/**
 * [Required] Returns a pseudo-random integer in the mathematical range
 * [`min_num`, `max_num`]. If `min_num` is greater than `max_num`, they will be
 * swapped automatically.
 *
 * Implementation details
 * ----------------------
 *
 * No assumption about seed or quality of random numbers is considered at this
 * point. All platforms are free to pursue their own implementations as they see
 * fit.
 *
 * There should be no assumption whether function is reentrant or not since
 * reproducible behavior is not a requirement.
 */
int random_number(int min, int max);

#ifdef USE_DEBUG_MODE

#define DEBUG_HEX          0
#define DEBUG_INT          1
#define DEBUG_BIN          2
#define DEBUG_INVALID      3

/**
 * [Optional] Change emulator's debug mode. This allows programs to output text
 * and values to the emulator for debugging purposes. The format of the output
 * value is defined below.
 *
 * | Name                  | Value |
 * | ---                   | ----- |
 * | DEBUG_HEX             |   0   |
 * | DEBUG_INT             |   1   |
 * | DEBUG_BIN             |   2   |
 *
 * See [debug](#debug) for more details about the output.
 *
 * Implementation details
 * ----------------------
 *
 * You can undefine `USE_DEBUG_MODE` macro to transform all the debug macros
 * into empty macros, so no additional code is generated on the release version.
 */
void debug_mode(uint8_t mode);

/**
 * [Optional] Send text message to emulator for debugging (printing).
 *
 * Implementation details
 * ----------------------
 *
 * You can undefine `USE_DEBUG_MODE` macro to transform all the debug macros
 * into empty macros, so no additional code is generated on the release version.
 */
void debug_msg(char* msg);

/**
 * [Optional] Send message to emulator for debugging (printing) along with a
 * numeric value. See [debug_mode](#debug_mode) for output options.
 *
 * Implementation details
 * ----------------------
 *
 * You can undefine `USE_DEBUG_MODE` macro to transform all the debug macros
 * into empty macros, so no additional code is generated on the release version.
 */
void debug(char* msg, uint8_t value);

/**
 * [Optional] Pause emulator at debug_break function.
 *
 * Implementation details
 * ----------------------
 *
 * You can undefine `USE_DEBUG_MODE` macro to transform all the debug macros
 * into empty macros, so no additional code is generated on the release version.
 */
void debug_break();

/**
 * Breaks execution if not `ok` and displays optional message after the `ok` parameter.
 *
 * Implementation details
 * ----------------------
 *
 * You can undefine `USE_DEBUG_MODE` macro to transform all the debug macros
 * into empty macros, so no additional code is generated on the release version.
 *
 * Regular C `assert`-like macros are not appropriate for graphics mode, so
 * Mines leverages some debug functions to implements its own.
 *
 * This macro requires that the platform's C compiler supports stringification
 * of arguments in macros. Otherwise, the `NO_STRINGIFICATION` macro should be
 * defined first.
 *
 * This macro can use variable argument support (variadic macros) to include a
 * message in case of assertion failure, but you can set NO_VARIADIC first to
 * disable this feature.
 */
#ifndef NO_STRINGIFICATION
# ifndef NO_VARIADIC
#  define assert(ok, ...) \
	do { if (!(ok)) { debug_msg("Assertion `" #ok "' failed. " __VA_ARGS__ "\nPaused\n"); debug_break(); } } while(0)
# else /* NO_VARIADIC */
#  define assert(ok) \
	do { if (!(ok)) { debug_msg("Assertion `" #ok "' failed.\nPaused\n"); debug_break(); } } while(0)
# endif /* NO_VARIADIC */
#else /* NO_STRINGIFICATION */
# ifndef NO_VARIADIC
#  define assert(ok, ...) \
	do { if (!(ok)) { debug_msg("Assertion failed. " __VA_ARGS__ "\nPaused\n"); debug_break(); } } while(0)
# else /* NO_VARIADIC */
#  define assert(ok) \
	do { if (!(ok)) { debug_msg("Assertion failed.\nPaused\n"); debug_break(); } } while(0)
# endif /* NO_VARIADIC */
#endif /* NO_STRINGIFICATION */

#else

/* empty macros */
#define debug_mode(x)
#define debug(x, y)
#define debug_msg(x)
#define debug_break()

# ifndef NO_VARIADIC
#  define assert(x, ...)
# else
#  define assert(x)
# endif /* NO_VARIADIC */

#endif /* USE_DEBUG_MODE */

#endif /* COMMON_H */
