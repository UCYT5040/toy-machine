#pragma once

#include "scene.h"

void play_scene_enter(void *state_data, App *app);
void play_scene_exit(void *state_data, App *app);
void play_scene_handle_event(void *state_data, App *app, SDL_Event *event);
void play_scene_update(void *state_data, App *app, double dt);
void play_scene_render(void *state_data, App *app, SDL_Renderer *renderer);

extern Scene play_scene;