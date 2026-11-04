#pragma once

#include "world.h"
#include <SDL2/SDL.h>

#define SIDEBAR_WIDTH 300

void draw_sidebar(SDL_Renderer *renderer, World *world, FontAtlas *font);

void sidebar_handle_mouse_down(World *world);

bool pos_in_sidebar(SDL_Renderer *renderer, float x);