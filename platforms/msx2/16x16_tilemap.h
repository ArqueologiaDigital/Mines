#define TILE_POS_X(x, y) ((x) * 8)
static const uint8_t TILE_X[MAX_VIDEO_TILES] = {
    [ONE_BOMB] = TILE_POS_X(4, 0),
    [TWO_BOMBS] = TILE_POS_X(6, 0),
    [THREE_BOMBS] = TILE_POS_X(8, 0),
    [FOUR_BOMBS] = TILE_POS_X(10, 0),
    [FIVE_BOMBS] = TILE_POS_X(12, 0),
    [SIX_BOMBS] = TILE_POS_X(14, 0),
    [SEVEN_BOMBS] = TILE_POS_X(16, 0),
    [EIGHT_BOMBS] = TILE_POS_X(18, 0),

    [BLANK] = TILE_POS_X(2, 0),
    [CLOSED_CELL] = TILE_POS_X(26, 0),
    [FLAG] = TILE_POS_X(28, 0),
    [QUESTION_MARK] = TILE_POS_X(30, 0),
    [EMPTY_FLAG] = TILE_POS_X(24, 0),
    [BOMB] = TILE_POS_X(20, 0),
    [EXPLOSION] = TILE_POS_X(22, 0),
    [GROUND] = TILE_POS_X(0, 2),

    [BOMB_ICON] = TILE_POS_X(0, 1),
    [HOURGLASS] = TILE_POS_X(1, 1),

    [MINEFIELD_CORNER_TOP_LEFT] = TILE_POS_X(0, 4),
    [MINEFIELD_HORIZONTAL_TOP] = TILE_POS_X(1, 4),
    [MINEFIELD_CORNER_TOP_RIGHT] = TILE_POS_X(2, 4),
    [MINEFIELD_VERTICAL_LEFT] = TILE_POS_X(0, 5),
    [MINEFIELD_VERTICAL_RIGHT] = TILE_POS_X(2, 5),
    [MINEFIELD_CORNER_BOTTOM_LEFT] = TILE_POS_X(0, 6),
    [MINEFIELD_HORIZONTAL_BOTTOM] = TILE_POS_X(1, 6),
    [MINEFIELD_CORNER_BOTTOM_RIGHT] = TILE_POS_X(2, 6),

    [FRAME_TOP_LEFT] = TILE_POS_X(3, 4),
    [FRAME_TOP_CENTER] = TILE_POS_X(4, 4),
    [FRAME_TOP_RIGHT] = TILE_POS_X(5, 4),
    [FRAME_VERTICAL_LEFT] = TILE_POS_X(3, 5),
    [FRAME_VERTICAL_RIGHT] = TILE_POS_X(5, 5),
    [FRAME_BOTTOM_LEFT] = TILE_POS_X(3, 6),
    [FRAME_BOTTOM_CENTER] = TILE_POS_X(4, 6),
    [FRAME_BOTTOM_RIGHT] = TILE_POS_X(5, 6),

    [COLON] = TILE_POS_X(6, 4),
    [NO_SIGN] = TILE_POS_X(1, 5),
    [MINUS_SIGN] = TILE_POS_X(7, 4),
    [ZERO_DIGIT] = TILE_POS_X(8, 4),
    [ONE_DIGIT] = TILE_POS_X(9, 4),
    [TWO_DIGIT] = TILE_POS_X(10, 4),
    [THREE_DIGIT] = TILE_POS_X(11, 4),
    [FOUR_DIGIT] = TILE_POS_X(12, 4),
    [FIVE_DIGIT] = TILE_POS_X(13, 4),
    [SIX_DIGIT] = TILE_POS_X(14, 4),
    [SEVEN_DIGIT] = TILE_POS_X(15, 4),
    [EIGHT_DIGIT] = TILE_POS_X(16, 4),
    [NINE_DIGIT] = TILE_POS_X(17, 4),

    /* not used:
    [MINEFIELD_VERTICAL_MIDDLE]
    [MINEFIELD_CROSS]
    [MINEFIELD_HORIZONTAL_TOP]
    [MINEFIELD_HORIZONTAL_MIDDLE]
    [MINEFIELD_HORIZONTAL_BOTTOM]
    [MINEFIELD_TOP_TEE]
    [MINEFIELD_BOTTOM_TEE]
    [MINEFIELD_LEFT_TEE]
    [MINEFIELD_RIGHT_TEE]
    [CORNER_TOP_LEFT]
    [CORNER_TOP_RIGHT]
    [CORNER_BOTTOM_LEFT]
    [CORNER_BOTTOM_RIGHT]
    [LEFT_BORDER__TOP]
    [RIGHT_BORDER__TOP]
    [LEFT_BORDER__BOTTOM]
    [RIGHT_BORDER__BOTTOM]
    [TOP_BORDER__LEFT]
    [BOTTOM_BORDER__LEFT]
    [TOP_BORDER__RIGHT]
    [BOTTOM_BORDER__RIGHT]
    */
};


#define PAGE2_OFFSET 256
#define TILE_POS_Y(x, y) ((y) * 8 + PAGE2_OFFSET)
static const uint16_t TILE_Y[MAX_VIDEO_TILES] = {
    [ONE_BOMB] = TILE_POS_Y(4, 0),
    [TWO_BOMBS] = TILE_POS_Y(6, 0),
    [THREE_BOMBS] = TILE_POS_Y(8, 0),
    [FOUR_BOMBS] = TILE_POS_Y(10, 0),
    [FIVE_BOMBS] = TILE_POS_Y(12, 0),
    [SIX_BOMBS] = TILE_POS_Y(14, 0),
    [SEVEN_BOMBS] = TILE_POS_Y(16, 0),
    [EIGHT_BOMBS] = TILE_POS_Y(18, 0),

    [BLANK] = TILE_POS_Y(2, 0),
    [CLOSED_CELL] = TILE_POS_Y(26, 0),
    [FLAG] = TILE_POS_Y(28, 0),
    [QUESTION_MARK] = TILE_POS_Y(30, 0),
    [EMPTY_FLAG] = TILE_POS_Y(24, 0),
    [BOMB] = TILE_POS_Y(20, 0),
    [EXPLOSION] = TILE_POS_Y(22, 0),
    [GROUND] = TILE_POS_Y(0, 2),

    [BOMB_ICON] = TILE_POS_Y(0, 1),
    [HOURGLASS] = TILE_POS_Y(1, 1),

    [MINEFIELD_CORNER_TOP_LEFT] = TILE_POS_Y(0, 4),
    [MINEFIELD_HORIZONTAL_TOP] = TILE_POS_Y(1, 4),
    [MINEFIELD_CORNER_TOP_RIGHT] = TILE_POS_Y(2, 4),
    [MINEFIELD_VERTICAL_LEFT] = TILE_POS_Y(0, 5),
    [MINEFIELD_VERTICAL_RIGHT] = TILE_POS_Y(2, 5),
    [MINEFIELD_CORNER_BOTTOM_LEFT] = TILE_POS_Y(0, 6),
    [MINEFIELD_HORIZONTAL_BOTTOM] = TILE_POS_Y(1, 6),
    [MINEFIELD_CORNER_BOTTOM_RIGHT] = TILE_POS_Y(2, 6),

    [FRAME_TOP_LEFT] = TILE_POS_Y(3, 4),
    [FRAME_TOP_CENTER] = TILE_POS_Y(4, 4),
    [FRAME_TOP_RIGHT] = TILE_POS_Y(5, 4),
    [FRAME_VERTICAL_LEFT] = TILE_POS_Y(3, 5),
    [FRAME_VERTICAL_RIGHT] = TILE_POS_Y(5, 5),
    [FRAME_BOTTOM_LEFT] = TILE_POS_Y(3, 6),
    [FRAME_BOTTOM_CENTER] = TILE_POS_Y(4, 6),
    [FRAME_BOTTOM_RIGHT] = TILE_POS_Y(5, 6),

    [COLON] = TILE_POS_Y(6, 4),
    [NO_SIGN] = TILE_POS_Y(1, 5),
    [MINUS_SIGN] = TILE_POS_Y(7, 4),
    [ZERO_DIGIT] = TILE_POS_Y(8, 4),
    [ONE_DIGIT] = TILE_POS_Y(9, 4),
    [TWO_DIGIT] = TILE_POS_Y(10, 4),
    [THREE_DIGIT] = TILE_POS_Y(11, 4),
    [FOUR_DIGIT] = TILE_POS_Y(12, 4),
    [FIVE_DIGIT] = TILE_POS_Y(13, 4),
    [SIX_DIGIT] = TILE_POS_Y(14, 4),
    [SEVEN_DIGIT] = TILE_POS_Y(15, 4),
    [EIGHT_DIGIT] = TILE_POS_Y(16, 4),
    [NINE_DIGIT] = TILE_POS_Y(17, 4),

    /* not used:
    [MINEFIELD_VERTICAL_MIDDLE]
    [MINEFIELD_CROSS]
    [MINEFIELD_HORIZONTAL_TOP]
    [MINEFIELD_HORIZONTAL_MIDDLE]
    [MINEFIELD_HORIZONTAL_BOTTOM]
    [MINEFIELD_TOP_TEE]
    [MINEFIELD_BOTTOM_TEE]
    [MINEFIELD_LEFT_TEE]
    [MINEFIELD_RIGHT_TEE]
    [CORNER_TOP_LEFT]
    [CORNER_TOP_RIGHT]
    [CORNER_BOTTOM_LEFT]
    [CORNER_BOTTOM_RIGHT]
    [LEFT_BORDER__TOP]
    [RIGHT_BORDER__TOP]
    [LEFT_BORDER__BOTTOM]
    [RIGHT_BORDER__BOTTOM]
    [TOP_BORDER__LEFT]
    [BOTTOM_BORDER__LEFT]
    [TOP_BORDER__RIGHT]
    [BOTTOM_BORDER__RIGHT]
    */
};
