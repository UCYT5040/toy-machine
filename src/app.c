#include "app.h"
#include "dev.h"

#include <SDL2/SDL_image.h>

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
        dev_console_init(&app->dev_console, &app->font_atlas);

        app->running = true;

        // Call the scene enter function (if it exists)
        if (app->scene->enter) {
                app->scene->enter(app->scene->state_data, app);
        }

        return true;
}

void app_run(App *app) {
        double frequency = (double)SDL_GetPerformanceFrequency();
        uint64_t last_counter = SDL_GetPerformanceCounter();

        while (app->running) {

                if (app->next_scene != NULL) {
                        if (app->scene->exit) {
                                app->scene->exit(app->scene->state_data, app);
                        }

                        app->scene = app->next_scene;
                        app->next_scene = NULL;

                        if (app->scene->enter) {
                                app->scene->enter(app->scene->state_data, app);
                        }
                }

                /* delta time compute */
                uint64_t current_counter = SDL_GetPerformanceCounter();
                float dt =
                    (float)(current_counter - last_counter) / (float)frequency;
                last_counter = current_counter;

                /* event processing */
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT) {
                                app->running = false;
                        }

                        if (!dev_console_handle_event(app, &event,
                                                      &app->running)) {
                                // Event was not consumed by the dev console
                                if (app->scene->handle_event) {
                                        app->scene->handle_event(
                                            app->scene->state_data, app,
                                            &event);
                                }
                        }
                }

                /* update world */
                if (app->scene->update) {
                        app->scene->update(app->scene->state_data, app, dt);
                }

                /* render pipe */
                SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
                SDL_RenderClear(app->renderer);

                if (app->scene->render) {
                        app->scene->render(app->scene->state_data, app,
                                           app->renderer);
                }

                if (app->dev_console.is_open) {
                        dev_console_render(&app->dev_console, app->renderer);
                }

                SDL_RenderPresent(app->renderer);
        }
}

void app_destroy(App *app) {
        // Call the scene exit function (if it exists)
        if (app->scene->exit) {
                app->scene->exit(app->scene->state_data, app);
        }

        font_destroy(&app->font_atlas);
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);
        SDL_Quit();
}

void app_set_scene(App *app, Scene *scene) { app->next_scene = scene; }
