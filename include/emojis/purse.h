#pragma once

#include "emoji_registry.h"
#include "world.h"

void emoji_purse_update(World *world, Entity *self, double dt);

void emoji_purse_collide(World *world, Entity *self, Entity *other);
