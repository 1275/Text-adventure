#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "dungeon.h"
#include "enemies.h"
#include "player.h"

// Direction arrays for maze generation
static const int dx[] = {0, 1, 0, -1};
static const int dy[] = {-1, 0, 1, 0};

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

// Recursive backtracking maze generation
static void carve_maze(Map *map, int x, int y) {
    map->visited[y][x] = 1;
    map->tiles[y][x] = TILE_FLOOR;
    
    // Create array of directions and shuffle them
    int dirs[4] = {0, 1, 2, 3};
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = dirs[i];
        dirs[i] = dirs[j];
        dirs[j] = temp;
    }
    
    // Try each direction
    for (int i = 0; i < 4; i++) {
        int dir = dirs[i];
        int nx = x + dx[dir] * 2;  // Move 2 cells at a time
        int ny = y + dy[dir] * 2;
        
        // Check if valid and unvisited
        if (nx >= 0 && nx < MAP_SIZE && ny >= 0 && ny < MAP_SIZE && 
            !map->visited[ny][nx]) {
            // Carve the corridor between current and next cell
            int mx = x + dx[dir];
            int my = y + dy[dir];
            map->tiles[my][mx] = TILE_CORRIDOR;
            
            // Recursively carve from the new cell
            carve_maze(map, nx, ny);
        }
    }
}

// Generate procedural maze
void map_generate(Map *map) {
    // Initialize all to walls
    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            map->tiles[y][x] = TILE_WALL;
            map->visited[y][x] = 0;
        }
    }
    
    // Start maze generation from center
    carve_maze(map, MAP_CENTER, MAP_CENTER);
    
    // Ensure special locations are accessible
    map->tiles[0][0] = TILE_FLOOR;  // Top-left boss
    map->tiles[0][MAP_SIZE - 1] = TILE_FLOOR;  // Bottom-left boss
    map->tiles[MAP_SIZE - 1][0] = TILE_FLOOR;  // Top-right boss
    map->tiles[MAP_SIZE - 1][MAP_SIZE - 1] = TILE_FLOOR;  // Bottom-right boss
    
    map->tiles[0][MAP_CENTER] = TILE_FLOOR;  // Left shrine
    map->tiles[MAP_SIZE - 1][MAP_CENTER] = TILE_FLOOR;  // Right shrine
    map->tiles[MAP_CENTER][0] = TILE_FLOOR;  // Top shrine
    map->tiles[MAP_CENTER][MAP_SIZE - 1] = TILE_FLOOR;  // Bottom shrine
    
    // Add some random connections to make maze less linear (20% chance)
    for (int y = 1; y < MAP_SIZE - 1; y++) {
        for (int x = 1; x < MAP_SIZE - 1; x++) {
            if (map->tiles[y][x] == TILE_WALL && rand() % 100 < 20) {
                map->tiles[y][x] = TILE_CORRIDOR;
            }
        }
    }
}

// Check if position is walkable
int map_can_move(const Map *map, int x, int y) {
    if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE) {
        return 0;
    }
    return map->tiles[y][x] != TILE_WALL;
}

// Get tile type at position
TileType map_get_tile(const Map *map, int x, int y) {
    if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE) {
        return TILE_WALL;
    }
    return map->tiles[y][x];
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

void handle_command(char command, int *running, Position *pos, Player *player, char *message, Map *map)
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
        snprintf(message, 256, "Full map view - press any key to continue");
        return;
    case 'I':
        snprintf(message, 256, "Inventory screen - press any key to continue");
        return;
    case 'N':
        new_pos.y--;
        if (map_can_move(map, new_pos.x, new_pos.y)) {
            moved = 1;
        } else {
            snprintf(message, 256, "Cannot go north - there's a wall!");
        }
        break;
    case 'S':
        new_pos.y++;
        if (map_can_move(map, new_pos.x, new_pos.y)) {
            moved = 1;
        } else {
            snprintf(message, 256, "Cannot go south - there's a wall!");
        }
        break;
    case 'E':
        new_pos.x++;
        if (map_can_move(map, new_pos.x, new_pos.y)) {
            moved = 1;
        } else {
            snprintf(message, 256, "Cannot go east - there's a wall!");
        }
        break;
    case 'W':
        new_pos.x--;
        if (map_can_move(map, new_pos.x, new_pos.y)) {
            moved = 1;
        } else {
            snprintf(message, 256, "Cannot go west - there's a wall!");
        }
        break;
    default:
        snprintf(message, 256, "Invalid command. Use N/S/E/W to move, I for inventory, M for map, Q to quit.");
        return;
    }
    
    if (moved) {
        *pos = new_pos;
        search_room(player, pos, message);
    }
}
