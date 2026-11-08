#include "app.h"
#include "recipes.h"
#include "title.h"
#include "world.h"

#define WINDOW_TITLE "Toy Machine"
#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 600

int main(void) {
        App app = {0};

        app.scene = &title_scene;

        if (!app_init(&app, WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT)) {
                return 1;
        }

        recipes_init();

        app_run(&app);

        app_destroy(&app);

        return 0;
}