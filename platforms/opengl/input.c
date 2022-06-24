#include "common.h"
#include "minefield.h"
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>

unsigned char last_key = MINE_INPUT_IGNORED;

void keyboard_normal_event(unsigned char key, int x, int y)
{
    switch(key){
        case ' ': last_key = MINE_INPUT_OPEN; break;
        case 'b': last_key = MINE_INPUT_OPEN_BLOCK; break;
        case 'B': last_key = MINE_INPUT_OPEN_BLOCK; break;
        case 'f': last_key = MINE_INPUT_FLAG; break;
        case 'F': last_key = MINE_INPUT_FLAG; break;
        case 'q': last_key = MINE_INPUT_QUIT; break;
        case 'Q': last_key = MINE_INPUT_QUIT; break;
        default: break;
    }
}


void keyboard_special_event(int key, int x, int y)
{
    switch(key){
        case GLUT_KEY_UP: last_key = MINE_INPUT_UP; break;
        case GLUT_KEY_DOWN: last_key = MINE_INPUT_DOWN; break;
        case GLUT_KEY_LEFT: last_key = MINE_INPUT_LEFT; break;
        case GLUT_KEY_RIGHT: last_key = MINE_INPUT_RIGHT; break;
        default: break;
    }
}


uint8_t input_read(uint8_t source)
{
    unsigned char key = last_key;
    last_key = MINE_INPUT_IGNORED;
    return key;
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
