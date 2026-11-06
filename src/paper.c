#include "paper.h"

/* All these constants are based on SVG viewport 800x600 */
#define PAPER_CENTER_X 381
#define PAPER_CENTER_Y 288
#define PAPER_SAFE_WIDTH 646 - 116
#define PAPER_SAFE_HEIGHT 576 - 116

void paper_popup_render(SDL_Renderer *renderer, FontAtlas *font,
                        const char *text, SDL_Texture *popup_texture,
                        int renderer_w, int renderer_h) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);

        SDL_RenderFillRect(renderer, NULL);

        float rect_w = renderer_w * 0.8;
        float rect_h = renderer_h * 0.6;

        SDL_FRect popup_rect = {.x = ((renderer_w - rect_w) / 2.0),
                                .y = ((renderer_h - rect_h) / 2.0),
                                .w = rect_w,
                                .h = rect_h};

        SDL_RenderCopyF(renderer, popup_texture, NULL, &popup_rect);

        // Draw the text in the safe area of the paper
        float safe_area_x = popup_rect.x + (PAPER_SAFE_WIDTH / 800.0) * rect_w;
        float safe_area_y = popup_rect.y + (PAPER_SAFE_HEIGHT / 600.0) * rect_h;
        /*float safe_area_w = (PAPER_SAFE_WIDTH / 800.0) * rect_w;
        float safe_area_h = (PAPER_SAFE_HEIGHT / 600.0) * rect_h;*/

        // TODO: Wrap!
        font_draw(font, renderer, text, safe_area_x, safe_area_y,
                  (SDL_Color){0, 0, 0, 255});
}