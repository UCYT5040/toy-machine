#include "scenes/title.h"
#include "app.h"
#include "emoji_registry.h"
#include "scenes/play.h"
#include "world.h"
#include "world_event.h"

#include "popup.h"
#include "sidebar.h"
#include "vault.h"

#include <SDL2/SDL_mixer.h>

#define HUD_MARGIN 16.0
#define HUD_ICON_SIZE 48.0
#define HUD_ICON_GAP 16.0

static void spawn_rain_cloud(World *world, float x) {
        float y = (float)(rand() % (world->height / 3) + 50);

        Entity *cloud = entity_spawn(world, EMOJI_RAIN_CLOUD, (Vec2){x, y});

        cloud->scale += (float)(rand() % 40 - 20) / 100.0f;
        cloud->angle += (float)(rand() % 40 - 20) / 100.0f;
}

#define NUM_RAIN_CLOUDS 3

void title_scene_exit_intro(World *world, App *app) {
        world->exit_timer = 0.0f;
        world->exiting = true;
}

void title_scene_enter_intro(World *world, App *app) {
        Mix_FadeOutMusic(500);

        Mix_FadeInMusic(app->asset_manager.intro_music, 1, 500);

        // Add a parachute & package falling from the top of the screen
        float x = (float)(rand() % (world->width - 200) + 100);
        Entity *parachute =
            entity_spawn(world, EMOJI_PARACHUTE, (Vec2){x, -150.0f});
        Entity *package =
            entity_spawn(world, EMOJI_PACKAGE, (Vec2){x, -100.0f});
        parachute->tags |= TAG_BOUNDARY_IMMUNE;
        package->tags |= TAG_BOUNDARY_IMMUNE;
        parachute->pinned_target_index = package->index;
        parachute->pinned_offset =
            (Vec2){0.0f, -package->definition->radius * package->scale};

        app->play_emoji_click_function = title_scene_exit_intro;
}

void title_scene_enter(void *state_data, App *app) {
        (void)state_data;

        app->scene->state_data = calloc(1, sizeof(World));

        World *world = (World *)app->scene->state_data;

        app->play_emoji_click_function = title_scene_enter_intro;

        world_init(world, app->renderer, SCENE_TYPE_TITLE);

        // Set title to be raining already
        world->rain_intensity = 1.0f;

        for (int i = 0; i < NUM_RAIN_CLOUDS; i++) {
                float x =
                    world->width * ((float)(i + 1) / (NUM_RAIN_CLOUDS + 2));
                x += (float)(rand() % 100 - 50); // Random offset
                spawn_rain_cloud(world, x);
        }

        entity_spawn(world, EMOJI_PLAY,
                     (Vec2){world->width / 2.0, world->height / 2.0});

        Mix_FadeInMusic(app->asset_manager.title_music, -1, 2000);
}

void title_scene_exit(void *state_data, App *app) {
        free(state_data);
        app->scene->state_data = NULL;

        Mix_FadeOutMusic(500);
}

void title_scene_handle_event(void *state_data, App *app, SDL_Event *event) {
        world_handle_event(app, (World *)state_data, app->renderer, event,
                           &app->running);
};

void title_scene_update(void *state_data, App *app, double dt) {
        world_update(app, (World *)state_data, dt);

        World *world = (World *)state_data;

        if (world->exiting) {
                world->exit_timer += (float)dt;
                if (world->exit_timer >= 1.0f) {
                        app_set_scene(app, &play_scene);
                }
        }
}

void title_scene_render(void *state_data, App *app, SDL_Renderer *renderer) {
        World *world = (World *)state_data;
        SDL_Texture *dummy_texture;
        if (world->exiting) {
                dummy_texture = SDL_CreateTexture(
                    renderer, SDL_PIXELFORMAT_RGBA8888,
                    SDL_TEXTUREACCESS_TARGET, world->width, world->height);
                SDL_SetRenderTarget(renderer, dummy_texture);
                SDL_RenderClear(renderer);
        }

        world_render(app, world, renderer);

        // TODO: Consider moving to, e.g., world_render_hud

        if (world->exiting) {
                SDL_SetRenderTarget(renderer, NULL);

                float zoom =
                    1.0f + 9.0f * (world->exit_timer * world->exit_timer);
                float view_w = (float)world->width / zoom;
                float view_h = (float)world->height / zoom;

                Vec2 package_pos = {world->width / 2.0f, world->height / 2.0f};

                for (int i = 0; i < MAX_ENTITIES; i++) {
                        Entity *entity = &world->entities[i];
                        if (entity->active && entity->type == EMOJI_PACKAGE) {
                                package_pos = entity->pos;
                                break;
                        }
                }

                SDL_Rect src_rect = {
                    (int)fmaxf(0.0f, fminf(package_pos.x - view_w / 2.0f,
                                           world->width - view_w)),
                    (int)fmaxf(0.0f, fminf(package_pos.y - view_h / 2.0f,
                                           world->height - view_h)),
                    (int)view_w, (int)view_h};

                SDL_RenderCopy(renderer, dummy_texture, &src_rect, NULL);
                SDL_DestroyTexture(dummy_texture);
        }

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