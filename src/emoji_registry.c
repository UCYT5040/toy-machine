#include "world.h"

EmojiDefinition EMOJI_DEFINITIONS[EMOJI_COUNT] = {
    [EMOJI_SOCCER_BALL] = {.name = "soccer_ball",
                           .svg_path = "assets/emojis/soccer_ball.svg",
                           .radius = 20.0f,
                           .mass = 1.0f,
                           .restitution = 0.8f,
                           .scale = 1.0f,
                           .default_tags = TAG_NONE,
                           .update = NULL,
                           .collide = NULL}};