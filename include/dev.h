#pragma once

#include <SDL2/SDL.h>

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "font.h"
#include "world.h"

#define MAX_DEV_COMMAND_LENGTH 256
#define MAX_DEV_COMMAND_OUTPUT 384

// Forward-declaration
typedef struct App App;

void run_dev_command(App *app, char *command, char *output_buffer,
                     size_t output_buffer_size);

void render_velocity_vector(SDL_Renderer *renderer, Entity *entity);

void render_hitbox(SDL_Renderer *renderer, Entity *entity);

typedef struct DevConsole {
        char command_buffer[MAX_DEV_COMMAND_LENGTH];
        char output_buffer[MAX_DEV_COMMAND_OUTPUT];
        FontAtlas *font_atlas;
        bool was_mode_active;
        bool is_open;
} DevConsole;

void dev_console_init(DevConsole *console, FontAtlas *font_atlas);
bool dev_console_handle_event(App *app, SDL_Event *event, bool *running);
void dev_console_render(DevConsole *console, SDL_Renderer *renderer);
