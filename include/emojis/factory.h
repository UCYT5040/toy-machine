#pragma once

#include "world.h"
#include "emoji_registry.h"

void emoji_factory_update(World *world, Entity *self, double dt);

void emoji_factory_collide(World *world, Entity *self, Entity *other);
