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
    
    // Reset visited array for player exploration tracking
    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            map->visited[y][x] = 0;
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

void search_room(Player *player, Position *pos, char *message, Map *map, BattleState *battle)
{
    // Check if this room has already been visited
    if (map->visited[pos->y][pos->x]) {
        snprintf(message, 256, "This area has already been explored. Nothing new here.");
        return;
    }
    
    // Mark room as visited
    map->visited[pos->y][pos->x] = 1;
    
    int special = is_special_location(pos);
    
    // Special boss locations at corners
    if (special == 1) {
        snprintf(message, 256, "*** BOSS LAIR! An Ancient Dragon appears! ***");
        battle->is_active = 1;
        battle->monster = (Monster){"Ancient Dragon", 250, 30, 12, 150, 300, 500};
        battle->monster_hp = battle->monster.hp;
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
    int difficulty = dist / 5;
    
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
        // Start a battle!
        static const Monster monsters[] = {
            {"Goblin",       30,  7, 1,  8, 20,  25},
            {"Skeleton",     40,  9, 2, 12, 26,  35},
            {"Giant Spider", 50, 11, 3, 15, 30,  45},
            {"Orc",          60, 13, 4, 20, 35,  60},
            {"Troll",        80, 16, 5, 30, 45,  80},
            {"Dark Knight", 100, 20, 7, 40, 60, 120},
        };
        int mcount = 6;
        int idx = rand() % mcount;
        
        // Scale monster with difficulty
        Monster m = monsters[idx];
        m.hp += difficulty * 10;
        m.attack += difficulty;
        m.defense += difficulty / 2;
        
        battle->is_active = 1;
        battle->monster = m;
        battle->monster_hp = m.hp;
        snprintf(message, 256, "A %s appears! Prepare for battle!", m.name);
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

// Handle battle commands
static void handle_battle_command(char command, Player *player, BattleState *battle, char *message, GameState *state) {
    command = (char)toupper((unsigned char)command);
    
    switch (command) {
    case 'A': {
        // Player attacks
        int p_roll = rand() % 6;
        int p_attack = player->total_damage + p_roll;
        int dmg_to_mon = p_attack - battle->monster.defense;
        if (dmg_to_mon < 1) dmg_to_mon = 1;
        
        battle->monster_hp -= dmg_to_mon;
        if (battle->monster_hp < 0) battle->monster_hp = 0;
        
        snprintf(message, 256, "You hit the %s for %d damage!", battle->monster.name, dmg_to_mon);
        
        // Check if monster died
        if (battle->monster_hp <= 0) {
            int loot = battle->monster.min_loot + (rand() % (battle->monster.max_loot - battle->monster.min_loot + 1));
            player->gold += loot;
            player_gain_exp(player, battle->monster.exp_reward);
            
            char temp[256];
            snprintf(temp, 256, "%s Victory! Defeated %s! Gained %d gold and %d XP.", 
                    message, battle->monster.name, loot, battle->monster.exp_reward);
            strcpy(message, temp);
            
            // Random item drops (15% chance)
            if (rand() % 100 < 15) {
                Item drop;
                int drop_type = rand() % 100;
                
                if (drop_type < 40) {
                    drop = (Item){10, ITEM_CONSUMABLE, "Health Potion", 1, {0, 0}, 10};
                } else if (drop_type < 70) {
                    const char *weapon_names[] = {"Iron Sword", "Steel Axe", "War Hammer", "Enchanted Blade"};
                    int wpn_idx = rand() % 4;
                    int dmg_bonus = 8 + rand() % 10;
                    drop = (Item){20 + wpn_idx, ITEM_WEAPON, weapon_names[wpn_idx], 1, {dmg_bonus, 0}, 20 + dmg_bonus * 2};
                } else {
                    const char *armor_names[] = {"Leather Armor", "Chain Mail", "Plate Armor", "Dragon Scale"};
                    int arm_idx = rand() % 4;
                    int def_bonus = 4 + rand() % 8;
                    drop = (Item){30 + arm_idx, ITEM_ARMOR, armor_names[arm_idx], 1, {0, def_bonus}, 15 + def_bonus * 2};
                }
                
                player_add_item(player, &drop);
                char temp2[256];
                snprintf(temp2, 256, "%s Also received %s!", message, drop.name);
                strcpy(message, temp2);
            }
            
            battle->is_active = 0;
            *state = STATE_EXPLORING;
            return;
        }
        
        // Monster counterattacks
        int m_roll = rand() % 4;
        int m_attack = battle->monster.attack + m_roll;
        int dmg_to_player = m_attack - player->total_defense;
        if (dmg_to_player < 1) dmg_to_player = 1;
        
        player->health -= dmg_to_player;
        if (player->health < 0) player->health = 0;
        
        char temp[256];
        snprintf(temp, 256, "%s The %s counters for %d damage!", 
                message, battle->monster.name, dmg_to_player);
        strcpy(message, temp);
        break;
    }
    
    case 'Q': {
        if (rand() % 100 < 30) {  // 30% flee chance
            snprintf(message, 256, "You successfully fled from the %s!", battle->monster.name);
            battle->is_active = 0;
            *state = STATE_EXPLORING;
        } else {
            // Failed flee - monster gets free hit
            int m_roll = rand() % 4;
            int m_attack = battle->monster.attack + m_roll;
            int dmg_to_player = m_attack - player->total_defense;
            if (dmg_to_player < 1) dmg_to_player = 1;
            
            player->health -= dmg_to_player;
            if (player->health < 0) player->health = 0;
            
            snprintf(message, 256, "Failed to flee! The %s punishes you for %d damage!", 
                    battle->monster.name, dmg_to_player);
        }
        break;
    }
    
    case 'I': {
        // TODO: Implement item use in battle
        snprintf(message, 256, "Item use in battle - coming soon!");
        break;
    }
    
    default:
        snprintf(message, 256, "Invalid command! Use A to attack, I for item, Q to flee.");
        break;
    }
}

void handle_command(char command, int *running, Position *pos, Player *player, char *message, Map *map, GameState *state, BattleState *battle)
{
    // If in battle, handle battle commands
    if (*state == STATE_BATTLE) {
        handle_battle_command(command, player, battle, message, state);
        return;
    }
    
    // Regular exploration commands
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
        search_room(player, pos, message, map, battle);
        
        // If a battle started, switch to battle state
        if (battle->is_active) {
            *state = STATE_BATTLE;
        }
    }
}
