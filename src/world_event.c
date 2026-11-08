#include <SDL2/SDL.h>
#include <stdint.h>

#include "save.h"
#include "sidebar.h"
#include "world_event.h"

// This will not draw anything using the renderer
void world_handle_event(App *app, World *world, SDL_Renderer *renderer,
                        SDL_Event *event, bool *running) {
        switch (event->type) {
        case SDL_QUIT:
                *running = false;
                break;

        case SDL_MOUSEMOTION: {
                uint32_t time = SDL_GetTicks();

                world->mouse_pos.x = (double)event->motion.x;
                world->mouse_pos.y = (double)event->motion.y;

                if (world->last_mouse_move != 0) {
                        double dt = (double)time / 1000.0 -
                                    (double)world->last_mouse_move / 1000.0;
                        if (dt > 0.0) {
                                world->mouse_speed.x =
                                    (double)event->motion.xrel / dt * 2.0;
                                world->mouse_speed.y =
                                    (double)event->motion.yrel / dt * 2.0;
                        }
                }
                world->last_mouse_move = time;

                break;
        }

        case SDL_MOUSEBUTTONDOWN: {
                if (event->button.button == SDL_BUTTON_LEFT) {
                        world->mouse_pos.x = (double)event->button.x;
                        world->mouse_pos.y = (double)event->button.y;
                        world->mouse_down =
                            true; // TODO: Consider adding a mouse button state
                        if (world->sidebar_shown &&
                            pos_in_sidebar(renderer, world->mouse_pos.x)) {
                                sidebar_handle_mouse_down(renderer, world);
                        } else if (world->popup_open) {
                                world->popup_open = false;
                        } else {
                                world_handle_mouse_down(world);
                        }
                }
                break;
        }

        case SDL_MOUSEBUTTONUP: {
                if (event->button.button == SDL_BUTTON_LEFT) {
                        world->mouse_down = false;
                        world_handle_mouse_up(app, world);
                } else if (event->button.button == SDL_BUTTON_MIDDLE) {
                        world->sidebar_shown = !world->sidebar_shown;
                }
                break;
        }

        case SDL_KEYDOWN: {
                if (event->key.keysym.sym == SDLK_F5) {
                        /* TODO: Some sort of animation or text to let the user
                         * know the game has saved/loaded */
                        char error[128];
                        if (!save(world, world->save_slot, error,
                                  sizeof error)) {
                                SDL_Log("Save failed: %s", error);
                        }
                } else if (event->key.keysym.sym == SDLK_F9) {
                        char error[128];
                        if (!load(world, world->save_slot, error,
                                  sizeof error)) {
                                SDL_Log("Load failed: %s", error);
                        }
                } else if (event->key.keysym.sym == SDLK_SPACE) {
                        world->sidebar_shown = !world->sidebar_shown;
                }
                break;
        }

        case SDL_MOUSEWHEEL: {
                if (world->sidebar_shown &&
                    pos_in_sidebar(renderer, world->mouse_pos.x)) {
                        sidebar_handle_scroll(renderer, world, event->wheel.y);
                }
        }
        }
}