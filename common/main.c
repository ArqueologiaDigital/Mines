#include "common.h"
#include "game.h"
#include "main.h"
#include "minefield.h"


void set_minefield_cell(minefield* mf, uint8_t x, uint8_t y, uint8_t input)
{
    uint8_t cell_index = CELL_INDEX(mf, x, y);
    uint8_t current_cell = mf->cells[cell_index];
    uint8_t has_bomb = current_cell & HASBOMB;
    uint8_t num_bombs_around = current_cell & 0x0F;

    switch (input) {
        case MINE_INPUT_OPEN:
            open_cell(mf, x, y);
            if (mf->state == PLAYING_GAME) {
                maybe_game_won(mf);
            }
            break;

        case MINE_INPUT_OPEN_BLOCK:
            if (!has_bomb && (count_surrounding_flags(mf, x, y) == num_bombs_around)) {
                open_block(mf, x, y);
                if (mf->state == PLAYING_GAME) {
                    maybe_game_won(mf);
                }
            }
            break;

        case MINE_INPUT_FLAG:
            if (!(current_cell & ISOPEN)) {
                if (current_cell & HASFLAG) {
                    mf->mines++;
                    mf->cells[cell_index] &= ~HASFLAG;
                    mf->cells[cell_index] |= HASQUESTIONMARK;
                } else if (current_cell & HASQUESTIONMARK) {
                    mf->cells[cell_index] &= ~HASQUESTIONMARK;
                } else {
                    mf->mines--;
                    mf->cells[cell_index] |= HASFLAG;
                    maybe_game_won(mf);
                }
            }
            mf->changed = true;
            break;

        default:
            debug("unknown command in set_minefield_cell: ", input);
            break;
    }
}


void update_gameplay_input(minefield* mf, uint8_t input)
{
    uint8_t num_cells = mf->width * mf->height;

    switch (input) {
        case MINE_INPUT_LEFT:
            if (mf->current_cell % mf->width > 0) {
                mf->current_cell--;
            }
            break;

        case MINE_INPUT_RIGHT:
            if (mf->current_cell % mf->width < mf->width - 1) {
                mf->current_cell++;
            }
            break;

        case MINE_INPUT_UP:
            if (mf->current_cell > mf->width - 1) {
                mf->current_cell -= mf->width;
            }
            break;

        case MINE_INPUT_DOWN:
            if (mf->current_cell < num_cells - mf->width) {
                mf->current_cell += mf->width;
            }
            break;

        case MINE_INPUT_QUIT:
            mf->state = QUIT;
            break;

        default: {
            uint8_t x = mf->current_cell % mf->width;
            uint8_t y = mf->current_cell / mf->width;
            set_minefield_cell(mf, x, y, input);
        }
    }
}


inline void title_screen_update(minefield* mf)
{
#ifdef DRAW_TITLE_SCREEN
    // TODO: platform dependent title screen, animations, music etc.
    draw_title_screen(mf);

    uint8_t input = input_read(KEYBOARD);
    if (input == MINE_INPUT_IGNORED) {
        return;
    }
    debug("\nkey code = ", input);

    switch (input) {
        case MINE_INPUT_OPEN:
        case MINE_INPUT_OPEN_BLOCK:
        case MINE_INPUT_FLAG:
            mf->state = PLAYING_GAME;
            reset_minefield(mf);
            draw_minefield(mf);
#ifdef ENABLE_TIMER
            reset_timer();
#endif /* ENABLE_TIMER */
            break;
        case MINE_INPUT_QUIT:
            mf->state = QUIT;
            break;
        default:
            /* Ignore cursor moves when game over */
            break;
    }
#else
    mf->state = PLAYING_GAME;
    reset_minefield(mf);
    draw_minefield(mf);
#ifdef ENABLE_TIMER
    reset_timer();
#endif /* ENABLE_TIMER */
#endif
}


inline void game_over_update(minefield* mf)
{
#ifdef DRAW_GAME_OVER
    draw_game_over(mf);
#else
    draw_minefield_contents(mf);
#endif

    uint8_t input = input_read(KEYBOARD);
    if (input == MINE_INPUT_IGNORED) {
        return;
    }
    debug("\nkey code = ", input);

    switch (input) {
        case MINE_INPUT_OPEN:
        case MINE_INPUT_OPEN_BLOCK:
        case MINE_INPUT_FLAG:
            mf->state = TITLE_SCREEN;
            break;
        case MINE_INPUT_QUIT:
            mf->state = QUIT;
            break;
        default:
            /* Ignore cursor moves when game over */
            break;
    }
}


inline void gameplay_update(minefield* mf)
{
    draw_minefield_contents(mf);

#ifdef ENABLE_TIMER
    update_timer(mf);
#endif /* ENABLE_TIMER */

#ifdef ENABLE_COUNTER
    update_counter(mf);
#endif /* ENABLE_COUNTER */

    uint8_t input = input_read(KEYBOARD);
    if (input == MINE_INPUT_IGNORED) {
        return;
    }
    debug("\nkey code = ", input);

    update_gameplay_input(mf, input);
}
 

#if !defined(FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION)

#ifdef __GNUC__
/* This is required for the MS-DOS port because otherwise
 * the linker won't see the call to main() from our _start,
 * and throw all the code away. */
__attribute__((used))
#endif
/* Temporary debug marker for KN5000 — writes to AudioMix port 0xFE */
/* Debug progress marker: write to both AudioMix port and RAM location.
 * RAM location 0x200050 can be read by Lua test scripts.
 * Uses 32-bit stores to avoid LLVM TLCS-900 bug #8. */
#define DBG(x) do { \
    *(volatile uint32_t *)0x150000 = ((uint32_t)(x) << 16) | 0x00FE; \
    *(volatile uint32_t *)0x200050 = (uint32_t)(x); \
} while(0)

int main() {
    DBG(0xD0);  /* D0 = main entry */
    platform_init();

    DBG(0xD1);  /* D1 = after platform_init */
    minefield* mf = init_minefield();

    DBG(0xD2);  /* D2 = after init_minefield */
    // draw screen elements
    draw_scenario(mf);

    DBG(0xD3);  /* D3 = after draw_scenario */

#ifdef ENABLE_COUNTER
    draw_counter();
#endif // ENABLE_COUNTER

#if ENABLE_TIMER
    draw_timer();
#endif // ENABLE_TIMER

    mf->state = TITLE_SCREEN;

    DBG(0xD4);  /* D4 = entering game loop */

#ifdef MAIN_LOOP_REIMPLEMENTED
    platform_main_loop(mf);
#else
    while (mf->state != QUIT) {
        DBG(0xD5);  /* D5 = game loop iteration */
        switch (mf->state)
        {
            case TITLE_SCREEN:
                DBG(0xD6);  /* D6 = title_screen_update */
                title_screen_update(mf);
                break;
            case PLAYING_GAME:
                DBG(0xD7);  /* D7 = gameplay_update */
                gameplay_update(mf);
                break;
            case GAME_WON:
            case GAME_OVER:
                game_over_update(mf);
                break;
        }

        idle_update(mf); // useful for doing other things
                         // such as running animations
    }
#endif /* MAIN_LOOP_REIMPLEMENTED */

    DBG(0xDF);  /* DF = main exiting (should not happen!) */
    free_minefield(mf);
    platform_shutdown();

    return 0;
}
#endif /* !defined(FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION) */
