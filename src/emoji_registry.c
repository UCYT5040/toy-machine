#include "world.h"

const EmojiDefinition EMOJI_DEFINITIONS[EMOJI_COUNT] = {
    [EMOJI_SOCCER_BALL] = {.name = "Soccer Ball",
                           .svg_path = "assets/soccer_ball.svg",
                           .radius = 20.0f,
                           .mass = 1.0f,
                           .restitution = 0.8f,
                           .default_tags = TAG_NONE,
                           .update = NULL,
                           .collide = NULL}};