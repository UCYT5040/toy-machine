#pragma once

#include "world.h"

typedef struct {
        Vec2 pos;
        double angle_rad;
        double spread_rad;
        double min_speed;
        double max_speed;
        double max_spin; // in radians per second
} SpewConfig;

Entity *dispense_entity(World *world, EmojiType type, const SpewConfig *config);

void dispense_note(World *world, const char *text);

void dispense_currency(World *world, int amount);