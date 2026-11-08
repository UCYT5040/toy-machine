#include "paper.h"

/* All these constants are based on SVG viewport 800x600 */
#define PAPER_SAFE_X (116.0f / 800.0f)
#define PAPER_SAFE_Y (180.0f / 600.0f)
#define PAPER_SAFE_W (530.0f / 800.0f)

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
        float safe_area_x = popup_rect.x + PAPER_SAFE_X * rect_w;
        float safe_area_y = popup_rect.y + PAPER_SAFE_Y * rect_h;
        /*float max_text_width = PAPER_SAFE_W * rect_w; */

        // TODO: Wrap!
        font_draw(font, renderer, text, safe_area_x, safe_area_y,
                  (SDL_Color){0, 0, 0, 255});
}