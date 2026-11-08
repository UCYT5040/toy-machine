#include "scenes/play.h"
#include "app.h"
#include "world.h"
#include "world_event.h"

#include "popup.h"
#include "settings.h"
#include "sidebar.h"
#include "vault.h"

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
        World *world = (World *)state_data;

        if (settings_handle_event(&world->settings, event, world->width,
                                  world->height, &world->popup_open)) {
                return;
        }

        world_handle_event(app, world, app->renderer, event, &app->running);
};

void play_scene_update(void *state_data, App *app, double dt) {
        World *world = (World *)state_data;

        if (!world->settings.open) {
                world_update(app, world, dt);
        }
}

void play_scene_render(void *state_data, App *app, SDL_Renderer *renderer) {
        world_render(app, (World *)state_data, renderer);

        // TODO: Consider moving to, e.g., world_render_hud
        World *world = (World *)state_data;

        double vault_right_offset = SETTINGS_ICON_MARGIN;
        if (settings_render_icon(app->renderer,
                                 app->asset_manager.icon_settings,
                                 world->width)) {
                vault_right_offset += SETTINGS_ICON_SIZE + SETTINGS_ICON_GAP;
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

        if (world->settings.open) {
                settings_render_menu(&world->settings, app->renderer,
                                     &app->font_atlas, world->width,
                                     world->height);
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