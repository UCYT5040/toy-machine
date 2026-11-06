#pragma once

#include "vec2.h"
#include "world.h"

#include <stdbool.h>

#define CAR_ACCELERATION 1000.0

struct emoji_car_ctx {
        bool inited;
        Vec2 last_wall_pos;
        Vec2 last_wall_normal;
};

void emoji_car_update(World *world, Entity *self, double dt);
void emoji_car_wall_collide(World *world, Entity *self, Vec2 normal);
