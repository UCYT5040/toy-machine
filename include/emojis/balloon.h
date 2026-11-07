#pragma once

#include "world.h"

#define BALLOON_ACCELERATION -200.0

struct emoji_balloon_ctx {
        double swerve_val;
        double swerve_dir;
};

void emoji_balloon_update(World *world, Entity *self, double dt);