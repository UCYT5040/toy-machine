#include "gui.h"
#include "dev.h"
#include "event.h"
#include <SDL/SDL2_image.h>
#include <string.h>

void gui_console_init(DevConsole *console, FontAtlas *font_atlas) {
        memset(console->command_buffer, 0, sizeof(console->command_buffer));
        memset(console->output_buffer, 0, sizeof(console->output_buffer));\
        console->font_atlas = font_atlas;
        console->was_mode_active = false;
}

void gui_console_handle_event(DevConsole *console, World *world, SDL_Event *event, bool *running) {
        if (world->dev_command_mode && !console->was_mode_active) {
                SDL_StartTextInput();
                console->was_mode_active = true;
        } else if (!world->dev_command_mode && console->was_mode_active) {
                SDL_StopTextInput();
                console->was_mode_active = false;
        }

        if (!world->dev_command_mode) {
                return;
        }

        /* These values are repeated quite a bit and 
        are pretty long so I assigned them to a variable */
        SDL_Keycode key = event->key.keysym.sym;
        cb = console->command_buffer;
        ob = console->output_buffer;
        text = event->text.text     

        switch (event->type) {
        case SDL_QUIT:
                *running = false;
                world->dev_command_mode = false;
                break;
        case SDL_KEYDOWN:
                if (key == SDLK_ESCAPE || key == SDLK_F1) {
                        world->dev_command_mode = false;
                } else if (key == SDLK_RETURN) {
                        run_dev_command(world, cb, ob, MAX_DEV_COMMAND_OUTPUT);
                        memset(cb, 0, sizeof(cb));
                } else if (key == SDLK_BACKSPACE) {
                        size_t len = strlen(cb);
                        if (len > 0) {
                                cb[len - 1] = '\0';
                        }
                }
                break;
        case SDL_TEXTINPUT:
                if (strlen(cb) + strlen(text) < MAX_DEV_COMMAND_LENGTH) {
                        strcat(cb, text);
                }
                break;
        }
}

void gui_console_render(DevConsole *console, SDL_Renderer *renderer) {
        SDL_Color white = {255, 255, 255, 255};
        float fh = console->font_atlas->line_height;
        font_draw(console->font_atlas, renderer, "> ", 10.0f, 10.0f, white);
        font_draw(console->font_atlas, renderer, console->command_buffer, 10.0f, 10.0f + fh, white);
        font_draw(console->font_atlas, renderer, console->output_buffer, 10.0f, 10.0f + fh * 2, white);
}

