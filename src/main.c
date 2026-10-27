#include "event.h"
#include "world.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

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

  World world = {0};

  while (running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      handle_event(&world, &event, &running);
    }

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}