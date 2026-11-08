#pragma once

#include "stdbool.h"
#include "world.h"
#include <SDL2/SDL.h>

void world_handle_event(World *world, SDL_Renderer *renderer, SDL_Event *event,
                        bool *running);