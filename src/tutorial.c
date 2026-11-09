#include "tutorial.h"
#include "dispenser.h"

void tutorial_enter_step(World *world, TutorialStep step) {
        switch (step) {
        case TUTORIAL_STEP_START:
                dispense_note(world,
                              "Welcome to the Toy Machine!\nPress space "
                              "to open the sidebar.\n(here... have a coin!)");
                break;
        case TUTORIAL_STEP_READ_WELCOME_NOTE:
                dispense_currency(world, 1);
                break;
        case TUTORIAL_STEP_OPEN_SIDEBAR:
                dispense_note(world, "Spawn a factory. Put stone and wood into "
                                     "it to make a hammer!");
                break;
        case TUTORIAL_STEP_CRAFT_HAMMER:
                dispense_note(
                    world,
                    "We need cash. Combine stone and a hammer to make a bank! "
                    "You'll have to put a coin in- hope you didn't lose it!");
                break;
        case TUTORIAL_STEP_CRAFT_BANK:
                dispense_note(
                    world,
                    "Shake the bank to get coins! (TIP: you can "
                    "automate the shaking using other emojis, but "
                    "it's less efficient than manual shaking)\nYou can also "
                    "make an ATM to get coins automatically! Combine a bank "
                    "with a gear. But it costs money to make!");
                break;
        case TUTORIAL_STEP_COMPLETE:
                dispense_note(world, "Congratulations, you finished the "
                                     "tutorial! Have fun!");
                break;
        }
}

void tutorial_update(World *world, double dt) {
        if (world->tutorial_progress >= TUTORIAL_STEP_COMPLETE)
                return;

        bool goal_reached = false;

        switch (world->tutorial_progress) {
        case TUTORIAL_STEP_START:
                if (world->popup_open) {
                        goal_reached = true;
                }
                break;
        case TUTORIAL_STEP_READ_WELCOME_NOTE:
                if (world->sidebar_shown) {
                        goal_reached = true;
                }
                break;
        case TUTORIAL_STEP_OPEN_SIDEBAR:
                if (EMOJI_DEFINITIONS[EMOJI_HAMMER].unlocked)
                        goal_reached = true;
                break;
        case TUTORIAL_STEP_CRAFT_HAMMER:
                if (EMOJI_DEFINITIONS[EMOJI_BANK].unlocked)
                        goal_reached = true;
                break;
        case TUTORIAL_STEP_CRAFT_BANK:
                if (EMOJI_DEFINITIONS[EMOJI_ATM].unlocked)
                        goal_reached = true;
                break;
        }

        if (goal_reached) {
                world->tutorial_progress++;
                tutorial_enter_step(world, world->tutorial_progress);
        }
}