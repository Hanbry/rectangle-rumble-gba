#ifndef PHYSICS_OBJECT_H
#define PHYSICS_OBJECT_H

#include <tonc.h>

struct physics_object {
    OBJ_ATTR *obj;                                    // Object attribute
    int pos_x;                                        // 1 = 1px
    int pos_y;                                        // 1 = 1px
    float _pos_x;                                     // 1 = 1px
    float _pos_y;                                     // 1 = 1px
    float velo_x;                                     // 1 = 1px/s
    float velo_y;                                     // 1 = 1px/s
    float accel_x;                                    // 1 = 1px/s^2
    float accel_y;                                    // 1 = 1px/s^2
    int width;                                        // 1 = 1px
    int height;                                       // 1 = 1px
    int physical_height;                              // 1 = 1px
    int physical_width;                               // 1 = 1px
    int mass;                                         // 1 = 1kg
    float animation_speed;                            // 1 = 1 frames, 0 = no animation
    int animation_frames;                             // 1 = 1 frames
    int obj_tiles;                                    // 1 = 1 tile
    int fixed_position;                               // 1 = fixed position, 0 = not fixed position
};

#endif // PHYSICS_OBJECT_H