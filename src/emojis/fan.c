#include <math.h>

#include "world.h"

void emoji_fan_update(World *world, Entity *self, double dt) {
        Entity *in_stream[MAX_ENTITIES];
        int in_stream_count = 0;

        if (self->angle == 0 || self->angle == M_PI) {
                for (int i = 0; i < MAX_ENTITIES; i++) {
                        Entity *other = &world->entities[i];
                }
        }

        double m = tan(self->angle - M_PI_2);
}