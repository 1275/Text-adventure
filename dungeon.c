#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "dungeon.h"
#include "enemies.h"
#include "player.h"

// Calculate distance from center (difficulty scaling)
static int distance_from_center(const Position *pos) {
    int dx = pos->x - MAP_CENTER;
    int dy = pos->y - MAP_CENTER;
    return (int)sqrt(dx * dx + dy * dy);
}

// Check if position is a special location
static int is_special_location(const Position *pos) {
    // Corners of the map
    if ((pos->x == 0 && pos->y == 0) ||
        (pos->x == 0 && pos->y == MAP_SIZE - 1) ||
        (pos->x == MAP_SIZE - 1 && pos->y == 0) ||
        (pos->x == MAP_SIZE - 1 && pos->y == MAP_SIZE - 1)) {
        return 1; // Boss location
    }
    
    // Special locations at cardinal directions from center
    if ((pos->x == MAP_CENTER && pos->y == 0) ||
        (pos->x == MAP_CENTER && pos->y == MAP_SIZE - 1) ||
        (pos->x == 0 && pos->y == MAP_CENTER) ||
        (pos->x == MAP_SIZE - 1 && pos->y == MAP_CENTER)) {
        return 2; // Shrine location
    }
    
    return 0;
}

void print_status(int hp, int gold)
{
    printf("HP : %d, Gold: %d\n", hp, gold);
}

char read_command(void)
{
    char command;
    printf("Enter command (N/S/E/W to move, M for map, Q to quit): ");
    if (scanf(" %c", &command) != 1) {
        return 'Q';
    }
    command = (char)toupper((unsigned char)command);
    printf("You entered command: %c\n", command);
    return command;
}

void search_room(Player *player, Position *pos)
{
    int special = is_special_location(pos);
    
    // Special boss locations at corners
    if (special == 1) {
        printf("\n*** You've discovered a BOSS LAIR! ***\n");
        printf("A massive Ancient Dragon guards a treasure hoard!\n");
        
        // Create a powerful boss
        Monster boss = {"Ancient Dragon", 250, 30, 12, 150, 300, 500};
        int mhp = boss.hp;
        printf("The %s roars with %d HP!\n", boss.name, mhp);
        
        while (mhp > 0 && player->health > 0) {
            int p_roll = rand() % 8;
            int m_roll = rand() % 6;
            
            int p_attack = player->total_damage + p_roll;
            int m_attack = boss.attack + m_roll;
            
            int dmg_to_mon = p_attack - boss.defense;
            if (dmg_to_mon < 1) dmg_to_mon = 1;
            
            mhp -= dmg_to_mon;
            if (mhp < 0) mhp = 0;
            printf("You strike the %s for %d damage! Boss HP: %d\n", boss.name, dmg_to_mon, mhp);
            
            if (mhp <= 0) {
                printf("\n*** VICTORY! You have slain the %s! ***\n", boss.name);
                int loot = boss.min_loot + (rand() % (boss.max_loot - boss.min_loot + 1));
                player->gold += loot;
                printf("You claim %d gold from the hoard!\n", loot);
                player_gain_exp(player, boss.exp_reward);
                
                // Boss always drops legendary item
                Item legendary = (Item){100, ITEM_WEAPON, "Legendary Dragonslayer", 1, {25, 5}, 500};
                player_add_item(player, &legendary);
                return;
            }
            
            int dmg_to_player = m_attack - player->total_defense;
            if (dmg_to_player < 1) dmg_to_player = 1;
            
            player->health -= dmg_to_player;
            if (player->health < 0) player->health = 0;
            printf("The %s breathes fire, dealing %d damage! Your HP: %d\n", boss.name, dmg_to_player, player->health);
        }
        return;
    }
    
    // Special shrine locations
    if (special == 2) {
        printf("\n*** You've found an Ancient Shrine! ***\n");
        int choice = rand() % 3;
        if (choice == 0) {
            int heal = 50 + rand() % 50;
            player->health += heal;
            if (player->health > player->max_health) player->health = player->max_health;
            printf("The shrine's holy light restores %d HP!\n", heal);
        } else if (choice == 1) {
            int gold = 75 + rand() % 75;
            player->gold += gold;
            printf("You find %d gold in offerings at the shrine!\n", gold);
        } else {
            int exp = 50 + rand() % 100;
            printf("You pray at the shrine and gain wisdom!\n");
            player_gain_exp(player, exp);
        }
        return;
    }
    
    int dist = distance_from_center(pos);
    int difficulty = dist / 5; // Difficulty increases every 5 tiles from center
    
    // More dangerous events further from center
    int event_roll = rand() % 100;
    int trap_threshold = 10 + difficulty * 2;
    int monster_threshold = trap_threshold + 20 + difficulty * 3;
    int treasure_threshold = monster_threshold + 15;
    
    if (event_roll < trap_threshold) {
        int dmg = 5 + rand() % 16 + difficulty * 2;
        player->health -= dmg;
        if (player->health < 0) player->health = 0;
        printf("A trap is triggered! You take %d damage.\n", dmg);
    }
    else if (event_roll < monster_threshold) {
        printf("A monster appears! Prepare for battle.\n");
        int loot = battle_monster(player);
        if (player->health > 0) {
            player->gold += loot;
            printf("You loot %d gold from the corpse.\n", loot);
        }
    }
    else if (event_roll < treasure_threshold) {
        int gold = 10 + rand() % 41 + difficulty * 5;
        player->gold += gold;
        printf("You found a hidden chest with %d gold!\n", gold);
    }
    else if (event_roll < treasure_threshold + 15) {
        int heal = 10 + rand() % 21;
        player->health += heal;
        if (player->health > player->max_health) player->health = player->max_health;
        printf("You find a healing potion and recover %d HP. HP now %d.\n", heal, player->health);
    }
    else if (event_roll < treasure_threshold + 20) {
        int gold = 100 + rand() % 51 + difficulty * 10;
        player->gold += gold;
        printf("An ancient artifact! You sell it for %d gold.\n", gold);
    }
    else if (event_roll < treasure_threshold + 25) {
        printf("You encounter a wandering merchant. They nod and pass by.\n");
    }
    else {
        printf("You find nothing of interest in this area.\n");
    }
}

void print_map(const Position *pos) {
    printf("\n=== MAP (you are at [%d, %d]) ===\n", pos->x, pos->y);
    
    // Show a 15x15 area around the player
    int view_range = 7;
    int min_x = pos->x - view_range;
    int max_x = pos->x + view_range;
    int min_y = pos->y - view_range;
    int max_y = pos->y + view_range;
    
    // Clamp to map boundaries
    if (min_x < 0) min_x = 0;
    if (max_x >= MAP_SIZE) max_x = MAP_SIZE - 1;
    if (min_y < 0) min_y = 0;
    if (max_y >= MAP_SIZE) max_y = MAP_SIZE - 1;
    
    // Print column headers
    printf("   ");
    for (int x = min_x; x <= max_x; x++) {
        printf("%2d", x % 10);
    }
    printf("\n");
    
    for (int y = min_y; y <= max_y; y++) {
        printf("%2d ", y);
        for (int x = min_x; x <= max_x; x++) {
            if (x == pos->x && y == pos->y) {
                printf(" @");  // Player position
            } else if (x == MAP_CENTER && y == MAP_CENTER) {
                printf(" +");  // Spawn point
            } else {
                Position check = {x, y};
                int special = is_special_location(&check);
                if (special == 1) {
                    printf(" B");  // Boss location
                } else if (special == 2) {
                    printf(" S");  // Shrine location
                } else {
                    printf(" .");  // Empty
                }
            }
        }
        printf("\n");
    }
    printf("Legend: @ = You, + = Spawn, B = Boss, S = Shrine, . = Unexplored\n");
    printf("Map size: %dx%d\n\n", MAP_SIZE, MAP_SIZE);
}

void handle_command(char command, int *running, Position *pos, Player *player)
{
    int moved = 0;
    Position new_pos = *pos;
    
    switch (command)
    {
    case 'Q':
        *running = 0;
        printf("Quitting the game. Thanks for playing!\n");
        return;
    case 'M':
        print_map(pos);
        return;
    case 'N':
        if (pos->y > 0) {
            new_pos.y--;
            moved = 1;
        } else {
            printf("You cannot go north - you've reached the edge of the world!\n");
        }
        break;
    case 'S':
        if (pos->y < MAP_SIZE - 1) {
            new_pos.y++;
            moved = 1;
        } else {
            printf("You cannot go south - you've reached the edge of the world!\n");
        }
        break;
    case 'E':
        if (pos->x < MAP_SIZE - 1) {
            new_pos.x++;
            moved = 1;
        } else {
            printf("You cannot go east - you've reached the edge of the world!\n");
        }
        break;
    case 'W':
        if (pos->x > 0) {
            new_pos.x--;
            moved = 1;
        } else {
            printf("You cannot go west - you've reached the edge of the world!\n");
        }
        break;
    default:
        printf("Invalid command. Please try again.\n");
        return;
    }
    
    if (moved) {
        *pos = new_pos;
        int dist = distance_from_center(pos);
        printf("You move to position [%d, %d] (distance from center: %d)\n", 
               pos->x, pos->y, dist);
        search_room(player, pos);
    }
}