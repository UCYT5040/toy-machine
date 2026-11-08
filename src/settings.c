#include "settings.h"

#define SETTINGS_MENU_WIDTH 220.0f
#define SETTINGS_MENU_GAP 8.0f
#define SETTINGS_MENU_PADDING 12.0f
#define SETTINGS_MENU_ROW_HEIGHT 30.0f

static SDL_FRect settings_icon_rect(int window_width) {
        return (SDL_FRect){
            (float)window_width - SETTINGS_ICON_MARGIN - SETTINGS_ICON_SIZE,
            SETTINGS_ICON_MARGIN, SETTINGS_ICON_SIZE, SETTINGS_ICON_SIZE};
}

static SDL_FRect settings_menu_rect(int window_width, int window_height) {
        float height = SETTINGS_MENU_PADDING * 2.0f +
                       SETTINGS_MENU_ROW_HEIGHT * 3.0f;
        float x = (float)window_width - SETTINGS_ICON_MARGIN -
                  SETTINGS_MENU_WIDTH;
        float y = SETTINGS_ICON_MARGIN + SETTINGS_ICON_SIZE + SETTINGS_MENU_GAP;

        if (x < SETTINGS_ICON_MARGIN) {
                x = SETTINGS_ICON_MARGIN;
        }
        if (y + height > (float)window_height - SETTINGS_ICON_MARGIN) {
                y = (float)window_height - SETTINGS_ICON_MARGIN - height;
        }

        return (SDL_FRect){x, y, SETTINGS_MENU_WIDTH, height};
}

static SDL_FRect settings_close_rect(int window_width, int window_height) {
        SDL_FRect menu = settings_menu_rect(window_width, window_height);
        return (SDL_FRect){menu.x + SETTINGS_MENU_PADDING,
                           menu.y + menu.h - SETTINGS_MENU_PADDING -
                               SETTINGS_MENU_ROW_HEIGHT,
                           menu.w - SETTINGS_MENU_PADDING * 2.0f,
                           SETTINGS_MENU_ROW_HEIGHT};
}

static bool point_in_rect(float x, float y, const SDL_FRect *rect) {
        return x >= rect->x && x <= rect->x + rect->w && y >= rect->y &&
               y <= rect->y + rect->h;
}

void settings_init(Settings *settings) { *settings = (Settings){0}; }

bool settings_render_icon(SDL_Renderer *renderer, SDL_Texture *icon,
                          int window_width) {
        if (!icon) {
                return false;
        }

        SDL_FRect icon_rect = settings_icon_rect(window_width);
        SDL_RenderCopyF(renderer, icon, NULL, &icon_rect);
        return true;
}

void settings_render_menu(const Settings *settings, SDL_Renderer *renderer,
                          FontAtlas *font, int window_width,
                          int window_height) {
        if (!settings->open || !font) {
                return;
        }

        SDL_FRect menu = settings_menu_rect(window_width, window_height);
        SDL_FRect close = settings_close_rect(window_width, window_height);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 235);
        SDL_RenderFillRectF(renderer, &menu);

        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        SDL_RenderDrawRectF(renderer, &menu);

        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);
        if (point_in_rect((float)mouse_x, (float)mouse_y, &close)) {
                SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
                SDL_RenderFillRectF(renderer, &close);
        }

        SDL_Color white = {255, 255, 255, 255};
        SDL_Color muted = {180, 180, 180, 255};

        double text_x = menu.x + SETTINGS_MENU_PADDING;
        double title_y = menu.y + SETTINGS_MENU_PADDING;
        double note_y = title_y + SETTINGS_MENU_ROW_HEIGHT;
        double close_y = close.y + (close.h - (double)font->line_height) / 2.0;

        font_draw(font, renderer, "Settings", text_x, title_y, white);
        font_draw(font, renderer, "No settings yet.", text_x, note_y, muted);
        font_draw(font, renderer, "Close", text_x, close_y, white);
}

bool settings_handle_event(Settings *settings, SDL_Event *event,
                           int window_width, int window_height,
                           bool *popup_open) {
        switch (event->type) {
        case SDL_MOUSEBUTTONDOWN: {
                if (event->button.button != SDL_BUTTON_LEFT) {
                        return false;
                }

                float x = (float)event->button.x;
                float y = (float)event->button.y;
                SDL_FRect icon = settings_icon_rect(window_width);

                if (point_in_rect(x, y, &icon)) {
                        settings->open = !settings->open;
                        settings->capturing_mouse = true;
                        if (settings->open && popup_open) {
                                *popup_open = false;
                        }
                        return true;
                }

                if (!settings->open) {
                        return false;
                }

                SDL_FRect menu = settings_menu_rect(window_width, window_height);
                SDL_FRect close =
                    settings_close_rect(window_width, window_height);

                if (point_in_rect(x, y, &close)) {
                        settings->open = false;
                } else if (!point_in_rect(x, y, &menu)) {
                        settings->open = false;
                }

                settings->capturing_mouse = true;
                return true;
        }

        case SDL_MOUSEBUTTONUP: {
                if (event->button.button == SDL_BUTTON_LEFT &&
                    settings->capturing_mouse) {
                        settings->capturing_mouse = false;
                        return true;
                }
                return false;
        }

        case SDL_KEYDOWN: {
                if (event->key.keysym.sym == SDLK_ESCAPE && settings->open) {
                        settings->open = false;
                        return true;
                }
                return false;
        }

        default:
                return false;
        }
}
