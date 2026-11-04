#include "emoji_picker.h"
#include "emoji_registry.h"
#include "entities.h"

#include <SDL2/SDL.h>

/*
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
*/