#include "popup.h"
#include "app.h"
#include "world.h"

/* All these constants are based on SVG viewport 800x600 */
#define PAPER_SAFE_X (116.0f / 800.0f)
#define PAPER_SAFE_Y (180.0f / 600.0f)
#define PAPER_SAFE_W (530.0f / 800.0f)

void popup_render(App *app, const char *text, World *world,
                  PopupType popup_type, int renderer_w, int renderer_h) {
        SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_BLEND);

        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 128);

        SDL_RenderFillRect(app->renderer, NULL);

        SDL_Texture *popup_texture = NULL;

        switch (popup_type) {
        case POPUP_TYPE_PAPER:
                popup_texture = app->asset_manager.popup_paper;
                break;
        case POPUP_TYPE_CREDITS:
                popup_texture = app->asset_manager.popup_credits;
                break;
        }

        int tex_width, tex_height = 0;
        SDL_QueryTexture(popup_texture, NULL, NULL, &tex_width, &tex_height);

        float aspect = (float)tex_width / (float)tex_height;

        float max_w = renderer_w * 0.8;
        float max_h = renderer_h * 0.6;

        float rect_w = max_w;
        float rect_h = rect_w / aspect;

        if (rect_h > max_h) {
                rect_h = max_h;
                rect_w = rect_h * aspect;
        }

        SDL_FRect popup_rect = {.x = ((renderer_w - rect_w) / 2.0),
                                .y = ((renderer_h - rect_h) / 2.0),
                                .w = rect_w,
                                .h = rect_h};

        SDL_RenderCopyF(app->renderer, popup_texture, NULL, &popup_rect);

        // Draw the text in the safe area of the paper
        float safe_area_x = popup_rect.x + PAPER_SAFE_X * rect_w;
        float safe_area_y = popup_rect.y + PAPER_SAFE_Y * rect_h;
        /*float max_text_width = PAPER_SAFE_W * rect_w; */

        // TODO: Wrap!
        font_draw(&app->font_atlas, app->renderer, text, safe_area_x,
                  safe_area_y, (SDL_Color){0, 0, 0, 255});
}