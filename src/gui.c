#include "gui.h"
#include "dev.h"
#include "event.h"
#include "paper.h"
#include "sidebar.h"
#include "vault.h"

#include <SDL2/SDL_image.h>
#include <string.h>

#define HUD_MARGIN 16.0
#define HUD_ICON_SIZE 48.0
#define HUD_ICON_GAP 16.0

/* App GUI stuff */
bool app_init(App *app, const char *title, int width, int height) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
                return false;
        }

        app->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED, width, height,
                                       SDL_WINDOW_SHOWN);
        if (!app->window) {
                SDL_Quit();
                return false;
        }

        app->renderer = SDL_CreateRenderer(app->window, -1,
                                           SDL_RENDERER_ACCELERATED |
                                               SDL_RENDERER_PRESENTVSYNC);
        if (!app->renderer) {
                SDL_DestroyWindow(app->window);
                SDL_Quit();
                return false;
        }

        font_init(&app->font_atlas, app->renderer,
                  "assets/Roboto_Mono/static/RobotoMono-Regular.ttf", 16);
        gui_console_init(&app->dev_console, &app->font_atlas);

        app->running = true;
        return true;
}

/* Probably should move this to the settings file when that exists */
static bool settings_icon_render(SDL_Renderer *renderer, const World *world) {
        if (!world->settings_icon) {
                return false;
        }

        SDL_FRect icon = {(float)world->width - HUD_MARGIN - HUD_ICON_SIZE,
                          HUD_MARGIN, HUD_ICON_SIZE, HUD_ICON_SIZE};
        SDL_RenderCopyF(renderer, world->settings_icon, NULL, &icon);
        return true;
}

void app_run(App *app, World *world) {
        double frequency = (double)SDL_GetPerformanceFrequency();
        uint64_t last_counter = SDL_GetPerformanceCounter();

        while (app->running) {
                /* delta time compute */
                uint64_t current_counter = SDL_GetPerformanceCounter();
                float dt =
                    (float)(current_counter - last_counter) / (float)frequency;
                last_counter = current_counter;

                /* event processing */
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                        /* handle_event will not draw anything using the
                        renderer */
                        handle_event(world, app->renderer, &event,
                                     &app->running);
                        gui_console_handle_event(&app->dev_console, world,
                                                 &event, &app->running);
                }

                /* update world */
                if (!world->dev_command_mode) {
                        world_update(world, dt);
                }

                /* render pipe */
                SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
                SDL_RenderClear(app->renderer);

                world_render(world, app->renderer);
                double vault_right_offset = HUD_MARGIN;
                if (settings_icon_render(app->renderer, world)) {
                        vault_right_offset += HUD_ICON_SIZE + HUD_ICON_GAP;
                }
                vault_render(&world->vault, app->renderer, &app->font_atlas,
                             world->width, vault_right_offset);

                if (world->dev_command_mode) {
                        gui_console_render(&app->dev_console, app->renderer);
                }

                if (world->sidebar_shown) {
                        draw_sidebar(app->renderer, world, &app->font_atlas);
                }

                if (world->popup_open) {
                        paper_popup_render(
                            app->renderer, &app->font_atlas, world->popup_text,
                            world->paper_popup, world->width, world->height);
                }

                SDL_RenderPresent(app->renderer);
        }
}

void app_destroy(App *app) {
        font_destroy(&app->font_atlas);
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);
        SDL_Quit();
}

/* Developer Console Stuff */
void gui_console_init(DevConsole *console, FontAtlas *font_atlas) {
        memset(console->command_buffer, 0, sizeof(console->command_buffer));
        memset(console->output_buffer, 0, sizeof(console->output_buffer));
        console->font_atlas = font_atlas;
        console->was_mode_active = false;
}

void gui_console_handle_event(DevConsole *console, World *world,
                              SDL_Event *event, bool *running) {
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
        char *cb = console->command_buffer;
        char *ob = console->output_buffer;
        const char *text = event->text.text;

        switch (event->type) {
        case SDL_QUIT:
                *running = false;
                world->dev_command_mode = false;
                break;
        case SDL_KEYDOWN:
                if (key == SDLK_ESCAPE) {
                        world->dev_command_mode = false;
                } else if (key == SDLK_RETURN) {
                        run_dev_command(world, cb, ob, MAX_DEV_COMMAND_OUTPUT);
                        memset(cb, 0, MAX_DEV_COMMAND_LENGTH);
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
        font_draw(console->font_atlas, renderer, console->command_buffer, 10.0f,
                  10.0f + fh, white);
        font_draw(console->font_atlas, renderer, console->output_buffer, 10.0f,
                  10.0f + fh * 2, white);
}
