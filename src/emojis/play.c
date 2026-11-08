#include "emojis/play.h"
#include "app.h"
#include "scenes/play.h"

void emoji_play_click(App *app, World *world, Entity *self) {
        if (app->play_emoji_click_function) {
                app->play_emoji_click_function(world, app);
        }
}