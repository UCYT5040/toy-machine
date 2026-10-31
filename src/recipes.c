#include "recipes.h"
#include "emoji_registry.h"
#include <stdlib.h>

static Recipe RECIPES[] = {{
    .id = "test_recipe",
    .inputs = {{.mode = CRITERION_EXACT, .type = EMOJI_COMPUTER_MOUSE},
               {.mode = CRITERION_EXACT, .type = EMOJI_SOCCER_BALL}},
    .output = {.mode = OUTPUT_STATIC, .static_output = EMOJI_EARTH},

    .processing_time = 5.0,
    .consume_a = true,
    .consume_b = true,

    .specificity = 0 // Will be calculated in recipes_init()
}};

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