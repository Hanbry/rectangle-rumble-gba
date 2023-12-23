#include "physics_object.h"

// Function that animates the sprite of the physics object. It is called every frame. It takes the animation speed into account.
void animate_object(struct physics_object *obj) {
    static int animation_counter;
    animation_counter = obj->animation_speed;

    if (animation_counter == 0) {
        int tid = ((int)(obj->obj->attr2 & ATTR2_ID_MASK)) + 1;
        int pb = (int)(obj->obj->attr2 & ATTR2_PALBANK_MASK);
        obj->obj->attr2 = ATTR2_BUILD(tid, pb, 0);
        animation_counter = obj->animation_speed;
    } else {
        animation_counter--;
    }
}