#include <SDL2/SDL.h>
#include <stdint.h>

#include "event.h"

void handle_event(World *world, SDL_Event *event, bool *running) {
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
                        world->mouse_down =
                            true; // TODO: Consider adding a mouse button state
                        world_handle_mouse_down(world);
                }
                break;
        }

        case SDL_MOUSEBUTTONUP: {
                if (event->button.button == SDL_BUTTON_LEFT) {
                        world->mouse_down = false;
                        world_handle_mouse_up(world);
                }
                break;
        }

        case SDL_KEYDOWN: {
                if (event->key.keysym.sym == SDLK_F1) {
                        world->dev_command_mode = !world->dev_command_mode;
                }
        }
        }
}