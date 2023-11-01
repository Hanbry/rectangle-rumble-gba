#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <tonc.h>

#include "player.h"
#include "level_maker.h"

#include "gfx_block.h"

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 160

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

int main(void) {
    REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D;

    memcpy(&tile_mem[4][0], blockTiles, blockTilesLen);
    memcpy(pal_obj_mem, blockPal, blockPalLen);

    oam_init(obj_buffer, 128);

    int x = 96, y = 32;
    u32 tid = 0, pb = 0;
    OBJ_ATTR *block = &obj_buffer[0];
    obj_set_attr(block, 
        ATTR0_SQUARE,               // Square, regular sprite
        ATTR1_SIZE_8,               // 8x8p, 
        ATTR2_PALBANK(pb) | tid);   // palbank 0, tile 0


    obj_set_pos(block, x, y);

    while (1) {
        vid_vsync();
        key_poll();

        x += 2*key_tri_horz();
        y += 2*key_tri_vert();

        tid += bit_tribool(key_hit(-1), KI_R, KI_L);

        if(key_hit(KEY_A))
            block->attr1 ^= ATTR1_HFLIP;
        if(key_hit(KEY_B))
            block->attr1 ^= ATTR1_VFLIP;
        
        if(key_hit(KEY_START))
            REG_DISPCNT ^= DCNT_OBJ_1D;

        block->attr2= ATTR2_BUILD(tid, pb, 0);
        obj_set_pos(block, x, y);

        oam_copy(oam_mem, obj_buffer, 1);
    }

    return 0;
}
