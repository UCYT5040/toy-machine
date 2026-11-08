#include "emojis/paper.h"

void emoji_paper_click(App *app, World *world, Entity *self) {
        world->popup_open = true;
        world->popup_text = self->note_text;
}