#pragma once

#include "SDL2/SDL.h"
#include "font.h"

void paper_popup_render(SDL_Renderer *renderer, FontAtlas *font,
                        const char *text, SDL_Texture *popup_texture,
                        int renderer_w, int renderer_h);