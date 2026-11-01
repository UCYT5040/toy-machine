#include "vault.h"

#include <stdio.h>
#include <SDL2/SDL_image.h>

#define VAULT_ICON_SIZE 48.0
#define VAULT_MARGIN 16.0
#define VAULT_TEXT_GAP 8.0

void vault_init(Vault *vault, SDL_Renderer, *renderer) {
        SDL_Surface *surface = IMG_Load("assets/vault.png");

        if (!surface) {
                return;
        }

        vault->texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
}

void vault_add(Vault *vault, int amount) {
        /* Negative amount (spending) */
        if (amount < 0) {
                return;
        }

        vault->balance += amount;

        if (vault->balance < 0) {
                vault->balance = 0;
        }
}

bool vault_spend(Vault *vault, int amount) {
        /* More money than vault has or no money at all */
        if (vault->balance < amount || amount < 0) {
                return false;
        }

        vault->balance -= amount;
        return true;
}

void vault_render(const Vault *vault, SDL_Renderer *renderer,
                  FontAtlas *font, int window_width) {
        SDL_FRect icon = {window_width - VAULT_MARGIN - VAULT_ICON_SIZE,
                          VAULT_MARGIN, VAULT_ICON_SIZE, VAULT_ICON_SIZE};
        
        if (vault->texture) {
                SDL_RenderCopyF(renderer, vault->texture, NULL, &icon);
        }

        if (!font) {
                return;
        }

        char text[32];
        snprintf(text, sizeof text, "%d", vault->balance);

        int text_width = font_measure_text(font, text);
        double text_x = icon.x - VUALT_TEXT_GAP - (double)text_width;
        double text_y = 
           icon.y + (VAULT_ICON_SIZE - (double)font->line_height) / 2.0;

        SDL_Color shadow = {0, 0, 0, 200};
        SDL_Color white = {255, 255, 255, 255};

        font_draw(font, renderer, text, text_x + 1.0, text_y + 1.0, shadow);
        font_draw(font, renderer, text, text_x, text_y, white);

}