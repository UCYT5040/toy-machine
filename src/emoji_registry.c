#include "world.h"

EmojiDefinition EMOJI_DEFINITIONS[EMOJI_COUNT] = {
    [EMOJI_COMPUTER_MOUSE] = {.name = "computer_mouse",
                              .svg_path = "assets/emojis/computer_mouse.svg",
                              .radius = 20.0,
                              .mass = 1.0, /* I don't know how this works so it's just 1.0 now */
                              .restitution = 0.1,
                              .default_tags = TAG_NONE, /* Not sure how the tags works so I'm leaving it as is */
                              .update = NULL,
                              .collide = NULL},
    [EMOJI_SOCCER_BALL] = {.name = "soccer_ball",
                           .svg_path = "assets/emojis/soccer_ball.svg",
                           .radius = 20.0,
                           .mass = 1.0,
                           .restitution = 0.8,
                           .default_tags = TAG_NONE,
                           .update = NULL,
                           .collide = NULL},
    [EMOJI_FACTORY] = {.name = "factory",
                       .svg_path = "assets/emojis/factory.svg",
                       .radius = 30.0,
                       .mass = 2.0,
                       .restitution = 0.5,
                       .default_tags = TAG_PROCESSOR,
                       .update = NULL,
                       .collide = NULL},
    [EMOJI_EARTH] = {.name = "earth",
                     .svg_path = "assets/emojis/earth.svg",
                     .radius = 100.0,
                     .mass = 5.974e17, // Too small on purpose
                     .restitution = 0.0,
                     .default_tags = TAG_NONE,
                     .update = NULL,
                     .collide = NULL}};
