#include "emoji_picker.h"
#include "emoji_registry.h"
#include "entities.h"

#include <SDL2/SDL.h>

#define EMOJI_PICKER_COLUMNS 8

void emoji_picker_toggle(World *world) {
        world->emoji_picker_open = !world->emoji_picker_open;
}

// void emoji_picker_update(World *world, double dt);

void emoji_picker_render(World *world, SDL_Renderer *renderer) {
        if (!world->emoji_picker_open)
                return;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
        SDL_Rect picker_rect = {0, 0, world->width, world->height};
        SDL_RenderFillRect(renderer, &picker_rect);

        double size = (double)world->width / (EMOJI_PICKER_COLUMNS + 1);
        double actual_size = size * 0.8;
        double padding = (size - actual_size) / 2.0;

        int unlocked_emoji_count = 0;

        for (int i = 1; i < EMOJI_COUNT; i++) {
                // EmojiDefinition *definition = &EMOJI_DEFINITIONS[i];
                SDL_Texture *texture = world->textures[i];

                if (!world->emoji_unlocks[i - 1] || !texture) {
                        continue;
                }

                unlocked_emoji_count++;

                int row = (unlocked_emoji_count - 1) / EMOJI_PICKER_COLUMNS;
                int col = (unlocked_emoji_count - 1) % EMOJI_PICKER_COLUMNS;

                SDL_FRect dest_rect = {col * size + padding,
                                       row * size + padding, actual_size,
                                       actual_size};

                SDL_RenderCopyF(renderer, texture, NULL, &dest_rect);
        }
}

void emoji_picker_handle_mouse_down(World *world) {
        // Assume emoji picker is open

        printf("Mouse down in emoji picker");

        double size = (double)world->width / (EMOJI_PICKER_COLUMNS + 1);

        int unlocked_emoji_count = 0;

        int col = (int)(world->mouse_pos.x / size);
        int row = (int)(world->mouse_pos.y / size);

        if (col < 0 || col >= EMOJI_PICKER_COLUMNS || row < 0) {
                return;
        }

        int target_emoji_index = row * EMOJI_PICKER_COLUMNS + col;

        for (int i = 1; i < EMOJI_COUNT; i++) {
                if (!world->emoji_unlocks[i - 1]) {
                        continue;
                }

                unlocked_emoji_count++;

                if (unlocked_emoji_count - 1 == target_emoji_index) {
                        // Spawn the emoji
                        Entity *new_entity =
                            entity_spawn(world, (EmojiType)i, world->mouse_pos);
                        if (new_entity) {
                                world->dragged_entity_index = new_entity->index;
                        }

                        world->emoji_picker_open = false;
                        break;
                }
        }
}