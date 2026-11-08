#pragma once

#include "world.h"
#include <SDL2/SDL.h>

// Width of the sidebar on the right side of the screen
#define SIDEBAR_WIDTH 300
// Negative factor to invert the scrolling direction
#define SCROLL_FACTOR -20.0

void draw_sidebar(App *app, World *world, FontAtlas *font);

void sidebar_handle_mouse_down(App *app, World *world);

void sidebar_handle_scroll(App *app, World *world, float y);

bool pos_in_sidebar(App *app, Vec2 *pos);