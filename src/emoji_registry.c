#include "world.h"

EmojiDefinition EMOJI_DEFINITIONS[EMOJI_COUNT] = {
    [EMOJI_SOCCER_BALL] = {.name = "soccer_ball",
                           .svg_path = "assets/emojis/soccer_ball.svg",
                           .radius = 20.0,
                           .mass = 1.0,
                           .restitution = 0.8,
                           .default_tags = TAG_NONE,
                           .update = NULL,
                           .collide = NULL}};