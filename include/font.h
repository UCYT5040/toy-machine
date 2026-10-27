#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

typedef struct {
        SDL_Texture *glyphs[128];
        int widths[128];
        int heights[128];
        int line_height;
} FontAtlas;

bool font_init(FontAtlas *atlas, SDL_Renderer *renderer, const char *font_path,
               int font_size);
void font_draw(FontAtlas *atlas, SDL_Renderer *renderer, const char *text,
               double x, double y, SDL_Color color);
void font_destroy(FontAtlas *atlas);