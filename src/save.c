#include "save.h"

#include "emoji_registry.h"
#include "entities.h"
#include "recipes.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAVE_VERSION 3u
#define SAVE_MAX_RECIPE_ID 32
#define SAVE_MAX_INPUTS 8

static const uint8_t SAVE_MAGIC[4] = {'T', 'O', 'Y', 'S'};

_Static_assert(EMOJI_COUNT <= 64,
               "unlocked_mask must be able to hold every emoji");

typedef struct {
        uint8_t magic[4];
        uint32_t version;
        uint32_t header_size;
        uint32_t entity_size;
        uint32_t emoji_count;
        uint32_t entity_count;
        int32_t vault_balance;
        uint64_t unlocked_mask;
        Vec2 gravity;
        bool space_mode;
        double space_mode_transition;
        uint32_t last_mouse_move;
        double speed;
} SaveHeader;

typedef struct {
        int32_t index;
        int32_t type;

        uint8_t active;
        uint8_t pinned;
        int32_t pinned_target_index;
        int32_t pinned_by_index;
        Vec2 pinned_offset;

        uint8_t processing;
        int32_t processing_input_count;
        double processing_timer;
        int32_t needs_coins;
        uint8_t ejecting;
        double eject_timer;

        char recipe_id[SAVE_MAX_RECIPE_ID];
        int32_t processing_inputs[SAVE_MAX_INPUTS];

        Vec2 pos;
        Vec2 last_pos;
        Vec2 v;
        Vec2 a;
        Vec2 shake_last_delta;

        double shake_distance;
        double despawn_timer;
        double angle;
        double omega;
        double torque;
        double scale;
        uint64_t tags;
} SaveEntity;

#define ENTITY_SAVE_FIELDS(X, e, s)                                            \
        X(active)                                                              \
        X(pinned)                                                              \
        X(pinned_target_index)                                                 \
        X(pinned_by_index)                                                     \
        X(pinned_offset)                                                       \
        X(processing)                                                          \
        X(processing_input_count)                                              \
        X(processing_timer)                                                    \
        X(needs_coins)                                                         \
        X(ejecting)                                                            \
        X(eject_timer)                                                         \
        X(pos)                                                                 \
        X(last_pos)                                                            \
        X(v)                                                                   \
        X(a)                                                                   \
        X(shake_last_delta)                                                    \
        X(shake_distance)                                                      \
        X(despawn_timer)                                                       \
        X(angle)                                                               \
        X(omega)                                                               \
        X(torque)                                                              \
        X(scale)                                                               \
        X(tags)

static SaveEntity pack_entity(const Entity *entity, int index) {
        SaveEntity saved = {0};
        saved.index = index;
        saved.type = entity->type;

#define COPY_TO_SAVE(field) saved.field = entity->field;
        ENTITY_SAVE_FIELDS(COPY_TO_SAVE, entity, &saved)
#undef COPY_TO_SAVE

        if (entity->current_recipe) {
                snprintf(saved.recipe_id, sizeof saved.recipe_id, "%s",
                         entity->current_recipe->id);
        }

        int input_count = entity->processing_input_count;
        if (input_count < 0) {
                input_count = 0;
        } else if (input_count > SAVE_MAX_INPUTS) {
                input_count = SAVE_MAX_INPUTS;
        }

        saved.processing_input_count = input_count;
        for (int i = 0; i < input_count; i++) {
                saved.processing_inputs[i] = entity->processing_inputs[i];
        }

        return saved;
}

static bool unpack_entity(Entity *entity, const SaveEntity *saved) {
        *entity = (Entity){0};
        entity->index = saved->index;
        entity->type = saved->type;
        entity->definition = &EMOJI_DEFINITIONS[saved->type];

#define COPY_FROM_SAVE(field) entity->field = saved->field;
        ENTITY_SAVE_FIELDS(COPY_FROM_SAVE, entity, saved)
#undef COPY_FROM_SAVE

        if (saved->recipe_id[0] != '\0') {
                entity->current_recipe = recipe_find_by_id(saved->recipe_id);
                if (!entity->current_recipe) {
                        return false;
                }
        }

        if (entity->processing_input_count > 0) {
                size_t count = (size_t)entity->processing_input_count;
                entity->processing_inputs = malloc(count * sizeof(int));
                if (!entity->processing_inputs) {
                        return false;
                }
                for (size_t i = 0; i < count; i++) {
                        entity->processing_inputs[i] =
                            saved->processing_inputs[i];
                }
        }

        entity_update_inertia(entity);
        return true;
}

static bool save_path(int slot, char *buffer, size_t buffer_size) {
        if (slot < 1 || slot > SAVE_SLOT_COUNT) {
                return false;
        }
        snprintf(buffer, buffer_size, "save_%d.bin", slot);
        return true;
}

bool save(const World *world, int slot, char *error, size_t error_size) {
        char path[64];
        if (!save_path(slot, path, sizeof path)) {
                snprintf(error, error_size,
                         "Save slot must be between 1 and %d", SAVE_SLOT_COUNT);
                return false;
        }

        char temp_path[72];
        snprintf(temp_path, sizeof temp_path, "%s.tmp", path);

        FILE *file = fopen(temp_path, "wb");
        if (!file) {
                snprintf(error, error_size, "Could not open %s", temp_path);
                return false;
        }

        SaveHeader header = {0};
        memcpy(header.magic, SAVE_MAGIC, sizeof header.magic);
        header.version = SAVE_VERSION;
        header.header_size = sizeof header;
        header.entity_size = sizeof(SaveEntity);
        header.emoji_count = EMOJI_COUNT;
        header.vault_balance = world->vault.balance;
        header.gravity = world->gravity;
        header.space_mode = world->space_mode;
        header.space_mode_transition = world->space_mode_transition;
        header.last_mouse_move = world->last_mouse_move;
        header.speed = world->speed;
        header.unlocked_mask = UINT64_C(0);

        for (int i = 1; i < EMOJI_COUNT; i++) {
                if (EMOJI_DEFINITIONS[i].unlocked) {
                        header.unlocked_mask |= UINT64_C(1) << i;
                }
        }

        for (int i = 0; i < MAX_ENTITIES; i++) {
                if (!world->entities[i].collect) {
                        header.entity_count++;
                }
        }

        bool ok = fwrite(&header, sizeof header, 1, file) == 1;

        for (int i = 0; ok && i < MAX_ENTITIES; i++) {
                const Entity *entity = &world->entities[i];
                if (entity->collect) {
                        continue;
                }

                SaveEntity saved = pack_entity(entity, i);
                ok = fwrite(&saved, sizeof saved, 1, file) == 1;
        }

        if (fclose(file) != 0) {
                ok = false;
        }

        if (!ok) {
                remove(temp_path);
                snprintf(error, error_size, "Could not write %s", temp_path);
                return false;
        }

        if (rename(temp_path, path) != 0) {
                remove(temp_path);
                snprintf(error, error_size, "Could not replace %s", path);
                return false;
        }

        return true;
}

bool load(World *world, int slot, char *error, size_t error_size) {
        char path[64];
        if (!save_path(slot, path, sizeof path)) {
                snprintf(error, error_size,
                         "Save slot must be between 1 and %d", SAVE_SLOT_COUNT);
                return false;
        }

        FILE *file = fopen(path, "rb");
        if (!file) {
                snprintf(error, error_size, "Could not open %s", path);
                return false;
        }

        SaveHeader header;
        bool ok = fread(&header, sizeof header, 1, file) == 1;

        if (ok) {
                ok = memcmp(header.magic, SAVE_MAGIC, sizeof SAVE_MAGIC) == 0 &&
                     header.version == SAVE_VERSION &&
                     header.header_size == sizeof header &&
                     header.entity_size == sizeof(SaveEntity) &&
                     header.emoji_count == (uint32_t)EMOJI_COUNT &&
                     header.entity_count <= (uint32_t)MAX_ENTITIES;
        }

        SaveEntity *saved = NULL;
        bool *occupied = NULL;

        if (ok && header.entity_count > 0) {
                saved = calloc(header.entity_count, sizeof *saved);
                occupied = calloc(MAX_ENTITIES, sizeof *occupied);
                ok = saved && occupied &&
                     fread(saved, sizeof *saved, header.entity_count, file) ==
                         header.entity_count;

                for (uint32_t i = 0; i < header.entity_count; i++) {
                        SaveEntity *s = &saved[i];
                        s->recipe_id[SAVE_MAX_RECIPE_ID - 1] = '\0';

                        if (s->index < 0 || s->index >= MAX_ENTITIES ||
                            occupied[s->index] || s->type <= EMOJI_NONE ||
                            s->type >= EMOJI_COUNT ||
                            s->processing_input_count < 0 ||
                            s->processing_input_count > SAVE_MAX_INPUTS) {
                                ok = false;
                                break;
                        }

                        if (s->recipe_id[0] != '\0' &&
                            !recipe_find_by_id(s->recipe_id)) {
                                ok = false;
                                break;
                        }

                        occupied[s->index] = true;
                }
        }

        if (ok && header.entity_count > 0) {
                for (uint32_t i = 0; i < header.entity_count && ok; i++) {
                        SaveEntity *s = &saved[i];

                        for (int j = 0; j < s->processing_input_count; j++) {
                                int input = s->processing_inputs[j];
                                if (input < 0 || input >= MAX_ENTITIES ||
                                    !occupied[input]) {
                                        ok = false;
                                        break;
                                }
                        }

                        if (s->pinned_target_index >= 0 &&
                            (s->pinned_target_index >= MAX_ENTITIES ||
                             !occupied[s->pinned_target_index])) {
                                ok = false;
                        }

                        if (s->pinned_by_index >= 0 &&
                            (s->pinned_by_index >= MAX_ENTITIES ||
                             !occupied[s->pinned_by_index])) {
                                ok = false;
                        }
                }
        }

        fclose(file);

        if (ok) {
                for (int i = 0; i < MAX_ENTITIES; i++) {
                        free(world->entities[i].processing_inputs);
                }

                memset(world->entities, 0, sizeof world->entities);

                for (int i = 0; i < MAX_ENTITIES; i++) {
                        world->entities[i].index = i;
                        world->entities[i].collect = true;
                }

                for (uint32_t i = 0; ok && i < header.entity_count; i++) {
                        const SaveEntity *s = &saved[i];
                        Entity *entity = &world->entities[s->index];
                        ok = unpack_entity(entity, s);
                        if (ok) {
                                entity->collect = false;
                        }
                }

                if (ok) {
                        world->vault.balance = header.vault_balance;
                        world->gravity = header.gravity;
                        world->space_mode = header.space_mode;
                        world->space_mode_transition =
                            header.space_mode_transition;
                        world->last_mouse_move = header.last_mouse_move;
                        world->speed = header.speed;
                        world->save_slot = slot;

                        for (int i = 1; i < EMOJI_COUNT; i++) {
                                EMOJI_DEFINITIONS[i].unlocked =
                                    (header.unlocked_mask &
                                     (UINT64_C(1) << i)) != 0;
                        }
                        EMOJI_DEFINITIONS[EMOJI_NONE].unlocked = false;
                        world->sidebar_shown = false;
                        world->dragged_entity_index = -1;
                } else {
                        for (int i = 0; i < MAX_ENTITIES; i++) {
                                free(world->entities[i].processing_inputs);
                        }
                        memset(world->entities, 0, sizeof world->entities);
                        for (int i = 0; i < MAX_ENTITIES; i++) {
                                world->entities[i].index = i;
                                world->entities[i].collect = true;
                        }
                }
        }

        free(saved);
        free(occupied);

        if (!ok) {
                snprintf(error, error_size, "Invalid save file %s", path);
                return false;
        }

        return true;
}