#pragma once

#include "stdbool.h"
#include "world.h"
#include <SDL2/SDL.h>

void world_handle_event(App *app, World *world, SDL_Renderer *renderer,
                        SDL_Event *event, bool *running);