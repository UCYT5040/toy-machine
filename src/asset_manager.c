#include "asset_manager.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_mixer.h>

void asset_manager_init(AssetManager *asset_manager, SDL_Renderer *renderer) {
        for (int i = 1; i < EMOJI_COUNT; i++) {
                EmojiDefinition *definition = &EMOJI_DEFINITIONS[i];

                definition->index = i;

                asset_manager->emojis[i] = NULL; // Initialize texture to NULL

                if (definition->svg_path) {
                        SDL_Surface *surface = IMG_Load(definition->svg_path);

                        if (surface) {
                                asset_manager->emojis[i] =
                                    SDL_CreateTextureFromSurface(renderer,
                                                                 surface);
                        }

                        SDL_FreeSurface(surface);
                }
        }

        SDL_Surface *bg_box_surface = IMG_Load("assets/box.svg");
        SDL_Surface *bg_space_surface = IMG_Load("assets/space.svg");
        SDL_Surface *bg_yard_surface = IMG_Load("assets/yard.svg");

        SDL_Surface *paper_popup_surface = IMG_Load("assets/paper_popup.svg");
        SDL_Surface *credits_popup_surface =
            IMG_Load("assets/credits_popup.svg");

        if (!bg_box_surface || !bg_space_surface) {
                return;
        }
        asset_manager->bg_box =
            SDL_CreateTextureFromSurface(renderer, bg_box_surface);
        asset_manager->bg_space =
            SDL_CreateTextureFromSurface(renderer, bg_space_surface);
        asset_manager->bg_yard =
            SDL_CreateTextureFromSurface(renderer, bg_yard_surface);
        SDL_FreeSurface(bg_box_surface);
        SDL_FreeSurface(bg_space_surface);
        SDL_FreeSurface(bg_yard_surface);

        if (paper_popup_surface) {
                asset_manager->popup_paper =
                    SDL_CreateTextureFromSurface(renderer, paper_popup_surface);
        }
        SDL_FreeSurface(paper_popup_surface);

        if (credits_popup_surface) {
                asset_manager->popup_credits = SDL_CreateTextureFromSurface(
                    renderer, credits_popup_surface);
        }
        SDL_FreeSurface(credits_popup_surface);

        SDL_Surface *settings_icon_surface =
            IMG_Load("assets/settings_icon.png");
        if (settings_icon_surface) {
                asset_manager->icon_settings = SDL_CreateTextureFromSurface(
                    renderer, settings_icon_surface);
        }
        SDL_FreeSurface(settings_icon_surface);

        SDL_SetTextureColorMod(asset_manager->bg_box, 190, 190,
                               190); // Dim the box background

        asset_manager->title_music =
            Mix_LoadMUS("assets/music/corona_radiata.ogg");
        asset_manager->intro_music = Mix_LoadMUS("assets/music/demon_seed.ogg");

        asset_manager->sfx_rain = Mix_LoadWAV("assets/sfx/rain.ogg");
}

void asset_manager_destroy(AssetManager *asset_manager) {
        for (int i = 1; i < EMOJI_COUNT; i++) {
                if (asset_manager->emojis[i]) {
                        SDL_DestroyTexture(asset_manager->emojis[i]);
                }
        }

        SDL_DestroyTexture(asset_manager->bg_box);
        SDL_DestroyTexture(asset_manager->bg_space);
        SDL_DestroyTexture(asset_manager->bg_yard);

        SDL_DestroyTexture(asset_manager->popup_paper);
        SDL_DestroyTexture(asset_manager->popup_credits);

        SDL_DestroyTexture(asset_manager->icon_settings);
        SDL_DestroyTexture(asset_manager->icon_vault);
}