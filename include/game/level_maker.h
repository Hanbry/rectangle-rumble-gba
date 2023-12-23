#ifndef LEVELMAKER_H
#define LEVELMAKER_H

#include "physics.h"
#include "physics_object.h"

struct obstacle {
    int pos_x;     // 1 = 1px
    int pos_y;     // 1 = 1px
    int width;     // 1 = 1px
    int height;    // 1 = 1px
    int animated;  // 0 = static, 1 = animated
    int direction; // 0 = up/down, 1 = left/right
    int speed;     // 1 = 1px/frame
    int distance;  // 1 = 1px
};

void generate_level(float coef_a, float coef_b, int level_progress, struct obstacle *obstacles);

#endif // LEVELMAKER_H