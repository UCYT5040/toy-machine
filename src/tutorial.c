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
        case TUTORIAL_STEP_COMPLETE:

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
        }

        if (goal_reached) {
                world->tutorial_progress++;
                tutorial_enter_step(world, world->tutorial_progress);
        }
}