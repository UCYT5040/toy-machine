#pragma once

#include "scene.h"

void title_scene_enter(void *state_data, App *app);
void title_scene_exit(void *state_data, App *app);
void title_scene_handle_event(void *state_data, App *app, SDL_Event *event);
void title_scene_update(void *state_data, App *app, double dt);
void title_scene_render(void *state_data, App *app, SDL_Renderer *renderer);

extern Scene title_scene;