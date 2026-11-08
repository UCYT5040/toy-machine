#pragma once

#include <SDL2/SDL.h>

/* Forward declation of App to prevent circular dependency */
typedef struct App App;

typedef struct Scene {
        void *state_data;
        void (*enter)(void *state_data, App *app);
        void (*exit)(void *state_data, App *app);
        void (*handle_event)(void *state_data, App *app, SDL_Event *event);
        void (*update)(void *state_data, App *app, double dt);
        void (*render)(void *state_data, App *app, SDL_Renderer *renderer);
} Scene;
