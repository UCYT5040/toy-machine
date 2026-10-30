#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>

#define MAX_ENTITIES 1024

typedef struct {
        double x, y;
} Vec2;

typedef enum {
        EMOJI_NONE = 0,
        EMOJI_COMPUTER_MOUSE,
        EMOJI_SOCCER_BALL,
        EMOJI_FACTORY,
        EMOJI_EARTH,
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
        TAG_PROCESSOR = 1ULL << 8
} EmojiTag;

struct World;
struct Entity;

typedef void (*EmojiUpdateFunction)(struct World *world, struct Entity *self,
                                    double dt);
typedef void (*EmojiCollideFunction)(struct World *world, struct Entity *self,
                                     struct Entity *other);

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

        double radius; /* TODO: Consider allowing custom shapes for non-circular
                       emojis, or perhaps auto-detecting the shape */
        double mass;   /* Immovable if 0.0 */
        double restitution;    /* Bounciness, from 0.0 (no bounce) to 1.0
                               (perfectly elastic) */
        uint64_t default_tags; // Supports up to 64 tags

        EmojiUpdateFunction update;
        EmojiCollideFunction collide;
} EmojiDefinition;

typedef struct Entity {
        EmojiDefinition *definition;

        bool active; /* Entity slot is free if false */
        int index;
        EmojiType type;

        bool processing; // Only applies to TAG_PROCESSOR emojis
        struct Entity *processing_entities;

        Vec2 pos;
        Vec2 v;
        Vec2 a;
        double angle;

        double scale; // To scale individual entities

        uint64_t tags;
        double timer; // Can be used for various purposes
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

        double speed; // Currently used only for dev commands, but could
                      // be an effect later on (this is a multiplier for dt)

        bool dev_command_mode; // If true, the dev command window is open (game
                               // is paused)
        bool dev_show_hitboxes;
        bool dev_show_velocity_vectors;
        // TODO: Add more developer/debugging features
} World;

void world_init(World *world, SDL_Renderer *renderer);
void world_update(World *world, double dt);
void world_render(World *world, SDL_Renderer *renderer);

int world_get_entity_at(
    World *world, Vec2 pos); // If multiple entities overlap, selects the entity
                             // with a center nearest to the position.

void world_handle_mouse_down(World *world);
void world_handle_mouse_up(World *world);
