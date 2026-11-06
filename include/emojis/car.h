#pragma once

#include "world.h"

#define CAR_ACCELERATION 1000.0

void emoji_car_update(World *world, Entity *self, double dt);
void emoji_car_wall_collide(World *world, Entity *self, Vec2 normal);
