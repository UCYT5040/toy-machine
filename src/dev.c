#include "dev.h"
#include "emoji_registry.h"
#include "entities.h"

void run_dev_command(World *world, char *command, char *output_buffer,
                     size_t output_buffer_size) {
        char *command_name = strtok(command, " \t\n");
        if (command_name == NULL) {
                snprintf(output_buffer, output_buffer_size,
                         "No command entered.");
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
                        if (strcmp(emoji_name, EMOJI_DEFINITIONS[i].name) ==
                            0) {
                                char *x_str = strtok(NULL, " \t\n");
                                char *y_str = strtok(NULL, " \t\n");
                                if (x_str == NULL || y_str == NULL) {
                                        snprintf(output_buffer,
                                                 output_buffer_size,
                                                 "Usage: spawn <emoji_name> "
                                                 "<x> <y>");
                                        return;
                                }

                                double x = strtof(x_str, NULL);
                                double y = strtof(y_str, NULL);

                                // Find an inactive entity slot
                                for (int j = 0; j < MAX_ENTITIES; j++) {
                                        Entity *entity = &world->entities[j];
                                        if (!entity->active) {
                                                entity_create_default(
                                                    entity, i, (Vec2){x, y}, j);

                                                snprintf(output_buffer,
                                                         output_buffer_size,
                                                         "Spawned %s at (%.2f, "
                                                         "%.2f)",
                                                         emoji_name, x, y);
                                                return;
                                        }
                                }

                                snprintf(output_buffer, output_buffer_size,
                                         "No available slots to spawn new "
                                         "entities.");
                                return;
                        }
                }
        } else if (strcmp(command_name, "speed") == 0) {
                char *speed_str = strtok(NULL, " \t\n");
                if (speed_str == NULL) {
                        snprintf(
                            output_buffer, output_buffer_size,
                            "Usage: speed <value> (e.g., 0.5 for half speed)");
                        return;
                }

                double speed = strtof(speed_str, NULL);

                // Accept `0.0`, since that can be used to freeze the game
                if (speed < 0.0) {
                        snprintf(output_buffer, output_buffer_size,
                                 "Speed must be non-negative");
                }

                snprintf(output_buffer, output_buffer_size, "Speed set to %.2f",
                         speed);
        }
}