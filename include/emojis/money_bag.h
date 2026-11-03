#pragma once

#include "emoji_registry.h"
#include "world.h"

void emoji_money_bag_update(World *world, Entity *self, double dt);

void emoji_money_bag_collide(World *world, Entity *self, Entity *other);
