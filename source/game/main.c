#include <unistd.h>
#include <gba.h>
#include "player.h"

#define MODE3 0x0003
#define BG2 0x0400
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160

int main(void) {
    // Set the video mode to mode 3 (full 15-bit color, bitmap mode)
    REG_DISPCNT = MODE3 | BG2;

    irqInit();
    irqEnable(IRQ_VBLANK);

    u16 backgroundColor = RGB8(0, 0, 0);
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        ((u16*)VRAM)[i] = backgroundColor;
    }

    int x = 60;
    int y = 80 - 5;

    while (1) {
        clear_player();

        x += 5;
        if (x > 230) x = 0;
        draw_player(x, y);

        VBlankIntrWait();
    }

    return 0;
}
