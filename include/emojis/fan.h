#pragma once

#include "world.h"

#define FAN_ACCEL 1800.0
#define FAN_FALLOFF 300.0

void emoji_fan_update(World *world, Entity *self, double dt);