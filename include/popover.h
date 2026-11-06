#include "vec2.h"
#include <SDL2/SDL.h>

enum PopoverPosition {
        POPOVER_POSITION_TOP
        // TODO: Add more positions
};

void popover_draw_background(SDL_Renderer *renderer, Vec2 pos, Vec2 size,
                             enum PopoverPosition position);

SDL_FRect popover_get_bounds(Vec2 pos, Vec2 size,
                             enum PopoverPosition position);