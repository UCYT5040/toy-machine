#include "emojis/wind.h"
#include "vec2.h"
#include "world.h"

#include <math.h>

void emoji_wind_update(World *world, Entity *self, double dt) {
        self->v.y += sin(self->angle) * WIND_SPEED;
        self->v.x += cos(self->angle) * WIND_SPEED;
}

void emoji_wind_collide(World *world, Entity *self, Entity *other) {
        Vec2 d = vec2_sub(self->pos, other->pos);
        self->angle = atan2(d.y, d.x);
}

void emoji_wind_wall_collide(World *world, Entity *self, Vec2 normal) {
        self->angle = atan2(normal.y, normal.x);
}