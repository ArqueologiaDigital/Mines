#include "common.h"
#include "minefield.h"
#include <stdio.h>
#include <stdlib.h>


static inline uint8_t getch(void)
{
    uint8_t key;
    asm __volatile("int $0x16" : "=Ral"(key) : "0"((unsigned char)0));
    return key;
}


uint8_t input_read(uint8_t source)
{
    char c = getch();

    if (c == 0 || c == -32) {
        c = getch();
        switch (c) {
        case 72:
            return MINE_INPUT_UP;
        case 80:
            return MINE_INPUT_DOWN;
        case 77:
            return MINE_INPUT_RIGHT;
        case 75:
            return MINE_INPUT_LEFT;
        default:
            break;
        }
    } else if (c == (int)'\n' || c == (int)' ') {
        return MINE_INPUT_OPEN;
    } else if (c == (int)'b' || c == (int)'B') {
        return MINE_INPUT_OPEN_BLOCK;
    } else if (c == (int)'f' || c == (int)'F') {
        return MINE_INPUT_FLAG;
    } else if (c == (int)'q' || c == (int)'Q' || c == 27) {
        return MINE_INPUT_QUIT;
    }

    return MINE_INPUT_IGNORED;
}


static inline uint16_t nearly_divisionless(uint16_t s)
{
    /* 16-bit version of Lemire's "Nearly Divisionless Random Integer
     * Generation" routine. Paper: https://arxiv.org/abs/1805.10941 */
    uint16_t x = rand();
    uint32_t m = (uint32_t)x * (uint32_t)s;
    uint16_t l = (uint32_t)m;

    if (l < s) {
        uint32_t t = -s % s;

        while (l < t) {
            x = rand();
            m = (uint32_t)x * (uint32_t)s;
            l = (uint32_t)m;
        }
    }

    return m >> 16;
}


int random_number(int min_num, int max_num)
{
    return nearly_divisionless(max_num - min_num) + min_num;
}
