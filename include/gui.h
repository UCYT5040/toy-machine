#ifdef GUI_H
#define GUI_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "font.h"
#include "world.h"

#define MAX_DEV_COMMAMD_LENGTH 256
#define MAX_DEV_COMMAND_OUTPUT 256

typedef struct {
        char command_buffer[MAX_DEV_COMMAMD_LENGTH];
        char output_buffer[MAX_DEV_COMMAND_OUTPUT];
        FontAtlas *font_atlas;
        bool was_mode_active;
} DevConsole;

typedef struct {
        SDL_Window *window;
        SDL_Renderer *renderer;
        FontAtlas *font_atlas;
        DevConsole dev_console;
} App;

bool app_init(App *app, const char *title, int width, int height);
bool app_run(App *app, World *world);
void app_destroy(App *app);

void gui_console_init(DevConsole *console, FontAtlas *font_atlas);
void gui_console_handle_event(DevConsole *console, World *world, SDL_Event *event, bool *running);
void gui_console_render(DevConsole *console, SDL_Renderer *renderer);

#endif