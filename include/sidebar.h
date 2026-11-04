#pragma once

#include "world.h"
#include <SDL2/SDL.h>

// Width of the sidebar on the right side of the screen
#define SIDEBAR_WIDTH 300
// Negative factor to invert the scrolling direction
#define SCROLL_FACTOR -20.0

void draw_sidebar(SDL_Renderer *renderer, World *world, FontAtlas *font);

void sidebar_handle_mouse_down(SDL_Renderer *renderer, World *world);

void sidebar_handle_scroll(SDL_Renderer *renderer, World *world, float y);

bool pos_in_sidebar(SDL_Renderer *renderer, float x);