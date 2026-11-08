#pragma once

#include "dev.h"
#include "font.h"
#include "scene.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct App {
        SDL_Window *window;
        SDL_Renderer *renderer;
        FontAtlas font_atlas;
        Scene *scene;
        Scene *next_scene;
        DevConsole dev_console;
        bool running;
} App;

bool app_init(App *app, const char *title, int width, int height);
void app_run(App *app);
void app_destroy(App *app);

void app_set_scene(App *app, Scene *scene);
