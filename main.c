#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dungeon.h"
#include "player.h"

int main(void)
{
    srand((unsigned int)time(NULL));

    Player player;
    player_init(&player);

    Position pos = {MAP_CENTER, MAP_CENTER}; // Start at center of map
    int running = 1;

    printf("=== Welcome to the Dungeon Crawler! ===\n");
    printf("You find yourself at the center of a vast %dx%d dungeon.\n", MAP_SIZE, MAP_SIZE);
    printf("Tips: Use N/S/E/W to move, M to view map, Q to quit.\n");
    printf("Starting position: [%d, %d]\n\n", pos.x, pos.y);
    player_print_status(&player);
    player_print_inventory(&player);

    while (running)
    {
        player_print_status(&player);
        char command = read_command();
        handle_command(command, &running, &pos, &player);

        if (player.health <= 0)
        {
            printf("You have perished in the dungeon. Game Over!\n");
            printf("Final Level: %d, Gold: %d, Position: [%d, %d]\n", 
                   player.level, player.gold, pos.x, pos.y);
            running = 0;
        }
    }

    return 0;
}