#pragma once

/* Define matrix key byte as defined by PPI return value */
#define MATRIX_KEY(row, value)  (((value) << 4)) | (row))

/* 0th row keys */
#define KEY_0                   MATRIX_KEY(0, 0)
#define KEY_1                   MATRIX_KEY(0, 1)
#define KEY_2                   MATRIX_KEY(0, 2)
#define KEY_3                   MATRIX_KEY(0, 3)
#define KEY_4                   MATRIX_KEY(0, 4)
#define KEY_5                   MATRIX_KEY(0, 5)
#define KEY_6                   MATRIX_KEY(0, 6)
#define KEY_7                   MATRIX_KEY(0, 7)

/* 1st row keys */
#define KEY_8                   MATRIX_KEY(1, 0)
#define KEY_9                   MATRIX_KEY(1, 1)
#define KEY_MINUS               MATRIX_KEY(1, 2)
#define KEY_EQUAL               MATRIX_KEY(1, 3)
#define KEY_BACKSLASH           MATRIX_KEY(1, 4)
#define KEY_OPENBRACKETS        MATRIX_KEY(1, 5)
#define KEY_CLOSEBRACKETS       MATRIX_KEY(1, 6)
#define KEY_SEMICOLON           MATRIX_KEY(1, 7)

/* 2nd row keys */
#define KEY_APOSTROPHE          MATRIX_KEY(2, 0)
#define KEY_GRAVE               MATRIX_KEY(2, 1)
#define KEY_COMMA               MATRIX_KEY(2, 2)
#define KEY_DOT                 MATRIX_KEY(2, 3)
#define KEY_SLASH               MATRIX_KEY(2, 4)
#define KEY_CURRENCY            MATRIX_KEY(2, 5)
#define KEY_A                   MATRIX_KEY(2, 6)
#define KEY_B                   MATRIX_KEY(2, 7)

/* 3rd row keys */
#define KEY_C                   MATRIX_KEY(3, 0)
#define KEY_D                   MATRIX_KEY(3, 1)
#define KEY_E                   MATRIX_KEY(3, 2)
#define KEY_F                   MATRIX_KEY(3, 3)
#define KEY_G                   MATRIX_KEY(3, 4)
#define KEY_H                   MATRIX_KEY(3, 5)
#define KEY_I                   MATRIX_KEY(3, 6)
#define KEY_J                   MATRIX_KEY(3, 7)

// 4th row keys
#define KEY_K                   MATRIX_KEY(4, 0)
#define KEY_L                   MATRIX_KEY(4, 1)
#define KEY_M                   MATRIX_KEY(4, 2)
#define KEY_N                   MATRIX_KEY(4, 3)
#define KEY_O                   MATRIX_KEY(4, 4)
#define KEY_P                   MATRIX_KEY(4, 5)
#define KEY_Q                   MATRIX_KEY(4, 6)
#define KEY_R                   MATRIX_KEY(4, 7)

/* 5th row keys */
#define KEY_S                   MATRIX_KEY(5, 0)
#define KEY_T                   MATRIX_KEY(5, 1)
#define KEY_U                   MATRIX_KEY(5, 2)
#define KEY_V                   MATRIX_KEY(5, 3)
#define KEY_W                   MATRIX_KEY(5, 4)
#define KEY_X                   MATRIX_KEY(5, 5)
#define KEY_Y                   MATRIX_KEY(5, 6)
#define KEY_Z                   MATRIX_KEY(5, 7)

/* 6th row keys */
#define KEY_SHIFT               MATRIX_KEY(6, 0)
#define KEY_CTRL                MATRIX_KEY(6, 1)
#define KEY_GRAPH               MATRIX_KEY(6, 2)
#define KEY_CAPS                MATRIX_KEY(6, 3)
#define KEY_CODE                MATRIX_KEY(6, 4)
#define KEY_F1                  MATRIX_KEY(6, 5)
#define KEY_F2                  MATRIX_KEY(6, 6)
#define KEY_F3                  MATRIX_KEY(6, 7)

/* 7th row keys */
#define KEY_F4                  MATRIX_KEY(7, 0)
#define KEY_F5                  MATRIX_KEY(7, 1)
#define KEY_ESC                 MATRIX_KEY(7, 2)
#define KEY_TAB                 MATRIX_KEY(7, 3)
#define KEY_STOP                MATRIX_KEY(7, 4)
#define KEY_BS                  MATRIX_KEY(7, 5)
#define KEY_SELECT              MATRIX_KEY(7, 6)
#define KEY_RET                 MATRIX_KEY(7, 7)

/* 8th row keys */
#define KEY_SPACE               MATRIX_KEY(8, 0)
#define KEY_HOME                MATRIX_KEY(8, 1)
#define KEY_INS                 MATRIX_KEY(8, 2)
#define KEY_DEL                 MATRIX_KEY(8, 3)
#define KEY_LEFT                MATRIX_KEY(8, 4)
#define KEY_UP                  MATRIX_KEY(8, 5)
#define KEY_DOWN                MATRIX_KEY(8, 6)
#define KEY_RIGHT               MATRIX_KEY(8, 7)

/* 9th row keys */
#define KEY_NUM_MUL             MATRIX_KEY(9, 0)
#define KEY_NUM_ADD             MATRIX_KEY(9, 1)
#define KEY_NUM_DIV             MATRIX_KEY(9, 2)
#define KEY_NUM_0               MATRIX_KEY(9, 3)
#define KEY_NUM_1               MATRIX_KEY(9, 4)
#define KEY_NUM_2               MATRIX_KEY(9, 5)
#define KEY_NUM_3               MATRIX_KEY(9, 6)
#define KEY_NUM_4               MATRIX_KEY(9, 7)

/* 10th row keys */
#define KEY_NUM_5               MATRIX_KEY(10, 0)
#define KEY_NUM_6               MATRIX_KEY(10, 1)
#define KEY_NUM_7               MATRIX_KEY(10, 2)
#define KEY_NUM_8               MATRIX_KEY(10, 3)
#define KEY_NUM_9               MATRIX_KEY(10, 4)
#define KEY_NUM_MIN             MATRIX_KEY(10, 5)
#define KEY_NUM_COM             MATRIX_KEY(10, 6)
#define KEY_NUM_DOT             MATRIX_KEY(10, 7)
