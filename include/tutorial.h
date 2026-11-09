#pragma once

#include "world.h"

typedef enum {
        TUTORIAL_STEP_START = 0,
        TUTORIAL_STEP_READ_WELCOME_NOTE,
        TUTORIAL_STEP_OPEN_SIDEBAR,
        TUTORIAL_STEP_CRAFT_HAMMER,
        TUTORIAL_STEP_CRAFT_BANK,
        TUTORIAL_STEP_CRAFT_ATM,
        TUTORIAL_STEP_COMPLETE
} TutorialStep;

void tutorial_enter_step(World *world, TutorialStep step);

void tutorial_update(World *world, double dt);