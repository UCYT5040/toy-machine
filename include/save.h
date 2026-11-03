#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "world.h"

#define SAVE_SLOT_COUNT 9

bool save(const World *world, int slot, char *error, size_t error_size);
bool load(World *world, int slot, char *error, size_t error_size);