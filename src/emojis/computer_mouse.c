#include "emojis/computer_mouse.h"
#include <SDL2/SDL.h>

void emoji_computer_mouse_update(World *world, Entity *self, double dt) {
        (void)dt;
        
        if (world->dragged_entity_index != -1) {
                self->last_pos = self->pos;
                return;
        }

        SDL_Window *window = SDL_GetMouseFocus();
        if (window == NULL || 
            !(SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS)) {
                self->last_pos = self->pos;
                return;
        }

        int width, height;
        SDL_GetWindowSize(window, &width, &height);

        int x = (int)self->pos.x;
        int y = (int)self->pos.y;

        if (x < 0 || y < 0 || x >= width || y >= height) {
                self->last_pos = self->pos;
                return;
        }

        int last_x = (int)self->last_pos.x;
        int last_y = (int)self->last_pos.y;

        if (x == last_x && y == last_y) {
                return;
        }

        SDL_WarpMouseInWindow(window, x, y);
        self->last_pos = self->pos;
}