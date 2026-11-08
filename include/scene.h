#pragma once

#include <SDL2/SDL.h>

/* Forward declation of App to prevent circular dependency */
typedef struct App App;

typedef enum SceneType { SCENE_TYPE_TITLE, SCENE_TYPE_PLAY } SceneType;

typedef enum SceneStateType {
        SCENE_STATE_TYPE_NONE,
        SCENE_STATE_TYPE_WORLD
} SceneStateType;

typedef struct Scene {
        SceneType type;
        SceneStateType state_type;
        void *state_data;
        void (*enter)(void *state_data, App *app);
        void (*exit)(void *state_data, App *app);
        void (*handle_event)(void *state_data, App *app, SDL_Event *event);
        void (*update)(void *state_data, App *app, double dt);
        void (*render)(void *state_data, App *app, SDL_Renderer *renderer);
} Scene;
