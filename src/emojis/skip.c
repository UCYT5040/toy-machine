#include "emojis/skip.h"
#include "app.h"
#include "scenes/play.h"

void emoji_skip_click(App *app, World *world, Entity *self) {
        if (app->play_emoji_click_function) {
                app->play_emoji_click_function(world, app);
        }
}