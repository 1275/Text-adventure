#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dungeon.h"

int main(void)
{
    srand((unsigned int)time(NULL)); // seed random number generator

    int player_health = 100;
    int player_gold = 0;
    int current_room = 1;
    int running = 1;

    printf("===Welcome to the Dungeon Crawler!===\n");

    while (running)
    {
        print_status(player_health, player_gold);
        char command = read_command();
        handle_command(command, &running, &current_room, &player_health, &player_gold);

        if (player_health <= 0)
        {
            printf("You have perished in the dungeon. Game Over!\n");
            running = 0;
        }
    }

    return 0;
}