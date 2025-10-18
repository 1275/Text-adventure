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

    int current_room = 1;
    int running = 1;

    printf("=== Welcome to the Dungeon Crawler! ===\n");
    printf("Tips: Use N/S/E/W to move, Q to quit.\n");
    player_print_status(&player);
    player_print_inventory(&player);

    while (running)
    {
        player_print_status(&player);
        char command = read_command();
        handle_command(command, &running, &current_room, &player);

        if (player.health <= 0)
        {
            printf("You have perished in the dungeon. Game Over!\n");
            running = 0;
        }
    }

    return 0;
}