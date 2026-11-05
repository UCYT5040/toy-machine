#pragma once

#include "world.h"

#define WIND_SPEED 40.0

void emoji_wind_update(World *world, Entity *self, double dt);
void emoji_wind_collide(World *world, Entity *self, Entity *other);
void emoji_wind_wall_collide(World *world, Entity *self, Vec2 normal);