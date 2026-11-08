#include "title.h"
#include "app.h"
#include "world.h"
#include "world_event.h"

void title_scene_enter(void *state_data, App *app) {
        (void)state_data;

        app->scene->state_data = calloc(1, sizeof(World));

        world_init((World *)app->scene->state_data, app->renderer);
}

void title_scene_exit(void *state_data, App *app) {
        free(state_data);
        app->scene->state_data = NULL;
}

void title_scene_handle_event(void *state_data, App *app, SDL_Event *event) {
        world_handle_event((World *)state_data, app->renderer, event,
                           &app->running);
};

void title_scene_update(void *state_data, App *app, double dt) {
        world_update((World *)state_data, dt);
}

void title_scene_render(void *state_data, App *app, SDL_Renderer *renderer) {
        world_render((World *)state_data, renderer);
}

Scene title_scene = {.type = SCENE_TYPE_TITLE,
                     .state_type = SCENE_STATE_TYPE_WORLD,
                     .state_data = NULL,
                     .enter = title_scene_enter,
                     .exit = title_scene_exit,
                     .handle_event = title_scene_handle_event,
                     .update = title_scene_update,
                     .render = title_scene_render};