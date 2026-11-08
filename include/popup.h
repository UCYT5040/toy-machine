#pragma once

#include "SDL2/SDL.h"
#include "font.h"

typedef enum PopupType { POPUP_TYPE_PAPER, POPUP_TYPE_CREDITS } PopupType;

typedef struct World World;

void popup_render(SDL_Renderer *renderer, FontAtlas *font, const char *text,
                  World *world, PopupType popup_type, int renderer_w,
                  int renderer_h);