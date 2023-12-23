#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <tonc.h>

#include "physics.h"
#include "physics_object.h"
#include "level_maker.h"

// Graphics
#include "player_gfx.h"
#include "background_gfx.h"
#include "boundaries_gfx.h"

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 160
#define PLAYER_JUMP_ACCEL 13000
#define GRAVITY 600
#define PLAYER_SPEED 80.0
#define SIMULATION_STEP 0.02

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

int main(void) {

    // Set up hardware registers
    REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D | DCNT_MODE0 | DCNT_BG0 | DCNT_BG1;
    // set up BG0 for a 4bpp 32x32t map, using
    // using charblock 0 and screenblock 30
    REG_BG0CNT = BG_CBB(0) | BG_PRIO(2) | BG_SBB(30) | BG_4BPP | BG_REG_32x32;
    REG_BG1CNT = BG_CBB(1) | BG_PRIO(1) | BG_SBB(31) | BG_4BPP | BG_REG_32x32;


    // Load graphics into memory
    // Player
    memcpy(&tile_mem[4][0], player_gfxTiles, player_gfxTilesLen);
    memcpy(&pal_obj_bank[0][0], player_gfxPal, player_gfxPalLen);

    // Boundaries
    memcpy(&tile_mem[4][1], boundaries_gfxTiles, boundaries_gfxTilesLen);
    memcpy(&pal_obj_bank[1][0], boundaries_gfxPal, boundaries_gfxPalLen);

    // Background
    memcpy(&tile_mem[0][0], background_gfxTiles, background_gfxTilesLen);
    memcpy(&se_mem[30][0], background_gfxMap, background_gfxMapLen);
    memcpy(&pal_bg_bank[0][0], background_gfxPal, background_gfxPalLen);

    // Set up text engine
    tte_init_con();
    tte_init_se(1,                      // Background number (BG 1)
                BG_CBB(1) | BG_SBB(31), // BG control (for REG_BGxCNT)
                0,                      // Tile offset (special cattr)
                RGB8(73, 77, 126),      // Ink color
                14,                     // BitUnpack offset (on-pixel = 15)
                NULL,                   // Default font (sys8) 
                NULL);                  // Default renderer (se_drawg_s)

    oam_init(obj_buffer, 128);

    // Create physics environment
    struct physics_environment env;
    init_physics(&env, SIMULATION_STEP);
    env.gravity = GRAVITY;

    // Create player object
    OBJ_ATTR *player = obj_buffer;
    obj_set_attr(player, 
                 ATTR0_SQUARE | ATTR0_4BPP, // Square, regular sprite
                 ATTR1_SIZE_8,              // 8x8p,
                 ATTR2_PALBANK(0) | 0);     // palbank 0, tile 0

    struct physics_object player_obj = { .obj = player,
                                         .pos_x = 0,           .pos_y = 0,
                                         ._pos_x = 0,          ._pos_y = 0,
                                         .velo_x = 0,          .velo_y = 0,
                                         .accel_x = 0,         .accel_y = 0,
                                         .width = 8,           .height = 8,
                                         .physical_height = 6, .physical_width = 6,
                                         .mass = 1,
                                         .animation_speed = 0.0,
                                         .animation_frames = 0,
                                         .obj_tiles = 1,
                                         .fixed_position = 0 };
    add_object(&env, &player_obj);

    // Create boundarie~s objects
    struct physics_object boundary_objs[4];
    for (int i = 0; i < 4; i++) {
        OBJ_ATTR *boundary = obj_buffer + 1 + i;
        obj_set_attr(boundary, 
                     ATTR0_WIDE | ATTR0_4BPP, // Square, regular sprite
                     ATTR1_SIZE_64,           // 32x64p,
                     ATTR2_PALBANK(1) | 1);   // palbank 1, tile 1

        boundary_objs[i] = (struct physics_object){ .obj = boundary,
                                                    .pos_x = i * 64,       .pos_y = SCREEN_HEIGHT - 32,
                                                    ._pos_x = 0,           ._pos_y = 0,
                                                    .velo_x = 0,           .velo_y = 0,
                                                    .accel_x = 0,          .accel_y = 0,
                                                    .width = 64,           .height = 32,
                                                    .physical_height = 32, .physical_width = 64,
                                                    .mass = 1,
                                                    .animation_speed = 0.0,
                                                    .animation_frames = 0,
                                                    .obj_tiles = 32,
                                                    .fixed_position = 1 };

        
        add_object(&env, &boundary_objs[i]);
    }

    int level_counter = 1;
    while (1) {
        vid_vsync();
        key_poll();
        
        // Wait for physics calculations to finish
        while (!physics_done);
        // Hold physics calculations
        hold_physics();

        tte_printf("#{P:88,8}");
        tte_printf("Level %d", level_counter);

        player_obj.velo_x = PLAYER_SPEED;
        
        render_environment(&env, obj_buffer);
 
        // Game logic
        if (player_obj.pos_x > SCREEN_WIDTH) {
            // Create calculate hjump coefficients ax^2 + bx
            float a = -GRAVITY / (2 * PLAYER_SPEED * PLAYER_SPEED);
            float b = PLAYER_JUMP_ACCEL * SIMULATION_STEP / PLAYER_SPEED;
            
            // Generate level
            struct obstacle obstacles[5] = { 0 };
            generate_level(a, b, level_counter, obstacles);

            set_obj_position_x(&player_obj, 0);
            level_counter++;
        }

        for (int i = 0; i < 4; i++) {
            if (player_obj.pos_y >= boundary_objs[i].pos_y - player_obj.height) {
                set_obj_position_y(&player_obj, boundary_objs[i].pos_y - player_obj.height + 1);
                player_obj.velo_y = 0;
                player_obj.accel_y = 0;
            }
        }
        
        // Input handling
        if (key_hit(KEY_A) && player_obj.velo_y == 0) {
            player_obj.accel_y = -PLAYER_JUMP_ACCEL;
        }
        if (key_hit(KEY_B)) {
            // reset level
        }

        // Release physics calculations
        release_physics();
    }

    return 0;
}
