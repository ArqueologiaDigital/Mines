enum {
    BLANK = 0,
    CURSOR = 0, // The MSX2 port defined this one. The name is not clear to me though.

    MINEFIELD_CORNER_TOP_LEFT,
    MINEFIELD_CORNER_TOP_RIGHT,
    MINEFIELD_CORNER_BOTTOM_LEFT,
    MINEFIELD_CORNER_BOTTOM_RIGHT,
    MINEFIELD_TOP_TEE,
    MINEFIELD_BOTTOM_TEE,
    MINEFIELD_LEFT_TEE,
    MINEFIELD_RIGHT_TEE,
    MINEFIELD_CROSS,
    MINEFIELD_VERTICAL,
    MINEFIELD_HORIZONTAL,

    /*    NOTE:
        These tile codes for the number
        of bombs *MUST* be sequential
        because the video code assumes
        that to be the case.    */
    ONE_BOMB,
    TWO_BOMBS,
    THREE_BOMBS,
    FOUR_BOMBS,
    FIVE_BOMBS,
    SIX_BOMBS,
    SEVEN_BOMBS,
    EIGHT_BOMBS,

    BOMB,
    FLAG,
    QUESTION_MARK,
    CLOSED_CELL,

    CORNER_TOP_LEFT,
    CORNER_TOP_RIGHT,
    CORNER_BOTTOM_LEFT,
    CORNER_BOTTOM_RIGHT,
    TOP_BORDER__LEFT,
    TOP_BORDER__RIGHT,
    BOTTOM_BORDER__LEFT,
    BOTTOM_BORDER__RIGHT,
    LEFT_BORDER__BOTTOM,
    LEFT_BORDER__TOP,
    RIGHT_BORDER__BOTTOM,
    RIGHT_BORDER__TOP,
};
