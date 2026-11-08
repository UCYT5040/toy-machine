#include "scenes/title.h"
#include "app.h"
#include "world.h"
#include "world_event.h"

#include "popup.h"
#include "sidebar.h"
#include "vault.h"

#define HUD_MARGIN 16.0
#define HUD_ICON_SIZE 48.0
#define HUD_ICON_GAP 16.0

void title_scene_enter(void *state_data, App *app) {
        (void)state_data;

        app->scene->state_data = calloc(1, sizeof(World));

        World *world = (World *)app->scene->state_data;

        world_init(world, app->renderer);

        entity_spawn(world, EMOJI_PLAY,
                     (Vec2){world->width / 2.0, world->height / 2.0});
}

void title_scene_exit(void *state_data, App *app) {
        free(state_data);
        app->scene->state_data = NULL;
}

void title_scene_handle_event(void *state_data, App *app, SDL_Event *event) {
        world_handle_event(app, (World *)state_data, app->renderer, event,
                           &app->running);
};

void title_scene_update(void *state_data, App *app, double dt) {
        world_update((World *)state_data, dt);
}

void title_scene_render(void *state_data, App *app, SDL_Renderer *renderer) {
        world_render(app, (World *)state_data, renderer);

        // TODO: Consider moving to, e.g., world_render_hud
        World *world = (World *)state_data;

        if (world->popup_open) {
                popup_render(app, world->popup_text, world, world->popup_type,
                             world->width, world->height);
        }
}

Scene title_scene = {.type = SCENE_TYPE_TITLE,
                     .state_type = SCENE_STATE_TYPE_WORLD,
                     .state_data = NULL,
                     .enter = title_scene_enter,
                     .exit = title_scene_exit,
                     .handle_event = title_scene_handle_event,
                     .update = title_scene_update,
                     .render = title_scene_render};