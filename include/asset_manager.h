#pragma once

#include "emoji_registry.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

typedef struct AssetManager {
        SDL_Texture *emojis[EMOJI_COUNT];

        SDL_Texture *bg_box;
        SDL_Texture *bg_space;
        SDL_Texture *bg_yard;

        SDL_Texture *popup_paper;
        SDL_Texture *popup_credits;

        SDL_Texture *icon_settings;
        SDL_Texture *icon_vault;

        Mix_Music *title_music;

        Mix_Chunk *sfx_rain;
} AssetManager;

void asset_manager_init(AssetManager *asset_manager, SDL_Renderer *renderer);

void asset_manager_destroy(AssetManager *asset_manager);