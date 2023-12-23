#ifndef PHYSICS_H
#define PHYSICS_H

#include <tonc.h>
#include "physics_object.h"

// Physics environment struct that contains all objects and the environment settings. Gravitiy is in m/s^2, friction is a value between 0 and 1.
struct physics_environment {
    float gravity;
    float x_friction;
    float y_friction;
    int num_objects;
    struct physics_object *objects[128];
};

// Variable that indicates whether the physics calculations are done or not. It is used for synchronization between the main loop and the physics loop.
extern volatile int physics_done;

void init_physics(struct physics_environment *env, float simulation_step);
void hold_physics();
void release_physics();
void add_object(struct physics_environment *env, struct physics_object *obj);
int check_collision(struct physics_object *obj1, struct physics_object *obj2);
void set_obj_position_x(struct physics_object *obj, int x);
void set_obj_position_y(struct physics_object *obj, int y);
int render_environment(struct physics_environment *env, OBJ_ATTR *obj_buffer);

#endif // PHYSICS_H