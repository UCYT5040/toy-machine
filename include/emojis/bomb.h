#pragma once

#include "world.h"

#include <stdbool.h>

#define BOMB_TIMER 3000    // 3 seconds
#define BOMB_FORCE 50000.0 // Still pretty weak

struct emoji_bomb_ctx {
        int spawned;
};

void emoji_bomb_update(World *world, Entity *self, double dt);
void emoji_bomb_collide(World *world, Entity *self, Entity *other);