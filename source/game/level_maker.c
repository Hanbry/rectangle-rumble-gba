#include <tonc.h>

#include "level_maker.h"

void easy_obstacles() {
    // Static obstacles
}

void hard_obstacles() {
    // Animate the obstacles
}

void generate_level(float coef_a, float coef_b, int level_progress, struct obstacle *obstacles) {
    if (level_progress % 2) {
        easy_obstacles();
    } else {
        hard_obstacles();
    }
}
