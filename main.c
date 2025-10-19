#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dungeon.h"
#include "player.h"
#include "ui.h"

int main(void)
{
    srand((unsigned int)time(NULL));

    Player player;
    player_init(&player);

    Position pos = {MAP_CENTER, MAP_CENTER};
    int running = 1;
    char message[256] = "Welcome to the Dungeon! You are at the center of the map.";

    // Initial render
    ui_render_game(&player, &pos, message);

    while (running)
    {
        char command;
        if (scanf(" %c", &command) != 1) {
            break;
        }
        
        // Handle command (this will modify player and pos)
        handle_command(command, &running, &pos, &player, message);
        
        // Re-render the screen
        if (running) {
            ui_render_game(&player, &pos, message);
        }

        if (player.health <= 0)
        {
            ui_clear_screen();
            printf("\n╔════════════════════════════════════════╗\n");
            printf("║         GAME OVER                      ║\n");
            printf("╚════════════════════════════════════════╝\n\n");
            printf("You have perished in the dungeon.\n");
            printf("Final Level: %d\n", player.level);
            printf("Gold Collected: %d\n", player.gold);
            printf("Final Position: [%d, %d]\n\n", pos.x, pos.y);
            running = 0;
        }
    }

    ui_show_cursor();
    return 0;
}