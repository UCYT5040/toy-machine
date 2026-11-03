#pragma once

#include "world.h"
#include <SDL2/SDL.h>

#define SIDEBAR_WIDTH 200

void draw_sidebar(SDL_Renderer *renderer, World *world, FontAtlas *font);