#include "emojis/play.h"
#include "app.h"
#include "scenes/play.h"

void emoji_play_click(App *app, World *world, Entity *self) {
        if (app->scene->type == SCENE_TYPE_TITLE) {
                app_set_scene(app, &play_scene);
        }
}