#include "emojis/credits.h"

void emoji_credits_click(App *app, World *world, Entity *self) {
        world->popup_open = true;
        self->note_text =
            "Toy Machine\nBy Jeremiah, Simon, and Utkrish\n\nMUSIC\n\"Corona "
            "Radiate\" & \"Demon Seed\"\nby Nine Inch Nails (The "
            "Slip)\nLicensed under CC BY-NC-SA 3.0\n\nSOUNDS\n\"Falling Rain "
            "SFX\" by valvalion\n via Wikimedia Commons (CC BY "
            "3.0)\n\nGRAPHICS\nEmojis based on Google Noto Emoji\nLicensed "
            "under Apache 2.0\nDerivative works adapted for Toy Machine";
        world->popup_text = self->note_text;
        world->popup_type = POPUP_TYPE_CREDITS;
}