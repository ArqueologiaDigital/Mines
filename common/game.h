/* game.h: game specific functions (separated for easier reuse) */
#ifndef GAME_H
#define GAME_H


#include "minefield.h"


/**
  * [Optional] Set up memory and resources of the board. The default
  * constructor is called if the `MINEFIELD_ALLOCATION` macro is not defined.
  *
  * Implementation details
  * ----------------------
  *
  * Depending on available system resources and compiler features, you may
  * prefer to statically reserve memory or allocate it dynamically.
  */
minefield* init_minefield();


/**
 * [Optional] Set minefield to initial state, setting number of bombs and their
 * positions in the grid. A default initialisation function is called if the
 * `RESET_MINEFIELD` macro is not defined.
 */
void reset_minefield(minefield* mf);


/**
 * [Optional] Free memory and resources of the board. The default destructor
 * is called if the `MINEFIELD_ALLOCATION` macro is not defined.
 *
 * Implementation details
 * ----------------------
 *
 * The same resources allocated by `init_minefield()` should be released by
 * this function.
 */
void free_minefield(minefield* mf);


/**
 * [Provided] Draws the background of the boardgame on screen.
 */
void draw_minefield(minefield* mf);


/**
 * [Provided] Draws the playfield of the boardgame on screen.
 */
void draw_minefield_contents(minefield* mf);


/**
 * [Optional] Draw the background image that surrounds the board.
 *
 * Implementation details
 * ----------------------
 *
 * If coding a tile-based platform that uses **video-tiles.h** header, you can
 * use the definition in **video-tiles.c** instead of creating your own.
 *
 * Note: some tile-based platforms use the `GROUND` tile to cover the whole
 * background area.
 *
 * See [tile_index](#tile_index) for the index value of the `GROUND` tile.
 */
void draw_scenario(minefield* mf);


#ifdef ENABLE_COUNTER
/**
 * [Optional] Display mine counter on screen.
 *
 * Implementation details
 * ----------------------
 *
 * If `ENABLE_COUNTER` is not defined, this function will not be called and
 * the mine counter will not be displayed and updated during the game loop.
 *
 * The set of tile characters must include all the digits and the negative
 * sign, since the counter may become negative.
 */
void update_counter(minefield* mf);
#endif /* ENABLE_COUNTER */

#ifdef ENABLE_TIMER
/**
 * [Optional] Display elapsed time on the screen.
 *
 * Implementation details
 * ----------------------
 *
 * If `ENABLE_TIMER` is not defined, this function will not be called
 * and the timer will not be displayed and updated during the game loop.
 *
 * The set of tile characters must include all the digits and the colon
 * to separate minutes and seconds.
 */
void update_timer();

/**
 * [Optional] Reset elapsed time before new game starts.
 *
 * Implementation details
 * ----------------------
 *
 * If `ENABLE_TIMER` is not defined, this function will not be called
 * and the timer will not be displayed and updated during the game loop.
 *
 * The set of tile characters must include all the digits and the colon
 * to separate minutes and seconds.
 */
void reset_timer();
#endif /* ENABLE_TIMER */

#ifdef MAIN_LOOP_REIMPLEMENTED
/**
 * [Optional] Reimplements the game loop on platforms that process their own
 * event loop. It will be called if the `MAIN_LOOP_REIMPLEMENTED` macro is
 * defined.
 */
void platform_main_loop(minefield* mf);
#endif /* MAIN_LOOP_REIMPLEMENTED */


#ifdef DRAW_TITLE_SCREEN
/**
 * [Optional] Draws a title screen message when the game starts. It will be
 * called at the start of the game if the `DRAW_TITLE_SCREEN` macro is defined.
 */
void draw_title_screen(minefield* mf);
#endif /* DRAW_TITLE_SCREEN */


#ifdef DRAW_GAME_OVER
/**
 * [Optional] Draws a game over message when the game ends. It will be called
 * when the game ends if the `DRAW_GAME_OVER` macro is defined.
 */
void draw_game_over(minefield* mf);
#endif /* DRAW_GAME_OVER */


/**
 * [Required] Runs things in background, like music and animations.
 *
 * Implementation details
 * ----------------------
 *
 * Refresh game state, like animation and music and when finished just call
 * `wait_ticks()` to wait for the next frame.
 */
void idle_update(minefield* mf);

#endif /* GAME_H */
