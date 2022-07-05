/*
 * This is pretty much taken from Lameguy64's tutorial
 * http://lameguy64.net/tutorials/pstutorials/chapter1/4-controllers.html
 */
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <psxapi.h>
#include <psxgpu.h>
#include <psxpad.h>
#include "../../common/common.h"

u_char padbuff[2][34];

void start_pad() {
    InitPAD(padbuff[0], 34, padbuff[1], 34);    
    padbuff[0][0] = padbuff[0][1] = 0xff;
    padbuff[1][0] = padbuff[1][1] = 0xff;
    StartPAD();
}

uint8_t input_read(uint8_t source) {
    u_short button;
    button = *((u_short*) (padbuff[0] + 2));
    if (!(button & PAD_START))
        return MINE_INPUT_OPEN;
    // Directional
    if (!(button & PAD_UP))
        return MINE_INPUT_UP;
    if (!(button & PAD_DOWN))
        return MINE_INPUT_DOWN;
    if (!(button & PAD_LEFT))
        return MINE_INPUT_LEFT;
    if (!(button & PAD_RIGHT))
        return MINE_INPUT_RIGHT;
    return MINE_INPUT_IGNORED;
}

int random_number(int min, int max) {
    srand(GetRCnt(0));
    return (rand() % max);
}
