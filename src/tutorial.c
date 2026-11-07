#include "tutorial.h"
#include "dispenser.h"

void tutorial_enter_step(World *world, TutorialStep step) {
        switch (step) {
        case TUTORIAL_STEP_START:
                dispense_note(world, "Welcome to the Toy Machine! Click on the "
                                     "note to read it.");
                break;
        }
}

void tutorial_update(World *world, double dt) {
        if (world->tutorial_progress >= TUTORIAL_STEP_COMPLETE)
                return;

        bool goal_reached = false;

        if (goal_reached) {
                world->tutorial_progress++;
                tutorial_enter_step(world, world->tutorial_progress);
        }
}