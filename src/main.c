#include "gui.h"
#include "recipes.h"
#include "world.h"

#define WINDOW_TITLE "Toy Machine"
#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 600

int main(void) {
        App app;
        if (!app_init(&app, WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT)) {
                return 1;
        }

        World world;
        world_init(&world, app.renderer);

        recipes_init();

        app_run(&app, &world);

        app_destroy(&app);

        return 0;
}