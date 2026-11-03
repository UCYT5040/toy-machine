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

        double size = (double)world->width / (EMOJI_PICKER_COLUMNS + 1);
        double screen_padding =
            (world->width - (EMOJI_PICKER_COLUMNS * size)) / 2.0;
        double actual_size = size * 0.8;
        double padding = (size - actual_size) / 2.0;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
        SDL_Rect picker_rect = {screen_padding, 0,
                                world->width - 2 * screen_padding,
                                world->height};
        SDL_RenderFillRect(renderer, &picker_rect);

        int unlocked_emoji_count = 0;

        for (int i = 1; i < EMOJI_COUNT; i++) {
                // EmojiDefinition *definition = &EMOJI_DEFINITIONS[i];
                SDL_Texture *texture = world->textures[i];

                if (!EMOJI_DEFINITIONS[i].unlocked || !texture) {
                        continue;
                }

                unlocked_emoji_count++;

                int row = (unlocked_emoji_count - 1) / EMOJI_PICKER_COLUMNS;
                int col = (unlocked_emoji_count - 1) % EMOJI_PICKER_COLUMNS;

                SDL_FRect dest_rect = {col * size + padding + screen_padding,
                                       row * size + padding, actual_size,
                                       actual_size};

                SDL_RenderCopyF(renderer, texture, NULL, &dest_rect);
        }
}

void emoji_picker_handle_mouse_down(World *world) {
        // Assume emoji picker is open

        double size = (double)world->width / (EMOJI_PICKER_COLUMNS + 1);
        double screen_padding =
            (world->width - (EMOJI_PICKER_COLUMNS * size)) / 2.0;

        int unlocked_emoji_count = 0;

        double rel_x = world->mouse_pos.x - screen_padding;
        double rel_y = world->mouse_pos.y;

        if (rel_x < 0.0 || rel_y < 0.0)
                return;

        int col = (int)(rel_x / size);
        int row = (int)(rel_y / size);

        if (col < 0 || col >= EMOJI_PICKER_COLUMNS) {
                return;
        }

        int target_emoji_index = row * EMOJI_PICKER_COLUMNS + col;

        for (int i = 1; i < EMOJI_COUNT; i++) {
                if (!EMOJI_DEFINITIONS[i].unlocked || !world->textures[i]) {
                        continue;
                }

                unlocked_emoji_count++;

                if (unlocked_emoji_count - 1 == target_emoji_index) {
                        // Spawn the emoji
                        Entity *new_entity =
                            entity_spawn(world, (EmojiType)i, world->mouse_pos);
                        if (new_entity) {
                                world->dragged_entity_index = new_entity->index;
                                world->drag_local_offset = (Vec2){0.0, 0.0};
                        }

                        world->emoji_picker_open = false;
                        break;
                }
        }
}