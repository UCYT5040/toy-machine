#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "font.h"

typedef struct {
        int balance;
        SDL_Texture *texture;
} Vault;

void vault_init(Vault *vault, SDL_Renderer *renderer);
void vault_add(Vault *vault, int amount);
bool vault_spend(Vault *vault, int amount);
void vault_render(const Vault *vault, SDL_Renderer *renderer, FontAtlas *font,
                  int window_width);