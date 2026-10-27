#include "dev.h"

void run_dev_command(World *world, char *command, char *output_buffer,
                     size_t output_buffer_size) {
  char *command_name = strtok(command, " \t\n");
  if (command_name == NULL) {
    snprintf(output_buffer, output_buffer_size, "No command entered.");
    return;
  }

  if (strcmp(command_name, "spawn") == 0) {
    char *emoji_name = strtok(NULL, " \t\n");
    if (emoji_name == NULL) {
      snprintf(output_buffer, output_buffer_size,
               "Usage: spawn <emoji_name> <x> <y>");
      return;
    }

    for (int i = 1; i < EMOJI_COUNT; i++) {
      if (strcmp(emoji_name, EMOJI_DEFINITIONS[i].name) == 0) {
        char *x_str = strtok(NULL, " \t\n");
        char *y_str = strtok(NULL, " \t\n");
        if (x_str == NULL || y_str == NULL) {
          snprintf(output_buffer, output_buffer_size,
                   "Usage: spawn <emoji_name> <x> <y>");
          return;
        }

        float x = strtof(x_str, NULL);
        float y = strtof(y_str, NULL);

        // Find an inactive entity slot
        for (int j = 0; j < MAX_ENTITIES; j++) {
          Entity *entity = &world->entities[j];
          if (!entity->active) {
            entity->active = true;
            entity->definition = &EMOJI_DEFINITIONS[i];
            entity->type = (EmojiType)i;
            entity->position.x = x;
            entity->position.y = y;
            entity->velocity.x = 0.0f;
            entity->velocity.y = 0.0f;
            entity->force.x = 0.0f;
            entity->force.y = 0.0f;
            entity->angle = 0.0f;
            entity->inverse_mass = (entity->definition->mass > 0.0f)
                                       ? 1.0f / entity->definition->mass
                                       : 0.0f; // Handle immovable objects

            snprintf(output_buffer, output_buffer_size,
                     "Spawned %s at (%.2f, %.2f)", emoji_name, x, y);
            return;
          }
        }

        snprintf(output_buffer, output_buffer_size,
                 "No available slots to spawn new entities.");
        return;
      }
    }
  }
}