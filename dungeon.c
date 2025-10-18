#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "dungeon.h"
#include "enemies.h"
#include "player.h"

void print_status(int hp, int gold)
{
    printf("HP : %d, Gold: %d\n", hp, gold);
}

char read_command(void)
{
    char command;
    printf("Enter command (N/S/E/W to move, Q to quit): ");
    if (scanf(" %c", &command) != 1) {
        return 'Q';
    }
    command = (char)toupper((unsigned char)command);
    printf("You entered command: %c\n", command);
    return command;
}

void search_room(Player *player, int *current_room)
{
    int event = rand() % 8;
    switch (event)
    {
    case 0: {
        int gold = 10 + rand() % 41; // 10..50
        player->gold += gold;
        printf("You found a hidden chest with %d gold!\n", gold);
        break;
    }
    case 1: {
        int dmg = 5 + rand() % 16; // 5..20
        player->health -= dmg;
        if (player->health < 0) player->health = 0;
        printf("A trap is triggered! You take %d damage.\n", dmg);
        break;
    }
    case 2:
        printf("You encounter a person. They nod silently and pass by.\n");
        break;
    case 3: {
        int heal = 10 + rand() % 21; // 10..30
        player->health += heal;
        if (player->health > player->max_health) player->health = player->max_health;
        printf("You find a healing potion and recover %d HP. HP now %d.\n", heal, player->health);
        break;
    }
    case 4: {
        printf("A monster appears! Prepare for battle.\n");
        int loot = battle_monster(player);
        if (player->health > 0) {
            player->gold += loot;
            printf("You loot %d gold from the corpse.\n", loot);
        }
        break;
    }
    case 5:
        (*current_room)++;
        printf("A secret passage leads you to room %d.\n", *current_room);
        break;
    case 6:
        printf("You find nothing of interest.\n");
        break;
    case 7: {
        int gold = 100 + rand() % 51; // 100..150
        player->gold += gold;
        printf("An ancient artifact! You sell it for %d gold.\n", gold);
        break;
    }
    }
}

void handle_command(char command, int *running, int *current_room, Player *player)
{
    switch (command)
    {
    case 'Q':
        *running = 0;
        printf("Quitting the game. Thanks for playing!\n");
        break;
    case 'N':
        (*current_room)++;
        printf("You move north to room %d.\n", *current_room);
        search_room(player, current_room);
        break;
    case 'S':
        *current_room = (*current_room > 1) ? *current_room - 1 : 1;
        printf("You move south to room %d.\n", *current_room);
        search_room(player, current_room);
        break;
    case 'E':
        printf("You move east.\n");
        search_room(player, current_room);
        break;
    case 'W':
        printf("You move west.\n");
        search_room(player, current_room);
        break;
    default:
        printf("Invalid command. Please try again.\n");
        break;
    }
}