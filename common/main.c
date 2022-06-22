#include "common.h"
#include "game.h"
#include "main.h"
#include "minefield.h"


void update_gameplay_input(minefield* mf, uint8_t input)
{
	uint8_t x = mf->current_cell % mf->width;
	uint8_t y = mf->current_cell / mf->width;
	uint8_t has_bomb = (mf->cells[mf->current_cell] & HASBOMB);
	uint8_t num_bombs_around = (mf->cells[mf->current_cell] & 0x0F);
	uint8_t num_cells = (mf->width * mf->height);

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

		case MINE_INPUT_OPEN:
			open_cell(mf, x, y);
			break;

		case MINE_INPUT_OPEN_BLOCK:
			if (!has_bomb && (count_surrounding_flags(mf, x, y) == num_bombs_around))
				open_block(mf, x, y);
			break;

		case MINE_INPUT_FLAG:
			if (!(mf->cells[mf->current_cell] & ISOPEN)) {
				if (mf->cells[mf->current_cell] & HASFLAG) {
					mf->cells[mf->current_cell] &= ~HASFLAG;
					mf->cells[mf->current_cell] |= HASQUESTIONMARK;
				} else if (mf->cells[mf->current_cell] & HASQUESTIONMARK) {
					mf->cells[mf->current_cell] &= ~HASQUESTIONMARK;
				} else {
					mf->cells[mf->current_cell] |= HASFLAG;
				}
			}
			mf->changed = true;
			break;

		case MINE_INPUT_QUIT:
			mf->state = QUIT;

		default:
			break;
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
			reset_minefield(mf);
			draw_minefield(mf);
			mf->state = PLAYING_GAME;
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

	uint8_t input = input_read(KEYBOARD);
	if (input == MINE_INPUT_IGNORED) {
		return;
	}
	debug("\nkey code = ", input);

	update_gameplay_input(mf, input);
}
 

#if !defined(FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION)
int main() {
	platform_init();

	minefield* mf = init_minefield();
	mf->state = TITLE_SCREEN;

#ifdef MAIN_LOOP_REIMPLEMENTED
	platform_main_loop(mf);
#else
	while (mf->state != QUIT) {
		switch (mf->state)
		{
			case TITLE_SCREEN:
				title_screen_update(mf);
				break;
			case PLAYING_GAME:
				gameplay_update(mf);
				break;
			case GAME_OVER:
				game_over_update(mf);
				break;
		}

		idle_update(mf); // useful for doing other things
		               // such as running animations
	}
#endif /* MAIN_LOOP_REIMPLEMENTED */

	free_minefield(mf);
	platform_shutdown();

	return 0;
}
#endif /* !defined(FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION) */
