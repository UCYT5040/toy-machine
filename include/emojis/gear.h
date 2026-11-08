#pragma once

#include "world.h"

#define GEAR_ANGULAR_ACCELERATION 50.0

void emoji_gear_update(World *world, Entity *self, double dt);
void emoji_gear_collide(World *world, Entity *self, Entity *other);
