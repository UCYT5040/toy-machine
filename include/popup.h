#pragma once

#include "SDL2/SDL.h"
#include "font.h"

typedef enum PopupType { POPUP_TYPE_PAPER, POPUP_TYPE_CREDITS } PopupType;

typedef struct World World;
typedef struct App App;

void popup_render(App *app, const char *text, World *world,
                  PopupType popup_type, int renderer_w, int renderer_h);