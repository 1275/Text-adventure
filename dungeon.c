#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
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

void search_room(Player *player, Position *pos, char *message)
{
    int special = is_special_location(pos);
    
    // Special boss locations at corners
    if (special == 1) {
        snprintf(message, 256, "*** BOSS LAIR! An Ancient Dragon guards treasure! ***");
        
        // Create a powerful boss
        Monster boss = {"Ancient Dragon", 250, 30, 12, 150, 300, 500};
        int mhp = boss.hp;
        
        // Note: Battle system needs to be refactored for message-based output
        // For now, just simulate the battle result
        // TODO: Refactor battle system to work with UI
        
        player->gold += 250;
        player_gain_exp(player, boss.exp_reward);
        
        Item legendary = (Item){100, ITEM_WEAPON, "Legendary Dragonslayer", 1, {25, 5}, 500};
        player_add_item(player, &legendary);
        
        snprintf(message, 256, "Victory! Defeated Ancient Dragon! +250 gold, legendary item!");
        return;
    }
    
    // Special shrine locations
    if (special == 2) {
        int choice = rand() % 3;
        if (choice == 0) {
            int heal = 50 + rand() % 50;
            player->health += heal;
            if (player->health > player->max_health) player->health = player->max_health;
            snprintf(message, 256, "Found Ancient Shrine! Restored %d HP.", heal);
        } else if (choice == 1) {
            int gold = 75 + rand() % 75;
            player->gold += gold;
            snprintf(message, 256, "Found Ancient Shrine! Received %d gold in offerings.", gold);
        } else {
            int exp = 50 + rand() % 100;
            player_gain_exp(player, exp);
            snprintf(message, 256, "Found Ancient Shrine! Gained wisdom (+%d XP).", exp);
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
        snprintf(message, 256, "Trap triggered! Took %d damage.", dmg);
    }
    else if (event_roll < monster_threshold) {
        // TODO: Battle system needs refactoring for UI
        // For now, simulate a quick battle
        int loot = 10 + rand() % 30;
        player->gold += loot;
        snprintf(message, 256, "Monster appeared and defeated! Looted %d gold.", loot);
    }
    else if (event_roll < treasure_threshold) {
        int gold = 10 + rand() % 41 + difficulty * 5;
        player->gold += gold;
        snprintf(message, 256, "Found hidden chest with %d gold!", gold);
    }
    else if (event_roll < treasure_threshold + 15) {
        int heal = 10 + rand() % 21;
        player->health += heal;
        if (player->health > player->max_health) player->health = player->max_health;
        snprintf(message, 256, "Found healing potion! Recovered %d HP.", heal);
    }
    else if (event_roll < treasure_threshold + 20) {
        int gold = 100 + rand() % 51 + difficulty * 10;
        player->gold += gold;
        snprintf(message, 256, "Ancient artifact found! Sold for %d gold.", gold);
    }
    else if (event_roll < treasure_threshold + 25) {
        snprintf(message, 256, "Encountered wandering merchant. They nod and pass by.");
    }
    else {
        snprintf(message, 256, "Nothing of interest found in this area.");
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

void handle_command(char command, int *running, Position *pos, Player *player, char *message)
{
    int moved = 0;
    Position new_pos = *pos;
    
    command = (char)toupper((unsigned char)command);
    
    switch (command)
    {
    case 'Q':
        *running = 0;
        snprintf(message, 256, "Quitting the game. Thanks for playing!");
        return;
    case 'M':
        // TODO: Implement full map view in UI
        snprintf(message, 256, "Full map view - press any key to continue");
        return;
    case 'I':
        // TODO: Implement inventory screen in UI
        snprintf(message, 256, "Inventory screen - press any key to continue");
        return;
    case 'N':
        if (pos->y > 0) {
            new_pos.y--;
            moved = 1;
        } else {
            snprintf(message, 256, "Cannot go north - edge of the world!");
        }
        break;
    case 'S':
        if (pos->y < MAP_SIZE - 1) {
            new_pos.y++;
            moved = 1;
        } else {
            snprintf(message, 256, "Cannot go south - edge of the world!");
        }
        break;
    case 'E':
        if (pos->x < MAP_SIZE - 1) {
            new_pos.x++;
            moved = 1;
        } else {
            snprintf(message, 256, "Cannot go east - edge of the world!");
        }
        break;
    case 'W':
        if (pos->x > 0) {
            new_pos.x--;
            moved = 1;
        } else {
            snprintf(message, 256, "Cannot go west - edge of the world!");
        }
        break;
    default:
        snprintf(message, 256, "Invalid command. Use N/S/E/W to move, I for inventory, M for map, Q to quit.");
        return;
    }
    
    if (moved) {
        *pos = new_pos;
        int dist = distance_from_center(pos);
        char temp_msg[256];
        snprintf(temp_msg, 256, "Moved to [%d, %d] (distance from center: %d). ", 
               pos->x, pos->y, dist);
        strcpy(message, temp_msg);
        
        char event_msg[256];
        search_room(player, pos, event_msg);
        strncat(message, event_msg, 256 - strlen(message) - 1);
    }
}