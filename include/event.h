#pragma once
#include "stdbool.h"
#include "world.h"
#include <SDL2/SDL.h>

void handle_event(World *world, SDL_Event *event, bool *running);