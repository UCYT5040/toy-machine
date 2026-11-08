#include "scenes/play.h"
#include "app.h"
#include "world.h"
#include "world_event.h"

#include "popup.h"
#include "sidebar.h"
#include "vault.h"

#define HUD_MARGIN 16.0
#define HUD_ICON_SIZE 48.0
#define HUD_ICON_GAP 16.0

/* Probably should move this to the settings file when that exists */
static bool settings_icon_render(App *app, const World *world) {
        if (!app->asset_manager.icon_settings) {
                return false;
        }

        SDL_FRect icon = {(float)world->width - HUD_MARGIN - HUD_ICON_SIZE,
                          HUD_MARGIN, HUD_ICON_SIZE, HUD_ICON_SIZE};
        SDL_RenderCopyF(app->renderer, app->asset_manager.icon_settings, NULL,
                        &icon);
        return true;
}

void play_scene_enter(void *state_data, App *app) {
        (void)state_data;

        app->scene->state_data = calloc(1, sizeof(World));

        world_init((World *)app->scene->state_data, app->renderer,
                   SCENE_TYPE_PLAY);
}

void play_scene_exit(void *state_data, App *app) {
        free(state_data);
        app->scene->state_data = NULL;
}

void play_scene_handle_event(void *state_data, App *app, SDL_Event *event) {
        world_handle_event(app, (World *)state_data, app->renderer, event,
                           &app->running);
};

void play_scene_update(void *state_data, App *app, double dt) {
        world_update((World *)state_data, dt);
}

void play_scene_render(void *state_data, App *app, SDL_Renderer *renderer) {
        world_render(app, (World *)state_data, renderer);

        // TODO: Consider moving to, e.g., world_render_hud
        World *world = (World *)state_data;

        double vault_right_offset = HUD_MARGIN;
        if (settings_icon_render(app, world)) {
                vault_right_offset += HUD_ICON_SIZE + HUD_ICON_GAP;
        }
        vault_render(&world->vault, app->renderer, &app->font_atlas,
                     world->width, vault_right_offset);

        if (world->sidebar_shown) {
                draw_sidebar(app, world, &app->font_atlas);
        }

        if (world->popup_open) {
                popup_render(app, world->popup_text, world, world->popup_type,
                             world->width, world->height);
        }
}

Scene play_scene = {.type = SCENE_TYPE_PLAY,
                    .state_type = SCENE_STATE_TYPE_WORLD,
                    .state_data = NULL,
                    .enter = play_scene_enter,
                    .exit = play_scene_exit,
                    .handle_event = play_scene_handle_event,
                    .update = play_scene_update,
                    .render = play_scene_render};