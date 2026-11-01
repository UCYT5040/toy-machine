#pragma once

#include "world.h"

void emoji_pin_update(World *world, Entity *self, double dt);
void emoji_pin_collide(World *world, Entity *self, Entity *other);
void eomji_pin_unpin(World *world, Entity *pin);