#ifndef CODES_H
#define CODES_H

enum {
    // playfield tiles
    CLOSED_CELL,
    /* NOTE:
       The following digits *MUST* be
       sequential because the video code
       assumes that to be the case. */
    ONE_BOMB,
    TWO_BOMBS,
    THREE_BOMBS,
    FOUR_BOMBS,
    FIVE_BOMBS,
    SIX_BOMBS,
    SEVEN_BOMBS,
    EIGHT_BOMBS,

    BLANK,
    CURSOR,
    BOMB,
    FLAG,
    QUESTION_MARK,
    EXPLOSION,
    GROUND,

    EMPTY_FLAG,
    BOMB_ICON,
    HOURGLASS,

    // screen borders
    CORNER_TOP_LEFT,
    TOP_BORDER__LEFT,
    TOP_BORDER__RIGHT,
    CORNER_TOP_RIGHT,
    LEFT_BORDER__TOP,
    RIGHT_BORDER__TOP,
    LEFT_BORDER__BOTTOM,
    RIGHT_BORDER__BOTTOM,
    CORNER_BOTTOM_LEFT,
    BOTTOM_BORDER__LEFT,
    BOTTOM_BORDER__RIGHT,
    CORNER_BOTTOM_RIGHT,

    // minefield borders
    MINEFIELD_CORNER_TOP_LEFT,
    MINEFIELD_TOP_TEE,
    MINEFIELD_HORIZONTAL_TOP,
    MINEFIELD_CORNER_TOP_RIGHT,
    MINEFIELD_LEFT_TEE,
    MINEFIELD_CROSS,
    MINEFIELD_HORIZONTAL_MIDDLE,
    MINEFIELD_VERTICAL_MIDDLE,
    MINEFIELD_RIGHT_TEE,
    MINEFIELD_VERTICAL_LEFT,
    MINEFIELD_VERTICAL_RIGHT,
    MINEFIELD_CORNER_BOTTOM_LEFT,
    MINEFIELD_BOTTOM_TEE,
    MINEFIELD_HORIZONTAL_BOTTOM,
    MINEFIELD_CORNER_BOTTOM_RIGHT,

    // frame tiles for counter and timer
    FRAME_TOP_LEFT,
    FRAME_TOP_CENTER,
    FRAME_TOP_RIGHT,
    FRAME_VERTICAL_LEFT,
    FRAME_VERTICAL_RIGHT,
    FRAME_BOTTOM_LEFT,
    FRAME_BOTTOM_CENTER,
    FRAME_BOTTOM_RIGHT,

    // mine counter and timer
    COLON,
    NO_SIGN,
    MINUS_SIGN,
    /* NOTE:
       The following digits *MUST* be
       sequential because the video code
       assumes that to be the case. */
    ZERO_DIGIT,
    ONE_DIGIT,
    TWO_DIGIT,
    THREE_DIGIT,
    FOUR_DIGIT,
    FIVE_DIGIT,
    SIX_DIGIT,
    SEVEN_DIGIT,
    EIGHT_DIGIT,
    NINE_DIGIT,

    MAX_VIDEO_TILES,
};

#endif /* CODES_H */
