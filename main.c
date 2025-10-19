/*
 * main.c - Dungeon Crawler Adventure
 * 
 * This is the entry point for the game. It initializes the game state,
 * generates the dungeon map, and runs the main game loop.
 * 
 * Game Flow:
 * 1. Initialize random number generator (for procedural generation and events)
 * 2. Create player character with starting stats and equipment
 * 3. Generate the procedural dungeon maze
 * 4. Start at center of map (spawn point)
 * 5. Main loop: read commands, update game state, render display
 * 6. End when player quits or dies
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dungeon.h"
#include "player.h"
#include "ui.h"

int main(void)
{
    /* Initialize random number generator with current time as seed
     * This ensures different dungeon layouts and events each playthrough */
    srand((unsigned int)time(NULL));

    /* Create and initialize the player character
     * Sets starting HP, stats, level, and gives starter equipment */
    Player player;
    player_init(&player);
    
    /* Generate the procedural dungeon map
     * Creates a 50x50 maze with rooms, corridors, and special locations */
    Map map;
    map_generate(&map);

    /* Set starting position at the center of the map (25, 25)
     * This is the spawn point marked with '+' on the map */
    Position pos = {MAP_CENTER, MAP_CENTER};
    
    /* Game running flag - set to 0 to exit the game loop */
    int running = 1;
    
    /* Message buffer for displaying events to the player
     * Shows what happened in the current room or from last action */
    char message[256] = "Whoa! You trigger a magical portal and find yourself in a mysterious dungeon...";

    /* Display the initial game screen */
    ui_render_game(&player, &pos, message, &map);

    /* Main game loop - continues until player quits or dies */
    while (running)
    {
        /* Read a single character command from the player
         * Commands: N/S/E/W (move), I (inventory), M (map), Q (quit) */
        char command;
        if (scanf(" %c", &command) != 1) {
            /* Input error - exit gracefully */
            break;
        }
        
        /* Process the command and update game state
         * This may move the player, trigger events, or change the running flag */
        handle_command(command, &running, &pos, &player, message, &map);
        
        /* Re-render the screen with updated information */
        if (running) {
            ui_render_game(&player, &pos, message, &map);
        }

        /* Check if player has died (health reached 0) */
        if (player.health <= 0)
        {
            /* Display game over screen with final statistics */
            ui_clear_screen();
            printf("\n╔════════════════════════════════════════╗\n");
            printf("║         GAME OVER                      ║\n");
            printf("╚════════════════════════════════════════╝\n\n");
            printf("You have perished in the dungeon.\n");
            printf("Final Level: %d\n", player.level);
            printf("Gold Collected: %d\n", player.gold);
            printf("Final Position: [%d, %d]\n\n", pos.x, pos.y);
            running = 0;  /* End the game loop */
        }
    }

    /* Restore cursor visibility before exiting */
    ui_show_cursor();
    return 0;
}