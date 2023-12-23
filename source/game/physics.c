#include <math.h>
#include "physics.h"

volatile int physics_done = 0;
static struct physics_environment *physics_env;

void calculate_physics(struct physics_environment *env);
int check_collision(struct physics_object *obj1, struct physics_object *obj2);
void calculate_bounce(struct physics_object *obj1, struct physics_object *obj2, float damping);

// Interrupt service routine that is called every 20ms. It calls the calculate_physics function.
void timer_isr() {
    calculate_physics(physics_env);
}

// Function that initializes the physics environment. It also initializes the calculation timer for the physics.
void init_physics(struct physics_environment *env, float simulation_step) {
    env->gravity = 9.81;
    env->x_friction = 0.0;
    env->y_friction = 0.0;
    env->num_objects = 0;
    physics_env = env;
    
    irq_init(NULL);
    irq_add(II_TIMER2, timer_isr);
    
    // 20 ms = 20000 us with period of 61.04 us (FREQ_1024) -> we need 328 ticks
    // TODO: calculate ticks incorporating a custom simulation step
    REG_TM2D = -328;
    REG_TM2CNT = TM_FREQ_1024 | TM_ENABLE | TM_IRQ;

    irq_enable(IRQ_TIMER2);
    REG_IME = 1;
}

// Function that holds the physics calculations. It is used to synchronize the physics calculations with the main loop.
void hold_physics() {
    irq_disable(IRQ_TIMER2);
}

// Function that releases the physics calculations. It is used to synchronize the physics calculations with the main loop.
void release_physics() {
    irq_enable(IRQ_TIMER2);
}

// Function that adds an object to the physics environment.
void add_object(struct physics_environment *env, struct physics_object *obj) {
    env->objects[env->num_objects] = obj;
    env->num_objects++;
}

// Function that calculates the physics of all objects in the environment. This function is called every 20ms. Velocities are in px/s, accelerations are in px/s^2.
void calculate_physics(struct physics_environment *env) {
    physics_done = 0;

    // Time interval in seconds (20ms)
    const float delta_time = 0.02;

    for (int i = 0; i < env->num_objects; i++) {
        struct physics_object *obj = env->objects[i];

        // Calculate new velocity considering acceleration and time interval
        obj->velo_x += obj->accel_x * delta_time;
        obj->velo_y += obj->accel_y * delta_time;
        
        // Apply friction to the velocity
        if (obj->fixed_position)
            continue;

        obj->velo_x *= (1 - env->x_friction);
        obj->velo_y *= (1 - env->y_friction);

        obj->_pos_x += obj->velo_x * delta_time;
        obj->_pos_y += obj->velo_y * delta_time;
        obj->pos_x = floor(obj->_pos_x);
        obj->pos_y = floor(obj->_pos_y);

        // Reset acceleration
        obj->accel_y = env->gravity;
        obj->accel_x = 0.0;
    }

    physics_done = 1;
}

// Function that calculates the collision between two objects. It returns 1 if the objects collide, 0 otherwise.
int check_collision(struct physics_object *obj1, struct physics_object *obj2) {
    int x_collision = obj1->pos_x < obj2->pos_x + obj2->physical_width && obj1->pos_x + obj1->physical_width > obj2->pos_x;
    int y_collision = obj1->pos_y < obj2->pos_y + obj2->physical_height && obj1->pos_y + obj1->physical_height > obj2->pos_y;
    
    if (x_collision && y_collision) return 1;

    return 0;
}

// Function that calculates the bounce of an object when it collides with another object. It takes two physics_objects and modifies their parameters. It takes a damping factor into account.
void calculate_bounce(struct physics_object *obj1, struct physics_object *obj2, float damping) {
    // Calculate the normal vector of the collision
    float normal_x = obj1->pos_x - obj2->pos_x;
    float normal_y = obj1->pos_y - obj2->pos_y;
    float normal_length = sqrt(normal_x * normal_x + normal_y * normal_y);
    normal_x /= normal_length;
    normal_y /= normal_length;

    // Calculate the relative velocity of the objects
    float rel_velo_x = obj1->velo_x - obj2->velo_x;
    float rel_velo_y = obj1->velo_y - obj2->velo_y;

    // Calculate the impulse
    float impulse = (-(1 + damping) * (rel_velo_x * normal_x + rel_velo_y * normal_y)) / (1 / obj1->mass + 1 / obj2->mass);

    // Calculate the new velocities
    obj1->velo_x += impulse * normal_x / obj1->mass;
    obj1->velo_y += impulse * normal_y / obj1->mass;
}

// Function that sets the x position of an object. It takes the object and the position in pixels.
void set_obj_position_x(struct physics_object *obj, int x) {
    obj->pos_x = x;
    obj->_pos_x = (float)x;
}

// Function that sets the y position of an object. It takes the object and the position in pixels.
void set_obj_position_y(struct physics_object *obj, int y) {
    obj->pos_y = y;
    obj->_pos_y = (float)y;
}

int render_environment(struct physics_environment *env, OBJ_ATTR *obj_buffer) {
    for (int i = 0; i < env->num_objects; i++) {
        struct physics_object *obj = env->objects[i];
        obj_set_pos(obj->obj, obj->pos_x, obj->pos_y);
    }

    oam_copy(oam_mem, obj_buffer, env->num_objects);

    return 0;
}