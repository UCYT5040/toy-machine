#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#include "emoji_registry.h"
#include "entities.h"
#include "types.h"
#include "vault.h"

typedef struct World {
        Entity entities[MAX_ENTITIES];
        SDL_Texture *textures[EMOJI_COUNT];

        SDL_Texture *bg_box;
        SDL_Texture *bg_space;

        SDL_Texture *paper_popup;

        bool popup_open;
        const char *popup_text;

        Vault vault;

        int width, height;

        Vec2 gravity; // in px/s^2
        bool space_mode;
        double space_mode_transition;

        bool emoji_unlocks[EMOJI_COUNT - 1]; // Exclude EMOJI_NONE

        Vec2 mouse_pos;
        bool mouse_down;
        uint32_t last_mouse_move;
        Vec2 last_mouse_speed;
        Vec2 mouse_speed;
        Vec2 mouse_accel;
        int pressed_entity_index; // -1 if no entity is being dragged
        Vec2 press_local_offset; // The point where this entity is being dragged
                                 // from, relative to it's center
        Vec2 press_start_pos;    // The position of the entity when it was first
                                 // pressed
        bool is_dragging;

        double speed; // Currently used only for dev commands, but could
                      // be an effect later on (this is a multiplier for dt)

        int save_slot;
        bool quit_requested; /* Currently only triggered by dev console exit
                                commands only*/

        bool dev_command_mode; // If true, the dev command window is open (game
                               // is paused)
        bool dev_show_hitboxes;
        bool dev_show_velocity_vectors;
        // TODO: Add more developer/debugging features

        bool sidebar_shown;
        double sidebar_scroll_px;
} World;

void world_init(World *world, SDL_Renderer *renderer);
void world_update(World *world, double dt);
void world_render(World *world, SDL_Renderer *renderer);

int world_get_entity_at(
    World *world, Vec2 pos); // If multiple entities overlap, selects the entity
                             // with a center nearest to the position.

void world_handle_mouse_down(World *world);
void world_handle_mouse_up(World *world);
