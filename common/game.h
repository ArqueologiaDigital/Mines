#ifndef GAME_H
#define GAME_H
/* game.h: game specific functions (separated for easier reuse) */

#include "minefield.h"


#ifdef DRAW_TITLE_SCREEN
/**
 * Optional. Draws a title screen message when the game starts. It will be
 * called at the start of the game if the DRAW_TITLE_SCREEN macro is defined.
 */
void draw_title_screen(minefield* mf);
#endif /* DRAW_TITLE_SCREEN */

/**
 * Draws the background of the boardgame on screen.
 * Needs to be implemented on new platforms.
 */
void draw_minefield(minefield* mf);

/**
 * Draws the playfield of the boardgame on screen.
 * Needs to be implemented on new platforms.
 */
void draw_minefield_contents(minefield* mf);

#ifdef DRAW_GAME_OVER
/**
 * Optional. Draws a game over message when the game ends. It will be called
 * when the game ends if the DRAW_GAME_OVER macro is defined.
 */
void draw_game_over(minefield* mf);
#endif /* DRAW_GAME_OVER */

/**
 * Runs things in background, like music and animations.
 * Needs to be implemented on new platforms.
 */
void idle_loop(minefield* mf);

#endif /* GAME_H */
