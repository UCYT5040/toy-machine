#pragma once

#include "types.h"

typedef enum {
        EMOJI_NONE = 0,
        EMOJI_COMPUTER_MOUSE,
        EMOJI_SOCCER_BALL,
        EMOJI_FACTORY,
        EMOJI_EARTH,
        EMOJI_STONE,
        EMOJI_FIRE,
        EMOJI_WATER,
        EMOJI_WOOD,
        EMOJI_WIND,
        EMOJI_HAMMER,
        EMOJI_GEAR,
        EMOJI_PIN,
        EMOJI_BANK,
        EMOJI_COIN,
        EMOJI_FAN,
        EMOJI_TRASHBIN,
        EMOJI_BROOM,
        EMOJI_STEAM,
        EMOJI_STEAM_TRAIN,
        EMOJI_COUNT
} EmojiType;

typedef enum {
        TAG_NONE = 0,
        TAG_FLAMMABLE = 1ULL << 0,
        TAG_CONDUCTIVE = 1ULL << 1,
        TAG_LIVING = 1ULL << 2,
        TAG_METALLIC = 1ULL << 3,
        TAG_HOT = 1ULL << 4,
        TAG_BURNING = 1ULL << 5,
        TAG_WET = 1ULL << 6,
        TAG_ELECTRIC = 1ULL << 7,
        TAG_PROCESSOR = 1ULL << 8,
        TAG_SPACE = 1ULL << 9 // E.g., planets, starts, etc.
} EmojiTag;

typedef enum {
        EMOJI_PROCESSING_DISPLAY_NONE = 0,
        EMOJI_PROCESSING_DISPLAY_COUNTER, // Show a list of each input, with
                                          // similar inputs stacked
        EMOJI_PROCESSING_DISPLAY_SUM      // Show each input being added
} EmojiProcessingDisplay;

typedef struct {
        const char *name;
        const char *svg_path;

        int index;

        double radius; // Essentially a bounding radius, holding all colliders
                       // within it
        Collider colliders[MAX_COLLIDERS];
        double mass;           /* Immovable if 0.0 */
        double restitution;    /* Bounciness, from 0.0 (no bounce) to 1.0
                               (perfectly elastic) */
        uint64_t default_tags; // Supports up to 64 tags

        int collision_priority; /* Higher values runs first and on contact */

        EmojiUpdateFunction update;
        EmojiCollideFunction collide;
} EmojiDefinition;

extern EmojiDefinition EMOJI_DEFINITIONS[EMOJI_COUNT];