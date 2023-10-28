#include "player.h"
#include <gba.h>

static int cur_x;
static int cur_y;

int draw_colored_rect(int x, int y, int width, int height, u16 color) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            ((u16*)VRAM)[(y + i) * 240 + (x + j)] = color;
        }
    }
    cur_x = x;
    cur_y = y;
    return 0;
}

int draw_player(int x, int y) {
    draw_colored_rect(x, y, 5, 5, RGB5(31, 0, 0));
    return 0;
}

int clear_player(void) {
    draw_colored_rect(cur_x, cur_y, 5, 5, RGB5(0, 0, 0));
    return 0;
}