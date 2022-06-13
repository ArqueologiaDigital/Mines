#ifndef GAME_H
#define GAME_H
/* game.h: game specific functions (separated for easier reuse) */

#include "minefield.h"

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

/**
 * Runs things in background, like music and animations.
 * Needs to be implemented on new platforms.
 */
void idle_loop(minefield* mf);

#endif /* GAME_H */
