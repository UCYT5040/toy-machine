#include "recipes.h"
#include "emoji_registry.h"
#include <stdlib.h>
#include <string.h>

static Recipe RECIPES[] =
    {
        {
            .id = "hammer",
            .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_STONE},
                       {.mode = CRITERION_EXACT, .type = EMOJI_WOOD}},
            .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_HAMMER},

            .processing_time = 5.0,

            .coins_required = 0,

            .specificity = 0 // Will be calculated in recipes_init()
        },
        {.id = "steam",
         .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_WATER},
                    {.mode = CRITERION_EXACT, .type = EMOJI_FIRE}},
         .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_STEAM},

         .processing_time = 1.0,

         .coins_required = 0,

         .specificity = 0},
        {.id = "steam_train",
         .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_STEAM},
                    {.mode = CRITERION_EXACT, .type = EMOJI_GEAR}},
         .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_STEAM_TRAIN},

         .processing_time = 8.0,

         .coins_required = 5,

         .specificity = 0},
        {.id = "fan",
         .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_WIND},
                    {.mode = CRITERION_EXACT, .type = EMOJI_GEAR}},
         .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_FAN},

         .processing_time = 4.0,

         .coins_required = 2,

         .specificity = 0},
        {.id = "electricity",
         .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_FIRE},
                    {.mode = CRITERION_EXACT, .type = EMOJI_GEAR}},
         .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_ELECTRICITY},

         .processing_time = 4.0,

         .coins_required = 10,

         .specificity = 0},
        {.id = "lava",
         .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_STONE},
                    {.mode = CRITERION_EXACT, .type = EMOJI_FIRE}},
         .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_LAVA},

         .processing_time = 4.0,

         .coins_required = 0,

         .specificity = 0},
        {
            .id = "gear",
            .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_LAVA},
                       {.mode = CRITERION_EXACT, .type = EMOJI_HAMMER}},
            .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_GEAR},

            .processing_time = 6.0,

            .coins_required = 0,

            .specificity = 0,
        },
        {
            .id = "electric_train",
            .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_ELECTRICITY},
                       {.mode = CRITERION_EXACT,
                        .type = EMOJI_STEAM_TRAIN}}, /* TODO: Probably should
                                                        change this up */
            .output = {.mode = OUTPUT_STATIC,
                       .static_output = EMOJI_ELECTRIC_TRAIN},

            .processing_time = 6.0,

            .coins_required = 15,

            .specificity = 0,
        },
        {
            .id = "bank",
            .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_STONE},
                       {.mode = CRITERION_EXACT, .type = EMOJI_HAMMER}},
            .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_BANK},

            .processing_time = 8.0,

            .coins_required = 1,

            .specificity = 0,
        },
        {
            .id = "atm",
            .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_BANK},
                       {.mode = CRITERION_EXACT, .type = EMOJI_GEAR}},
            .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_ATM},

            .processing_time = 8.0,

            .coins_required = 4,

            .specificity = 0,
        },
        {
            .id = "paper",
            .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_WOOD},
                       {.mode = CRITERION_EXACT, .type = EMOJI_WATER}},
            .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_PAPER},

            .processing_time = 2.0,

            .coins_required = 0,

            .specificity = 0,
        },
        {.id = "pin",
         .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_STONE},
                    {.mode = CRITERION_EXACT, .type = EMOJI_HAMMER}},
         .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_PIN},

         .processing_time = 2.0,

         .coins_required = 0},
        {.id = "balloon",
         .inputs =
             {
                 {.mode = CRITERION_EXACT, .type = EMOJI_WIND},
                 {.mode = CRITERION_EXACT, .type = EMOJI_FIRE},
             },
         .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_BALLOON},

         .processing_time = 3.0,

         .coins_required = 2},
        {.id = "bomb",
         .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_FIRE},
                    {.mode = CRITERION_EXACT, .type = EMOJI_LAVA}},
         .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_BOMB},
         .processing_time = 5.0,
         .coins_required = 10},
        {.id = "parachute",
         .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_PIN},
                    {.mode = CRITERION_EXACT, .type = EMOJI_WIND}},
         .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_PARACHUTE},

         .processing_time = 4.0,
         .coins_required = 5},
        {.id = "computer_mouse",
         .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_ELECTRICITY},
                    {.mode = CRITERION_EXACT, .type = EMOJI_WOOD}},

         .processing_time = 4.0,

         .coins_required = 10},
        {.id = "mirror",
         .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_FIRE},
                    {.mode = CRITERION_EXACT, .type = EMOJI_STONE}},
         .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_MIRROR},

         .processing_time = 3.0,

         .coins_required = 0},
        {.id = "package",
         .inputs =
             {
                 {.mode = CRITERION_EXACT, .type = EMOJI_PAPER},
                 {.mode = CRITERION_EXACT, .type = EMOJI_WOOD},
             },
         .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_PACKAGE},
         .processing_time = 2.0,

         .coins_required = 0},
        {.id = "soccer_ball",
         .inputs =
             {
                 {.mode = CRITERION_EXACT, .type = EMOJI_BALLOON},
                 {.mode = CRITERION_EXACT, .type = EMOJI_WOOD},
             },
         .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_PACKAGE},
         .processing_time = 3.0,

         .coins_required = 5},
        {.id = "rain_cloud",
         .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_WATER},
                    {.mode = CRITERION_EXACT, .type = EMOJI_WIND}},
         .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_RAIN_CLOUD},

         .processing_time = 4.0,

         .coins_required = 0}};

static const size_t RECIPE_COUNT = sizeof(RECIPES) / sizeof(RECIPES[0]);

static int popcount64(uint64_t x) {
        int count = 0;
        while (x) {
                count += x & 1;
                x >>= 1;
        }
        return count;
}

static int
calculate_criterion_specificity(const IngredientCriterion *criterion) {
        switch (criterion->mode) {
        case CRITERION_EXACT:
                return 100;
        case CRITERION_TAGS:
                return popcount64(criterion->tags.required_tags) * 10 +
                       popcount64(criterion->tags.forbidden_tags) * 5;
        case CRITERION_PREDICATE:
                return 20;
        }
        return 0;
}

static int recipe_compare_specificity(const void *a, const void *b) {
        const Recipe *ra = (const Recipe *)a;
        const Recipe *rb = (const Recipe *)b;
        return rb->specificity - ra->specificity;
}

void recipes_init(void) {
        for (size_t i = 0; i < RECIPE_COUNT; i++) {
                if (RECIPES[i].specificity == 0) {
                        RECIPES[i].specificity =
                            calculate_criterion_specificity(
                                &RECIPES[i].inputs[0]) +
                            calculate_criterion_specificity(
                                &RECIPES[i].inputs[1]);
                }
        }

        qsort(RECIPES, RECIPE_COUNT, sizeof(Recipe),
              recipe_compare_specificity);
}

const Recipe *recipe_find_by_id(const char *id) {
        if (!id || !*id) {
                return NULL;
        }

        for (size_t i = 0; i < RECIPE_COUNT; i++) {
                if (RECIPES[i].id && strcmp(RECIPES[i].id, id) == 0) {
                        return &RECIPES[i];
                }
        }

        return NULL;
}

bool recipe_criterion_matches(const IngredientCriterion *criterion,
                              const Entity *entity) {
        if (!entity)
                return false;

        switch (criterion->mode) {
        case CRITERION_EXACT:
                return entity->type == criterion->type;
        case CRITERION_TAGS: {
                uint64_t tags =
                    criterion->tags.default_tags_only
                        ? EMOJI_DEFINITIONS[entity->type].default_tags
                        : entity->tags;

                if ((tags & criterion->tags.required_tags) !=
                    criterion->tags.required_tags) {
                        return false;
                }
                if (criterion->tags.forbidden_tags &&
                    (tags & criterion->tags.forbidden_tags) != 0) {
                        return false;
                }

                return true;
        }
        case CRITERION_PREDICATE:
                return criterion->predicate(entity);
        default:
                return false; // idk
        }
}

const Recipe *recipe_find_match(const Entity *a, const Entity *b,
                                const Entity **out_a, const Entity **out_b) {
        if (!a || !b)
                return NULL;

        for (size_t i = 0; i < RECIPE_COUNT; i++) {
                const Recipe *recipe = &RECIPES[i];

                if (recipe_criterion_matches(&recipe->inputs[0], a) &&
                    recipe_criterion_matches(&recipe->inputs[1], b)) {
                        if (out_a)
                                *out_a = a;
                        if (out_b)
                                *out_b = b;
                        return recipe;
                }

                if (recipe_criterion_matches(&recipe->inputs[0], b) &&
                    recipe_criterion_matches(&recipe->inputs[1], a)) {
                        if (out_a)
                                *out_a = b;
                        if (out_b)
                                *out_b = a;
                        return recipe;
                }
        }

        return NULL;
}

EmojiType recipe_resolve_output(const Recipe *recipe, const Entity *a,
                                const Entity *b) {
        if (!recipe)
                return EMOJI_NONE;

        if (recipe->output.mode == OUTPUT_STATIC) {
                return recipe->output.static_output;
        } else if (recipe->output.mode == OUTPUT_DYNAMIC) {
                return recipe->output.dynamic_output(a, b);
        }

        return EMOJI_NONE;
}