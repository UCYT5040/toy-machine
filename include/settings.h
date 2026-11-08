#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "font.h"

#define SETTINGS_ICON_SIZE 48.0f
#define SETTINGS_ICON_MARGIN 16.0f
#define SETTINGS_ICON_GAP 16.0f

typedef struct {
        bool open;
        bool capturing_mouse;
} Settings;

void settings_init(Settings *settings);
bool settings_render_icon(SDL_Renderer *renderer, SDL_Texture *icon,
                          int window_width);
void settings_render_menu(const Settings *settings, SDL_Renderer *renderer,
                          FontAtlas *font, int window_width,
                          int window_height);
bool settings_handle_event(Settings *settings, SDL_Event *event,
                           int window_width, int window_height,
                           bool *popup_open);
