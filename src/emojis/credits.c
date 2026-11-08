#include "emojis/credits.h"

void emoji_credits_click(App *app, World *world, Entity *self) {
        world->popup_open = true;
        self->note_text = "Toy Machine";
        world->popup_text = self->note_text;
        world->popup_type = POPUP_TYPE_CREDITS;
}