#include "dev.h"
#include "event.h"
#include "font.h"
#include "world.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define MAX_DEV_COMMAND_LENGTH 256
#define MAX_DEV_COMMAND_OUTPUT 256

int main(void) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
                return 1;

        SDL_Window *window = SDL_CreateWindow(
            "Toy Machine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960,
            600, SDL_WINDOW_SHOWN);

        SDL_Renderer *renderer = SDL_CreateRenderer(
            window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        bool running = true;

        double frequency = SDL_GetPerformanceFrequency();
        uint64_t last_counter = SDL_GetPerformanceCounter();
        double dt = 0.0;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        World world;

        world_init(&world, renderer);

        FontAtlas font_atlas;

        font_init(&font_atlas, renderer,
                  "assets/Roboto_Mono/static/RobotoMono-Regular.ttf", 16);

        char dev_command_output_buffer[256] = {0};

        while (running) {
                uint64_t current_counter = SDL_GetPerformanceCounter();
                dt = (double)(current_counter - last_counter) /
                     (double)frequency;
                if (dt > 0.05)
                        dt = 0.05; // In case of a freeze
                last_counter = current_counter;

                SDL_Event event;

                while (SDL_PollEvent(&event)) {
                        handle_event(&world, &event, &running);
                }

                if (world.dev_command_mode) {
                        char dev_command_buffer[MAX_DEV_COMMAND_LENGTH] = {0};
                        SDL_StartTextInput();
                        while (world.dev_command_mode) {
                                while (SDL_PollEvent(&event)) {
                                        switch (event.type) {
                                        case SDL_QUIT:
                                                running = false;
                                                world.dev_command_mode = false;
                                                break;
                                        case SDL_KEYDOWN:
                                                if (event.key.keysym.sym ==
                                                        SDLK_ESCAPE ||
                                                    event.key.keysym.sym ==
                                                        SDLK_F1) {
                                                        world.dev_command_mode =
                                                            false;
                                                } else if (event.key.keysym
                                                               .sym ==
                                                           SDLK_RETURN) {
                                                        run_dev_command(
                                                            &world,
                                                            dev_command_buffer,
                                                            dev_command_output_buffer,
                                                            MAX_DEV_COMMAND_OUTPUT);
                                                } else if (
                                                    event.key.keysym.sym ==
                                                        SDLK_BACKSPACE &&
                                                    strlen(dev_command_buffer) >
                                                        0) {
                                                        dev_command_buffer
                                                            [strlen(
                                                                 dev_command_buffer) -
                                                             1] = '\0';
                                                }
                                                break;
                                        case SDL_TEXTINPUT:
                                                if (strlen(dev_command_buffer) +
                                                        strlen(
                                                            event.text.text) <
                                                    MAX_DEV_COMMAND_LENGTH) {
                                                        strcat(
                                                            dev_command_buffer,
                                                            event.text.text);
                                                }
                                                break;
                                        }
                                }

                                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                                SDL_RenderClear(renderer);

                                font_draw(&font_atlas, renderer, "> \\", 10.0,
                                          10.0,
                                          (SDL_Color){255, 255, 255, 255});

                                font_draw(&font_atlas, renderer,
                                          dev_command_buffer, 10.0,
                                          10.0 + font_atlas.line_height,
                                          (SDL_Color){255, 255, 255, 255});

                                font_draw(&font_atlas, renderer,
                                          dev_command_output_buffer, 10.0,
                                          10.0 + font_atlas.line_height * 2,
                                          (SDL_Color){255, 255, 255, 255});

                                SDL_RenderPresent(renderer);
                        }
                        last_counter = SDL_GetPerformanceCounter();
                        dt = 0.0;
                }

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);

                world_update(&world, dt);

                world_render(&world, renderer);

                SDL_RenderPresent(renderer);
        }

        font_destroy(&font_atlas);

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
}