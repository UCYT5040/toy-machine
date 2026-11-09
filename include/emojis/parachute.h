#pragma once

#include "world.h"

void emoji_parachute_update(World *world, Entity *self, double dt);
void emoji_parachute_collide(World *world, Entity *self, Entity *other);
