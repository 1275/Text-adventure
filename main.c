#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dungeon.h"
#include "player.h"
#include "ui.h"

int main(void)
{
    srand((unsigned int)time(NULL)); // Seed random number generator

    Player player; // Initialize player
    player_init(&player);  // Set starting stats and inventory
    
    Map map;
    map_generate(&map);

    Position pos = {MAP_CENTER, MAP_CENTER};
    GameState state = STATE_EXPLORING;
    BattleState battle = {0};
    battle.is_active = 0;
    
    int running = 1;
    char message[256] = "Whoa! You trigger a magical portal and find yourself in a mysterious dungeon...";

    ui_render_game(&player, &pos, message, &map);

    while (running)
    {
        char command;
        if (scanf(" %c", &command) != 1) {
            break;
        }
        
        handle_command(command, &running, &pos, &player, message, &map, &state, &battle);
        
        if (running) {
            if (state == STATE_BATTLE) {
                ui_render_battle(&player, &battle, message);
            } else {
                ui_render_game(&player, &pos, message, &map);
            }
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