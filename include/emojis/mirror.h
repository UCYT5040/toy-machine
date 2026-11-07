#pragma once

#include "world.h"

#include <stdbool.h>

#define MIRROR_DUPLICATION_TIMEOUT 2000 // In milliseconds

struct emoji_mirror_ctx {
        int last_action; // Get using SDL_GetTicks()
};

void emoji_mirror_collide(World *world, Entity *self, Entity *other);
