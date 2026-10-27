#include "event.h"
#include "font.h"
#include "world.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

#define MAX_DEV_COMMAND_LENGTH 256

int main(void) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
    return 1;

  SDL_Window *window =
      SDL_CreateWindow("Toy Machine", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 960, 600, SDL_WINDOW_SHOWN);

  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  bool running = true;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  World world = {0}; // TODO: Add init_world function

  FontAtlas font_atlas;

  font_init(&font_atlas, renderer,
            "assets/Roboto_Mono/static/RobotoMono-Regular.ttf", 16);

  while (running) {
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
            if (event.key.keysym.sym == SDLK_ESCAPE) {
              world.dev_command_mode = false;
            } else if (event.key.keysym.sym == SDLK_RETURN) {
              world.dev_command_mode = false;
              // TODO: Submit command
            } else if (event.key.keysym.sym == SDLK_BACKSPACE &&
                       strlen(dev_command_buffer) > 0) {
              dev_command_buffer[strlen(dev_command_buffer) - 1] = '\0';
            }
            break;
          case SDL_TEXTINPUT:
            if (strlen(dev_command_buffer) + strlen(event.text.text) <
                MAX_DEV_COMMAND_LENGTH) {
              strcat(dev_command_buffer, event.text.text);
            }
            break;
          }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        font_draw(&font_atlas, renderer, "> \\", 10.0f, 10.0f,
                  (SDL_Color){255, 255, 255, 255});

        font_draw(&font_atlas, renderer, dev_command_buffer, 10.0f,
                  10.0f + font_atlas.line_height,
                  (SDL_Color){255, 255, 255, 255});

        SDL_RenderPresent(renderer);
      }
    }

    SDL_RenderPresent(renderer);
  }

  font_destroy(&font_atlas);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}