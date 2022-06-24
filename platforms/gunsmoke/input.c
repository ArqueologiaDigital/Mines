#include "common.h"
#include "minefield.h"
#include <stdlib.h>

#define SYSTEM ((uint8_t*) 0xC000)
#define P1 ((uint8_t*) 0xC001)
#define P2 ((uint8_t*) 0xC002)
#define DSW1 ((uint8_t*) 0xC003)
#define DSW2 ((uint8_t*) 0xC004)


uint8_t input_read(uint8_t source){
    uint8_t p1 = *P1;
    uint8_t p2 = *P2;

    if (source == KEYBOARD){ // <-- This looks incorrect :-P
        /* P1 directionals */
        if ((p1 & (1 << 0))==0) return MINE_INPUT_RIGHT;
        if ((p1 & (1 << 1))==0) return MINE_INPUT_LEFT;
        if ((p1 & (1 << 2))==0) return MINE_INPUT_DOWN;
        if ((p1 & (1 << 3))==0) return MINE_INPUT_UP;
        
        /* P1 buttons */
        if ((p1 & (1 << 4))==0) return MINE_INPUT_FLAG;
        if ((p1 & (1 << 5))==0) return MINE_INPUT_OPEN;
        if ((p1 & (1 << 6))==0) return MINE_INPUT_OPEN_BLOCK;

        /* P2 buttons */
        if ((p2 & (1 << 4))==0) return MINE_INPUT_QUIT;
//        if ((p2 & (1 << 5))==0) return MINE_INPUT_...;
//        if ((p2 & (1 << 6))==0) return MINE_INPUT_...;
    }
    return MINE_INPUT_IGNORED;
}


int random_number(int min_num, int max_num)
{
    int result = 0, low_num = 0, hi_num = 0;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; // include max_num in output
    } else {
        low_num = max_num + 1; // include max_num in output
        hi_num = min_num;
    }

    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
}
