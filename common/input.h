#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

/**
 * Reads input from controller or keyboard, including all the buttons and pad directions
 * and returns immediately. `source` identifies the input source.
 *
 * Output is defined by the bit mask:
 *
 * | Mask                  | Bit |
 * | ---                   | --- |
 * | MINE_INPUT_IGNORED    | 0   |
 * | MINE_INPUT_LEFT       | 1   |
 * | MINE_INPUT_RIGHT      | 2   |
 * | MINE_INPUT_UP         | 3   |
 * | MINE_INPUT_DOWN       | 4   |
 * | MINE_INPUT_OPEN       | 5   |
 * | MINE_INPUT_OPEN_BLOCK | 6   |
 *
 * See [input_select](#input_select) for possible `source` values.
 *
 * Implementation details
 * ----------------------
 *
 * **Fire1** and **fire2** buttons on the keyboard should be mapped to whatever the
 * platform defines as the default mapping for games that use the keyboard.
 *
 * Function should not be buffered, so combinations of keys are not processed and
 * keys and buttons can be shadowed by other buttons with higher priority.
 *
 * Function is **non-blocking**.
 */
void input_read(uint8_t source);

#endif /* INPUT_H */
