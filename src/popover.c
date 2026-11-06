#include "popover.h"

#define POPOVER_MARGIN 10.0
#define POPOVER_POINTER_THICKNESS 5.0
#define POPOVER_POINTER_THICKNESS_HALF (POPOVER_POINTER_THICKNESS / 2.0)
#define POPOVER_POINTER_SIZE 20.0

void popover_draw_background(SDL_Renderer *renderer, Vec2 pos, Vec2 size,
                             enum PopoverPosition position) {
        SDL_Vertex vertices[3 + 4];

        float hw = size.x / 2.0 + POPOVER_MARGIN;

        vertices[0].position = (SDL_FPoint){pos.x, pos.y};
        vertices[1].position =
            (SDL_FPoint){pos.x - POPOVER_POINTER_THICKNESS_HALF,
                         pos.y - POPOVER_POINTER_SIZE - POPOVER_MARGIN};
        vertices[2].position = (SDL_FPoint){
            pos.x - hw, pos.y - POPOVER_POINTER_SIZE - POPOVER_MARGIN};
        vertices[3].position =
            (SDL_FPoint){pos.x - hw, pos.y - POPOVER_POINTER_SIZE - size.y -
                                         POPOVER_MARGIN - POPOVER_MARGIN};
        vertices[4].position =
            (SDL_FPoint){pos.x + hw, pos.y - POPOVER_POINTER_SIZE - size.y -
                                         POPOVER_MARGIN - POPOVER_MARGIN};
        vertices[5].position = (SDL_FPoint){
            pos.x + hw, pos.y - POPOVER_POINTER_SIZE - POPOVER_MARGIN};
        vertices[6].position =
            (SDL_FPoint){pos.x + POPOVER_POINTER_THICKNESS_HALF,
                         pos.y - POPOVER_POINTER_SIZE - POPOVER_MARGIN};

        for (int i = 0; i < 7; i++) {
                vertices[i].color = (SDL_Color){255, 255, 255, 255};
        }

        int indices[9] = {0, 1, 6, 2, 3, 5, 3, 4, 5};

        SDL_RenderGeometry(renderer, NULL, vertices, 7, indices, 9);
}

SDL_FRect popover_get_bounds(Vec2 pos, Vec2 size,
                             enum PopoverPosition position) {
        float hw = size.x / 2.0;

        return (SDL_FRect){
            pos.x - hw, pos.y - POPOVER_POINTER_SIZE - size.y - POPOVER_MARGIN,
            size.x, size.y + POPOVER_POINTER_SIZE + POPOVER_MARGIN};
}