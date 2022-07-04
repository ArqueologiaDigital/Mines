#include <stdint.h>
#include "video-tiles.h"


#define TILE_POS_X(x, y) ((x) * 8)
static const uint8_t TILE_X[MAX_VIDEO_TILES] = {
    [ONE_BOMB] = TILE_POS_X(2, 0),
    [TWO_BOMBS] = TILE_POS_X(4, 0),
    [THREE_BOMBS] = TILE_POS_X(6, 0),
    [FOUR_BOMBS] = TILE_POS_X(8, 0),
    [FIVE_BOMBS] = TILE_POS_X(10, 0),
    [SIX_BOMBS] = TILE_POS_X(12, 0),
    [SEVEN_BOMBS] = TILE_POS_X(14, 0),
    [EIGHT_BOMBS] = TILE_POS_X(16, 0),
    [BLANK] = TILE_POS_X(18, 0),
    [FLAG] = TILE_POS_X(20, 0),
    [QUESTION_MARK] = TILE_POS_X(22, 0),
    [EMPTY_FLAG] = TILE_POS_X(24, 0),
    [BOMB] = TILE_POS_X(26, 0),
    [EXPLOSION] = TILE_POS_X(28, 0),
    [GROUND] = TILE_POS_X(30, 0),

    [BOMB_ICON] = TILE_POS_X(0, 1),
    [AMPERSAND] = TILE_POS_X(1, 1),

    [MINEFIELD_CORNER_TOP_LEFT] = TILE_POS_X(0, 2),
    [MINEFIELD_TOP_TEE] = TILE_POS_X(1, 2),
    [MINEFIELD_HORIZONTAL_TOP] = TILE_POS_X(2, 2),
    [MINEFIELD_HORIZONTAL_MIDDLE] = TILE_POS_X(2, 3),
    [MINEFIELD_HORIZONTAL_BOTTOM] = TILE_POS_X(2, 5),
    [MINEFIELD_CORNER_TOP_RIGHT] = TILE_POS_X(4, 2),

    [MINEFIELD_LEFT_TEE] = TILE_POS_X(0, 3),
    [MINEFIELD_CROSS] = TILE_POS_X(1, 3),
    [MINEFIELD_RIGHT_TEE] = TILE_POS_X(4, 3),

    [MINEFIELD_VERTICAL_LEFT] = TILE_POS_X(0, 4),
    [MINEFIELD_VERTICAL_MIDDLE] = TILE_POS_X(2, 3),
    [MINEFIELD_VERTICAL_RIGHT] = TILE_POS_X(4, 4),
    [CLOSED_CELL] = TILE_POS_X(2, 4),

    [MINEFIELD_CORNER_BOTTOM_LEFT] = TILE_POS_X(0, 5),
    [MINEFIELD_BOTTOM_TEE] = TILE_POS_X(1, 5),
    [MINEFIELD_CORNER_BOTTOM_RIGHT] = TILE_POS_X(4, 5),

    [CORNER_TOP_LEFT] = TILE_POS_X(7, 2),
    [CORNER_TOP_RIGHT] = TILE_POS_X(10, 2),
    [CORNER_BOTTOM_LEFT] = TILE_POS_X(7, 5),
    [CORNER_BOTTOM_RIGHT] = TILE_POS_X(10, 5),

    [TOP_BORDER__LEFT] = TILE_POS_X(8, 2),
    [TOP_BORDER__RIGHT] = TILE_POS_X(9, 2),
    [BOTTOM_BORDER__LEFT] = TILE_POS_X(8, 5),
    [BOTTOM_BORDER__RIGHT] = TILE_POS_X(9, 5),

    [LEFT_BORDER__TOP] = TILE_POS_X(7, 3),
    [LEFT_BORDER__BOTTOM] = TILE_POS_X(7, 4),
    [RIGHT_BORDER__TOP] = TILE_POS_X(10, 3),
    [RIGHT_BORDER__BOTTOM] = TILE_POS_X(10, 4),

    [COLON] = TILE_POS_X(0, 6),
    [NO_SIGN] = TILE_POS_X(12, 6),
    [MINUS_SIGN] = TILE_POS_X(1, 6),
    [ZERO_DIGIT] = TILE_POS_X(2, 6),
    [ONE_DIGIT] = TILE_POS_X(3, 6),
    [TWO_DIGIT] = TILE_POS_X(4, 6),
    [THREE_DIGIT] = TILE_POS_X(5, 6),
    [FOUR_DIGIT] = TILE_POS_X(6, 6),
    [FIVE_DIGIT] = TILE_POS_X(7, 6),
    [SIX_DIGIT] = TILE_POS_X(8, 6),
    [SEVEN_DIGIT] = TILE_POS_X(9, 6),
    [EIGHT_DIGIT] = TILE_POS_X(10, 6),
    [NINE_DIGIT] = TILE_POS_X(11, 6),
};


#define PAGE2_OFFSET 256
#define TILE_POS_Y(x, y) ((y) * 8 + PAGE2_OFFSET)
static const uint16_t TILE_Y[MAX_VIDEO_TILES] = {
    [ONE_BOMB] = TILE_POS_Y(2, 0),
    [TWO_BOMBS] = TILE_POS_Y(4, 0),
    [THREE_BOMBS] = TILE_POS_Y(6, 0),
    [FOUR_BOMBS] = TILE_POS_Y(8, 0),
    [FIVE_BOMBS] = TILE_POS_Y(10, 0),
    [SIX_BOMBS] = TILE_POS_Y(12, 0),
    [SEVEN_BOMBS] = TILE_POS_Y(14, 0),
    [EIGHT_BOMBS] = TILE_POS_Y(16, 0),
    [BLANK] = TILE_POS_Y(18, 0),
    [FLAG] = TILE_POS_Y(20, 0),
    [QUESTION_MARK] = TILE_POS_Y(22, 0),
    [EMPTY_FLAG] = TILE_POS_Y(24, 0),
    [BOMB] = TILE_POS_Y(26, 0),
    [EXPLOSION] = TILE_POS_Y(28, 0),
    [GROUND] = TILE_POS_Y(30, 0),

    [BOMB_ICON] = TILE_POS_Y(0, 1),
    [AMPERSAND] = TILE_POS_Y(1, 1),

    [MINEFIELD_CORNER_TOP_LEFT] = TILE_POS_Y(0, 2),
    [MINEFIELD_TOP_TEE] = TILE_POS_Y(1, 2),
    [MINEFIELD_HORIZONTAL_TOP] = TILE_POS_Y(2, 2),
    [MINEFIELD_HORIZONTAL_MIDDLE] = TILE_POS_Y(2, 3),
    [MINEFIELD_HORIZONTAL_BOTTOM] = TILE_POS_Y(2, 5),
    [MINEFIELD_CORNER_TOP_RIGHT] = TILE_POS_Y(4, 2),

    [MINEFIELD_LEFT_TEE] = TILE_POS_Y(0, 3),
    [MINEFIELD_CROSS] = TILE_POS_Y(1, 3),
    [MINEFIELD_RIGHT_TEE] = TILE_POS_Y(4, 3),

    [MINEFIELD_VERTICAL_LEFT] = TILE_POS_Y(0, 4),
    [MINEFIELD_VERTICAL_MIDDLE] = TILE_POS_Y(2, 3),
    [MINEFIELD_VERTICAL_RIGHT] = TILE_POS_Y(4, 4),
    [CLOSED_CELL] = TILE_POS_Y(2, 4),

    [MINEFIELD_CORNER_BOTTOM_LEFT] = TILE_POS_Y(0, 5),
    [MINEFIELD_BOTTOM_TEE] = TILE_POS_Y(1, 5),
    [MINEFIELD_CORNER_BOTTOM_RIGHT] = TILE_POS_Y(4, 5),

    [CORNER_TOP_LEFT] = TILE_POS_Y(7, 2),
    [CORNER_TOP_RIGHT] = TILE_POS_Y(10, 2),
    [CORNER_BOTTOM_LEFT] = TILE_POS_Y(7, 5),
    [CORNER_BOTTOM_RIGHT] = TILE_POS_Y(10, 5),

    [TOP_BORDER__LEFT] = TILE_POS_Y(8, 2),
    [TOP_BORDER__RIGHT] = TILE_POS_Y(9, 2),
    [BOTTOM_BORDER__LEFT] = TILE_POS_Y(8, 5),
    [BOTTOM_BORDER__RIGHT] = TILE_POS_Y(9, 5),

    [LEFT_BORDER__TOP] = TILE_POS_Y(7, 3),
    [LEFT_BORDER__BOTTOM] = TILE_POS_Y(7, 4),
    [RIGHT_BORDER__TOP] = TILE_POS_Y(10, 3),
    [RIGHT_BORDER__BOTTOM] = TILE_POS_Y(10, 4),

    [COLON] = TILE_POS_Y(0, 6),
    [NO_SIGN] = TILE_POS_Y(12, 6),
    [MINUS_SIGN] = TILE_POS_Y(1, 6),
    [ZERO_DIGIT] = TILE_POS_Y(2, 6),
    [ONE_DIGIT] = TILE_POS_Y(3, 6),
    [TWO_DIGIT] = TILE_POS_Y(4, 6),
    [THREE_DIGIT] = TILE_POS_Y(5, 6),
    [FOUR_DIGIT] = TILE_POS_Y(6, 6),
    [FIVE_DIGIT] = TILE_POS_Y(7, 6),
    [SIX_DIGIT] = TILE_POS_Y(8, 6),
    [SEVEN_DIGIT] = TILE_POS_Y(9, 6),
    [EIGHT_DIGIT] = TILE_POS_Y(10, 6),
    [NINE_DIGIT] = TILE_POS_Y(11, 6),
};
