/*
 * dungeon.c - Dungeon Map Generation and Event System
 * 
 * This file handles:
 * - Procedural maze generation using recursive backtracking
 * - Movement validation and position tracking
 * - Random events (combat, traps, treasure, healing)
 * - Special locations (boss lairs, ancient shrines)
 * - Difficulty scaling based on distance from spawn point
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "dungeon.h"
#include "enemies.h"
#include "player.h"

/*
 * Direction arrays for maze generation
 * Used to move in four cardinal directions
 * Index 0=North, 1=East, 2=South, 3=West
 * dx[i] gives the X offset, dy[i] gives the Y offset
 */
static const int dx[] = {0, 1, 0, -1};  /* X offsets for each direction */
static const int dy[] = {-1, 0, 1, 0};  /* Y offsets for each direction */

/*
 * Calculate distance from spawn point (center of map)
 * This is used for difficulty scaling - monsters get harder further from center
 * Uses Euclidean distance formula: sqrt((x2-x1)² + (y2-y1)²)
 */
static int distance_from_center(const Position *pos) {
    int dx = pos->x - MAP_CENTER;  /* Horizontal distance from center */
    int dy = pos->y - MAP_CENTER;  /* Vertical distance from center */
    return (int)sqrt(dx * dx + dy * dy);  /* Calculate straight-line distance */
}

/*
 * Check if a position is a special location on the map
 * Returns:
 *   1 - Boss location (four corners of the map)
 *   2 - Ancient shrine (four cardinal directions from center)
 *   0 - Normal location
 */
static int is_special_location(const Position *pos) {
    /* Check if position is one of the four corners [0,0], [0,49], [49,0], [49,49]
     * These are boss lair locations with Ancient Dragons */
    if ((pos->x == 0 && pos->y == 0) ||
        (pos->x == 0 && pos->y == MAP_SIZE - 1) ||
        (pos->x == MAP_SIZE - 1 && pos->y == 0) ||
        (pos->x == MAP_SIZE - 1 && pos->y == MAP_SIZE - 1)) {
        return 1; // Boss location
    }
    
    /* Check if position is at one of four cardinal points from center
     * These are ancient shrine locations: North, South, East, West from spawn */
    if ((pos->x == MAP_CENTER && pos->y == 0) ||
        (pos->x == MAP_CENTER && pos->y == MAP_SIZE - 1) ||
        (pos->x == 0 && pos->y == MAP_CENTER) ||
        (pos->x == MAP_SIZE - 1 && pos->y == MAP_CENTER)) {
        return 2; // Shrine location
    }
    
    return 0;  /* Normal location - no special properties */
}

/*
 * Recursive backtracking maze generation algorithm
 * 
 * This creates a perfect maze (no loops, all areas connected) by:
 * 1. Mark current cell as visited and make it a floor tile
 * 2. Randomly shuffle directions to try
 * 3. For each direction, check if we can move 2 cells away
 * 4. If the destination is unvisited, carve a corridor between them
 * 5. Recursively continue from the new cell
 * 
 * Why move 2 cells at a time? This creates walls between paths,
 * resulting in proper maze corridors instead of a completely open area.
 */
static void carve_maze(Map *map, int x, int y) {
    /* Mark this cell as visited and make it walkable */
    map->visited[y][x] = 1;
    map->tiles[y][x] = TILE_FLOOR;
    
    /* Create array of 4 directions (0=North, 1=East, 2=South, 3=West)
     * and shuffle them using Fisher-Yates algorithm
     * This ensures random maze generation each time */
    int dirs[4] = {0, 1, 2, 3};
    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);  /* Pick random index from 0 to i */
        /* Swap elements i and j */
        int temp = dirs[i];
        dirs[i] = dirs[j];
        dirs[j] = temp;
    }
    
    /* Try each direction in random order */
    for (int i = 0; i < 4; i++) {
        int dir = dirs[i];
        
        /* Calculate next position - move 2 cells in chosen direction
         * This leaves a wall between paths for proper maze structure */
        int nx = x + dx[dir] * 2;  /* New X coordinate */
        int ny = y + dy[dir] * 2;  /* New Y coordinate */
        
        /* Check if the new position is:
         * 1. Within map boundaries (0 to MAP_SIZE-1)
         * 2. Not yet visited (to avoid creating loops) */
        if (nx >= 0 && nx < MAP_SIZE && ny >= 0 && ny < MAP_SIZE && 
            !map->visited[ny][nx]) {
            
            /* Carve a corridor in the cell between current and next position
             * This connects the two rooms with a hallway */
            int mx = x + dx[dir];  /* Middle X coordinate */
            int my = y + dy[dir];  /* Middle Y coordinate */
            map->tiles[my][mx] = TILE_CORRIDOR;
            
            /* Recursively carve maze from the new cell
             * This creates branching paths throughout the dungeon */
            carve_maze(map, nx, ny);
        }
    }
}

/*
 * Generate a procedural dungeon maze
 * 
 * Steps:
 * 1. Initialize entire map as walls
 * 2. Use recursive backtracking to carve maze from center
 * 3. Ensure special locations (bosses and shrines) are accessible
 * 4. Add random connections to reduce linearity
 */
void map_generate(Map *map) {
    /* Initialize the entire map as solid walls
     * The maze generation will carve out walkable areas from this */
    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            map->tiles[y][x] = TILE_WALL;  /* Start with everything as walls */
            map->visited[y][x] = 0;         /* Mark as unvisited */
        }
    }
    
    /* Begin maze generation from the center of the map
     * This is where the player spawns, so maze grows outward from there */
    carve_maze(map, MAP_CENTER, MAP_CENTER);
    
    /* Manually ensure special locations are accessible
     * These may not have been reached by the maze algorithm */
    
    /* Make boss lair corners walkable - these contain Ancient Dragons */
    map->tiles[0][0] = TILE_FLOOR;                        // Top-left corner
    map->tiles[0][MAP_SIZE - 1] = TILE_FLOOR;            // Bottom-left corner
    map->tiles[MAP_SIZE - 1][0] = TILE_FLOOR;            // Top-right corner
    map->tiles[MAP_SIZE - 1][MAP_SIZE - 1] = TILE_FLOOR; // Bottom-right corner
    
    /* Make shrine locations walkable - these offer healing, gold, or XP */
    map->tiles[0][MAP_CENTER] = TILE_FLOOR;              // Left shrine
    map->tiles[MAP_SIZE - 1][MAP_CENTER] = TILE_FLOOR;   // Right shrine
    map->tiles[MAP_CENTER][0] = TILE_FLOOR;              // Top shrine
    map->tiles[MAP_CENTER][MAP_SIZE - 1] = TILE_FLOOR;   // Bottom shrine
    
    /* Add random extra corridors to break up the linear maze structure
     * 20% of remaining walls become corridors, creating loops and shortcuts
     * This makes exploration more interesting and less predictable */
    for (int y = 1; y < MAP_SIZE - 1; y++) {
        for (int x = 1; x < MAP_SIZE - 1; x++) {
            if (map->tiles[y][x] == TILE_WALL && rand() % 100 < 20) {
                map->tiles[y][x] = TILE_CORRIDOR;
            }
        }
    }
}

/*
 * Check if a position is walkable (not a wall or out of bounds)
 * Returns 1 if player can move there, 0 if blocked
 */
int map_can_move(const Map *map, int x, int y) {
    /* First check if position is within map boundaries */
    if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE) {
        return 0;  /* Out of bounds - can't move there */
    }
    /* Check if tile is not a wall (floors and corridors are walkable) */
    return map->tiles[y][x] != TILE_WALL;
}

/*
 * Get the tile type at a specific position
 * Returns TILE_WALL if position is out of bounds (treats edges as walls)
 */
TileType map_get_tile(const Map *map, int x, int y) {
    /* Check boundaries - treat out of bounds as walls */
    if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE) {
        return TILE_WALL;
    }
    /* Return the actual tile type at this position */
    return map->tiles[y][x];
}

/*
 * Trigger events when player enters a room
 * 
 * This function handles all random encounters and events:
 * - Boss fights at corners (Ancient Dragons)
 * - Ancient Shrines at cardinal points (healing, gold, or XP)
 * - Random events based on distance from spawn (difficulty scaling)
 * - Traps, monsters, treasure, healing items, artifacts, merchants
 * 
 * Events become more dangerous/rewarding further from the center
 */
void search_room(Player *player, Position *pos, char *message)
{
    /* Check if this is a special location (boss or shrine) */
    int special = is_special_location(pos);
    
    /* BOSS ENCOUNTERS - Ancient Dragons at the four corners of the map
     * These are end-game challenges with legendary rewards */
    if (special == 1) {
        snprintf(message, 256, "*** BOSS LAIR! An Ancient Dragon guards treasure! ***");
        
        /* Create a powerful boss enemy with high stats
         * HP=250, Attack=30, Defense=12, Loot=150-300 gold, 500 XP */
        Monster boss = {"Ancient Dragon", 250, 30, 12, 150, 300, 500};
        int mhp = boss.hp;
        
        /* Note: Battle system currently needs refactoring for UI integration
         * For now, we simulate an automatic victory
         * TODO: Refactor battle system to work with message-based UI */
        
        /* Award boss rewards */
        player->gold += 250;
        player_gain_exp(player, boss.exp_reward);
        
        /* Create legendary weapon with high damage and defense bonuses */
        Item legendary = (Item){100, ITEM_WEAPON, "Legendary Dragonslayer", 1, {25, 5}, 500};
        player_add_item(player, &legendary);
        
        snprintf(message, 256, "Victory! Defeated Ancient Dragon! +250 gold, legendary item!");
        return;
    }
    
    /* ANCIENT SHRINE ENCOUNTERS - Special locations at cardinal points
     * These offer one of three random blessings: healing, gold, or experience */
    if (special == 2) {
        /* Randomly choose one of three shrine blessings */
        int choice = rand() % 3;
        
        if (choice == 0) {
            /* Healing blessing - restore 50-99 HP */
            int heal = 50 + rand() % 50;
            player->health += heal;
            /* Cap health at maximum (can't overheal) */
            if (player->health > player->max_health) player->health = player->max_health;
            snprintf(message, 256, "Found Ancient Shrine! Restored %d HP.", heal);
        } else if (choice == 1) {
            /* Wealth blessing - receive 75-149 gold */
            int gold = 75 + rand() % 75;
            player->gold += gold;
            snprintf(message, 256, "Found Ancient Shrine! Received %d gold in offerings.", gold);
        } else {
            /* Wisdom blessing - gain 50-149 experience points */
            int exp = 50 + rand() % 100;
            player_gain_exp(player, exp);
            snprintf(message, 256, "Found Ancient Shrine! Gained wisdom (+%d XP).", exp);
        }
        return;
    }
    
    /* NORMAL ROOM EVENTS - Random encounters with difficulty scaling
     * Difficulty increases based on distance from spawn point */
    
    /* Calculate how far player is from the spawn point (center) */
    int dist = distance_from_center(pos);
    
    /* Difficulty level - increases by 1 for every 5 tiles from center
     * Used to scale trap damage, monster strength, and loot amounts */
    int difficulty = dist / 5;
    
    /* Roll a random number (0-99) to determine which event occurs
     * Different event types have probability ranges that shift with difficulty
     * This makes encounters more dangerous further from spawn */
    int event_roll = rand() % 100;
    
    /* Calculate probability thresholds for each event type
     * These increase with difficulty, making traps and monsters more common */
    int trap_threshold = 10 + difficulty * 2;      /* Base 10%, +2% per difficulty */
    int monster_threshold = trap_threshold + 20 + difficulty * 3;  /* +23-35% range */
    int treasure_threshold = monster_threshold + 15;  /* +15% chance */
    
    /* TRAP EVENT - Spikes, poison darts, pit traps, etc. */
    if (event_roll < trap_threshold) {
        /* Trap damage scales with difficulty: 5-20 base + 0-difficulty*2 extra */
        int dmg = 5 + rand() % 16 + difficulty * 2;
        player->health -= dmg;
        /* Prevent health from going negative */
        if (player->health < 0) player->health = 0;
        snprintf(message, 256, "Trap triggered! Took %d damage.", dmg);
    }
    /* MONSTER ENCOUNTER - Combat with random enemy */
    else if (event_roll < monster_threshold) {
        /* TODO: Battle system needs refactoring to work with UI messages
         * For now, we simulate a quick auto-resolved battle */
        int loot = 10 + rand() % 30;
        player->gold += loot;
        snprintf(message, 256, "Monster appeared and defeated! Looted %d gold.", loot);
    }
    /* TREASURE CHEST - Gold found in chest */
    else if (event_roll < treasure_threshold) {
        /* Gold amount: 10-50 base + 0-difficulty*5 bonus */
        int gold = 10 + rand() % 41 + difficulty * 5;
        player->gold += gold;
        snprintf(message, 256, "Found hidden chest with %d gold!", gold);
    }
    /* HEALING POTION - Restore health */
    else if (event_roll < treasure_threshold + 15) {
        /* Heal 10-30 HP */
        int heal = 10 + rand() % 21;
        player->health += heal;
        /* Cap at maximum health */
        if (player->health > player->max_health) player->health = player->max_health;
        snprintf(message, 256, "Found healing potion! Recovered %d HP.", heal);
    }
    /* ANCIENT ARTIFACT - Valuable item to sell */
    else if (event_roll < treasure_threshold + 20) {
        /* High value: 100-150 base + 0-difficulty*10 bonus */
        int gold = 100 + rand() % 51 + difficulty * 10;
        player->gold += gold;
        snprintf(message, 256, "Ancient artifact found! Sold for %d gold.", gold);
    }
    /* WANDERING MERCHANT - Flavor event (no mechanical effect) */
    else if (event_roll < treasure_threshold + 25) {
        snprintf(message, 256, "Encountered wandering merchant. They nod and pass by.");
    }
    /* EMPTY ROOM - Nothing happens */
    else {
        snprintf(message, 256, "Nothing of interest found in this area.");
    }
}

/*
 * Process player commands and update game state
 * 
 * Handles:
 * - Movement in four directions (N/S/E/W)
 * - Quit command (Q)
 * - Map view (M)
 * - Inventory (I)
 * - Wall collision detection
 * - Triggering room events after movement
 */
void handle_command(char command, int *running, Position *pos, Player *player, char *message, Map *map)
{
    /* Track whether player successfully moved this turn */
    int moved = 0;
    
    /* Create a tentative new position (may not be valid) */
    Position new_pos = *pos;
    
    /* Convert command to uppercase for easier matching */
    command = (char)toupper((unsigned char)command);
    
    /* Process the command using a switch statement */
    switch (command)
    {
    case 'Q':  /* QUIT - Exit the game */
        *running = 0;  /* Set running flag to false */
        snprintf(message, 256, "Quitting the game. Thanks for playing!");
        return;
        
    case 'M':  /* MAP - Show full map view (not yet fully implemented) */
        snprintf(message, 256, "Full map view - press any key to continue");
        return;
        
    case 'I':  /* INVENTORY - Show items (not yet fully implemented) */
        snprintf(message, 256, "Inventory screen - press any key to continue");
        return;
        
    case 'N':  /* NORTH - Move up (decrease Y coordinate) */
        new_pos.y--;
        if (map_can_move(map, new_pos.x, new_pos.y)) {
            moved = 1;  /* Valid move */
        } else {
            snprintf(message, 256, "Cannot go north - there's a wall!");
        }
        break;
        
    case 'S':  /* SOUTH - Move down (increase Y coordinate) */
        new_pos.y++;
        if (map_can_move(map, new_pos.x, new_pos.y)) {
            moved = 1;  /* Valid move */
        } else {
            snprintf(message, 256, "Cannot go south - there's a wall!");
        }
        break;
        
    case 'E':  /* EAST - Move right (increase X coordinate) */
        new_pos.x++;
        if (map_can_move(map, new_pos.x, new_pos.y)) {
            moved = 1;  /* Valid move */
        } else {
            snprintf(message, 256, "Cannot go east - there's a wall!");
        }
        break;
        
    case 'W':  /* WEST - Move left (decrease X coordinate) */
        new_pos.x--;
        if (map_can_move(map, new_pos.x, new_pos.y)) {
            moved = 1;  /* Valid move */
        } else {
            snprintf(message, 256, "Cannot go west - there's a wall!");
        }
        break;
        
    default:  /* INVALID COMMAND */
        snprintf(message, 256, "Invalid command. Use N/S/E/W to move, I for inventory, M for map, Q to quit.");
        return;
    }
    
    /* If movement was valid, update position and trigger room event */
    if (moved) {
        *pos = new_pos;  /* Update player's actual position */
        search_room(player, pos, message);  /* Trigger events in new room */
    }
}
