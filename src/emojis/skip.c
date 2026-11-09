#include "emojis/skip.h"
#include "app.h"
#include "scenes/play.h"

void emoji_skip_click(App *app, World *world, Entity *self) {
        app_set_scene(app, &play_scene);
}