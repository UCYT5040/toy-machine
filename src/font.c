#include "font.h"

bool font_init(FontAtlas *atlas, SDL_Renderer *renderer, const char *font_path,
               int font_size) {
  if (TTF_Init() == -1)
    return false;

  TTF_Font *ttf = TTF_OpenFont(font_path, font_size);
  if (!ttf)
    return false;

  atlas->line_height = TTF_FontLineSkip(ttf);
  SDL_Color white = {255, 255, 255, 255};

  for (int c = 32; c < 127; c++) {
    char str[2] = {(char)c, '\0'};
    SDL_Surface *surface = TTF_RenderUTF8_Blended(ttf, str, white);
    if (surface) {
      atlas->glyphs[c] = SDL_CreateTextureFromSurface(renderer, surface);
      atlas->widths[c] = surface->w;
      atlas->heights[c] = surface->h;
      SDL_FreeSurface(surface);
    }
  }
  TTF_CloseFont(ttf);
  return true;
}
void font_draw(FontAtlas *atlas, SDL_Renderer *renderer, const char *text,
               float x, float y, SDL_Color color) {
  float cursor_x = x;
  float cursor_y = y;

  for (const char *p = text; *p; p++) {
    if (*p == '\n') {
      cursor_x = x;
      cursor_y += atlas->line_height;
      continue;
    }

    unsigned char c = (unsigned char)*p;
    if (c < 32 || c >= 127 || !atlas->glyphs[c]) {
      continue; // Skip unsupported characters
    }

    SDL_Texture *glyph_texture = atlas->glyphs[c];
    SDL_SetTextureColorMod(glyph_texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(glyph_texture, color.a);

    SDL_FRect dest_rect = {cursor_x, cursor_y, (float)atlas->widths[c],
                           (float)atlas->heights[c]};
    SDL_RenderCopyF(renderer, glyph_texture, NULL, &dest_rect);

    cursor_x += atlas->widths[c];
  }
}
void font_destroy(FontAtlas *atlas) {
  for (int c = 32; c < 127; c++) {
    if (atlas->glyphs[c]) {
      SDL_DestroyTexture(atlas->glyphs[c]);
      atlas->glyphs[c] = NULL;
    }
  }
  TTF_Quit();
}