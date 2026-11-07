#include "tutorial.h"

void tutorial_enter_step(World *world, TutorialStep step) {
        switch (step) {
        case TUTORIAL_STEP_START:
                break; // TODO: Show a note
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