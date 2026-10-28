#include "event.h"

void handle_event(World *world, SDL_Event *event, bool *running) {
        switch (event->type) {
        case SDL_QUIT:
                *running = false;
                break;

        case SDL_MOUSEMOTION: {
                world->mouse_pos.x = (double)event->motion.x;
                world->mouse_pos.y = (double)event->motion.y;
                break;
        }

        case SDL_MOUSEBUTTONDOWN: {
                if (event->button.button == SDL_BUTTON_LEFT) {
                        world->mouse_down =
                            true; // TODO: Consider adding a mouse button state
                }
                break;
        }

        case SDL_MOUSEBUTTONUP: {
                if (event->button.button == SDL_BUTTON_LEFT) {
                        world->mouse_down = false;
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