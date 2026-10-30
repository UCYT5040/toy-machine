#pragma once
#include "world.h"
#include <stdbool.h>

typedef enum {
        CRITERION_EXACT,
        CRITERION_TAGS,
        CRITERION_PREDICATE
} CriterionMode;

typedef struct {
        CriterionMode mode;
        union {
                EmojiType type;
                struct {
                        uint64_t required_tags;
                        uint64_t forbidden_tags;
                        bool default_tags_only; // If true, tags are taken from
                                                // the emoji definition, not the
                                                // entity instance (recommended
                                                // true for most cases)
                } tags;
                bool (*predicate)(const struct Entity *entity);
        };
} IngredientCriterion;

typedef enum { OUTPUT_STATIC, OUTPUT_DYNAMIC } OutputMode;

typedef EmojiType (*RecipeOutputFunction)(const Entity *a, const Entity *b);

typedef struct {
        OutputMode mode;
        union {
                EmojiType static_output;
                RecipeOutputFunction dynamic_output;
        };
} RecipeOutput;

typedef struct Recipe {
        const char *id;
        IngredientCriterion inputs[2];
        RecipeOutput output;

        double processing_time; // Time in seconds to process the recipe
        bool consume_a;
        bool consume_b;

        int specificity; // Higher values evaluated first
} Recipe;

void recipes_init(void);

bool recipe_criterion_matches(const IngredientCriterion *criterion,
                              const Entity *entity);

const Recipe *recipe_find_match(
    const Entity *a, const Entity *b, const Entity **out_a,
    const Entity **out_b); // Sets out_a and out_b to the entities that matched
                           // the recipe criteria, in order

EmojiType recipe_resolve_output(const Recipe *recipe, const Entity *a,
                                const Entity *b);
