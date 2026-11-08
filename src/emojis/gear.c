#include "emojis/gear.h"
#include "world.h"

void emoji_gear_update(World *world, Entity *self, double dt) {
        if (self->omega <= 100.0 && world->pressed_entity_index != self->index)
                self->omega += GEAR_ANGULAR_ACCELERATION * dt;
}

void emoji_gear_collide(World *world, Entity *self, Entity *other) {
        (void)world;

        other->omega = -self->omega;
}