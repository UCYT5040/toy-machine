#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>

#define MAX_ENTITIES 1024

typedef struct {
        double x, y;
} Vec2;

typedef enum {
        EMOJI_NONE = 0,
        EMOJI_SOCCER_BALL,
        EMOJI_EARTH,
        EMOJI_COUNT
} EmojiType;

typedef enum {
        TAG_NONE = 0,
        TAG_FLAMMABLE,
        TAG_CONDUCTIVE,
        TAG_LIVING,
        TAG_METALLIC,
        TAG_HOT,
        TAG_BURNING,
        TAG_WET,
        TAG_ELECTRIC
} EmojiTag;

struct World;
struct Entity;

typedef void (*EmojiUpdateFunction)(struct World *world, struct Entity *self,
                                    double dt);
typedef void (*EmojiCollideFunction)(struct World *world, struct Entity *self,
                                     struct Entity *other);

typedef struct {
        const char *name;
        const char *svg_path;

        int index;

        double radius; /* TODO: Consider allowing custom shapes for non-circular
                          emojis, or perhaps auto-detecting the shape */
        double mass;   /* Immovable if 0.0 */
        double restitution; /* Bounciness, from 0.0 (no bounce) to 1.0
                               (perfectly elastic) */
        uint32_t default_tags;

        EmojiUpdateFunction update;
        EmojiCollideFunction collide;
} EmojiDefinition;

typedef struct Entity {
        EmojiDefinition *definition;

        bool active; /* Entity slot is free if false */
        int index;
        EmojiType type;

        Vec2 pos;
        Vec2 v;
        Vec2 a;
        double angle;

        double scale; // To scale individual entities

        uint32_t tags; // TODO: See above comment about bitmasking
        double timer;  // Can be used for various purposes
} Entity;

typedef struct World {
        Entity entities[MAX_ENTITIES];
        SDL_Texture *textures[EMOJI_COUNT];

        int width, height;

        /*
        Using passive object with large mass to create gravity
        double gravity;
        */

        Vec2 mouse_pos;
        bool mouse_down;
        int dragged_entity_index; // -1 if no entity is being dragged

        bool dev_command_mode; // If true, the dev command window is open (game
                               // is paused)
        bool dev_show_hitboxes;
        // TODO: Add more developer/debugging features
} World;

void world_init(World *world, SDL_Renderer *renderer);
void world_update(World *world, double dt);
void world_render(World *world, SDL_Renderer *renderer);
