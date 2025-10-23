#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "dungeon.h"
#include "enemies.h"
#include "player.h"
#include "ui.h"

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
__attribute__((unused))
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
            // Initialize tile data
            map->data[y][x].content = CONTENT_EMPTY;
            map->data[y][x].difficulty = DIFFICULTY_EASY;
            map->data[y][x].treasure_value = 0;
            map->data[y][x].is_looted = 0;
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
    
    // ========================================================================
    // PRE-POPULATE DUNGEON WITH CONTENT
    // ========================================================================
    
    // Place bosses at corners
    map->data[0][0].content = CONTENT_BOSS;
    map->data[0][MAP_SIZE - 1].content = CONTENT_BOSS;
    map->data[MAP_SIZE - 1][0].content = CONTENT_BOSS;
    map->data[MAP_SIZE - 1][MAP_SIZE - 1].content = CONTENT_BOSS;
    map->data[0][0].difficulty = DIFFICULTY_BOSS;
    map->data[0][MAP_SIZE - 1].difficulty = DIFFICULTY_BOSS;
    map->data[MAP_SIZE - 1][0].difficulty = DIFFICULTY_BOSS;
    map->data[MAP_SIZE - 1][MAP_SIZE - 1].difficulty = DIFFICULTY_BOSS;
    
    // Place shrines at cardinal directions
    map->data[0][MAP_CENTER].content = CONTENT_SHRINE;
    map->data[MAP_SIZE - 1][MAP_CENTER].content = CONTENT_SHRINE;
    map->data[MAP_CENTER][0].content = CONTENT_SHRINE;
    map->data[MAP_CENTER][MAP_SIZE - 1].content = CONTENT_SHRINE;
    
    // Populate the dungeon with monsters, treasures, and traps
    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            // Only populate walkable tiles
            if (map->tiles[y][x] == TILE_WALL) continue;
            
            // Skip center starting position
            if (x == MAP_CENTER && y == MAP_CENTER) continue;
            
            // Skip already assigned special locations
            if (map->data[y][x].content != CONTENT_EMPTY) continue;
            
            // Calculate distance from center for difficulty scaling
            int dx = x - MAP_CENTER;
            int dy = y - MAP_CENTER;
            int dist = (int)sqrt(dx * dx + dy * dy);
            
            // Random chance to populate this tile
            int roll = rand() % 100;
            
            // Close to center: safer, more healing and treasure
            if (dist < 10) {
                if (roll < 15) {  // 15% monster
                    map->data[y][x].content = CONTENT_MONSTER;
                    map->data[y][x].difficulty = DIFFICULTY_EASY;
                } else if (roll < 30) {  // 15% treasure
                    map->data[y][x].content = CONTENT_TREASURE;
                    map->data[y][x].treasure_value = 20 + rand() % 40;
                } else if (roll < 40) {  // 10% healing fountain
                    map->data[y][x].content = CONTENT_HEALING_FOUNTAIN;
                } else if (roll < 45) {  // 5% trap
                    map->data[y][x].content = CONTENT_TRAP;
                }
            }
            // Mid range: balanced danger
            else if (dist < 20) {
                if (roll < 30) {  // 30% monster
                    map->data[y][x].content = CONTENT_MONSTER;
                    map->data[y][x].difficulty = (rand() % 2) ? DIFFICULTY_EASY : DIFFICULTY_MEDIUM;
                } else if (roll < 45) {  // 15% treasure
                    map->data[y][x].content = CONTENT_TREASURE;
                    map->data[y][x].treasure_value = 40 + rand() % 60;
                } else if (roll < 53) {  // 8% healing fountain
                    map->data[y][x].content = CONTENT_HEALING_FOUNTAIN;
                } else if (roll < 63) {  // 10% trap
                    map->data[y][x].content = CONTENT_TRAP;
                }
            }
            // Far from center: dangerous
            else {
                if (roll < 40) {  // 40% monster
                    map->data[y][x].content = CONTENT_MONSTER;
                    int diff_roll = rand() % 100;
                    if (diff_roll < 40) {
                        map->data[y][x].difficulty = DIFFICULTY_MEDIUM;
                    } else if (diff_roll < 80) {
                        map->data[y][x].difficulty = DIFFICULTY_HARD;
                    } else {
                        map->data[y][x].difficulty = DIFFICULTY_EASY;  // Still some easy ones
                    }
                } else if (roll < 55) {  // 15% treasure
                    map->data[y][x].content = CONTENT_TREASURE;
                    map->data[y][x].treasure_value = 60 + rand() % 100;
                } else if (roll < 60) {  // 5% healing fountain
                    map->data[y][x].content = CONTENT_HEALING_FOUNTAIN;
                } else if (roll < 75) {  // 15% trap
                    map->data[y][x].content = CONTENT_TRAP;
                }
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
    // Mark room as visited for map display
    map->visited[pos->y][pos->x] = 1;
    
    // Get the pre-placed content of this tile
    TileData *tile = &map->data[pos->y][pos->x];
    
    // If already looted, nothing happens
    if (tile->is_looted) {
        snprintf(message, 256, "This area has already been explored. Nothing new here.");
        return;
    }
    
    // Process the content based on what was pre-placed
    switch (tile->content) {
    case CONTENT_BOSS: {
        snprintf(message, 256, "*** BOSS LAIR! An Ancient Dragon appears! ***");
        battle->is_active = 1;
        battle->monster = (Monster){
            .name = "Ancient Dragon",
            .level = 20,
            .hp = 250,
            .attack = 30,
            .defense = 12,
            .min_loot = 150,
            .max_loot = 300,
            .exp_reward = 500
        };
        battle->monster_hp = battle->monster.hp;
        tile->is_looted = 1;  // Mark as encountered
        return;
    }
    
    case CONTENT_SHRINE: {
        int choice = rand() % 3;
        if (choice == 0) {
            int heal = 50 + rand() % 50;
            player->health += heal;
            if (player->health > player->max_health) player->health = player->max_health;
            snprintf(message, 256, "✦ Found Ancient Shrine! Restored %d HP.", heal);
        } else if (choice == 1) {
            int gold = 75 + rand() % 75;
            player->gold += gold;
            snprintf(message, 256, "✦ Found Ancient Shrine! Received %d gold in offerings.", gold);
        } else {
            int exp = 50 + rand() % 100;
            player_gain_exp(player, exp);
            snprintf(message, 256, "✦ Found Ancient Shrine! Gained wisdom (+%d XP).", exp);
        }
        tile->is_looted = 1;
        return;
    }
    
    case CONTENT_MONSTER: {
        // Monster encounter - use pre-determined difficulty
        static const Monster easy_monsters[] = {
            {.name = "Goblin", .level = 1, .hp = 30, .attack = 7, .defense = 1, .min_loot = 8, .max_loot = 20, .exp_reward = 25},
            {.name = "Skeleton", .level = 2, .hp = 40, .attack = 9, .defense = 2, .min_loot = 12, .max_loot = 26, .exp_reward = 35},
            {.name = "Giant Rat", .level = 1, .hp = 25, .attack = 6, .defense = 1, .min_loot = 5, .max_loot = 15, .exp_reward = 20},
        };
        static const Monster medium_monsters[] = {
            {.name = "Orc Warrior", .level = 4, .hp = 60, .attack = 13, .defense = 4, .min_loot = 20, .max_loot = 35, .exp_reward = 60},
            {.name = "Giant Spider", .level = 3, .hp = 50, .attack = 11, .defense = 3, .min_loot = 15, .max_loot = 30, .exp_reward = 45},
            {.name = "Zombie", .level = 3, .hp = 55, .attack = 10, .defense = 3, .min_loot = 18, .max_loot = 28, .exp_reward = 50},
        };
        static const Monster hard_monsters[] = {
            {.name = "Troll", .level = 6, .hp = 80, .attack = 16, .defense = 5, .min_loot = 30, .max_loot = 45, .exp_reward = 80},
            {.name = "Dark Knight", .level = 8, .hp = 100, .attack = 20, .defense = 7, .min_loot = 40, .max_loot = 60, .exp_reward = 120},
            {.name = "Demon", .level = 7, .hp = 90, .attack = 18, .defense = 6, .min_loot = 35, .max_loot = 55, .exp_reward = 100},
        };
        
        Monster m;
        switch (tile->difficulty) {
        case DIFFICULTY_EASY:
            m = easy_monsters[rand() % 3];
            break;
        case DIFFICULTY_MEDIUM:
            m = medium_monsters[rand() % 3];
            break;
        case DIFFICULTY_HARD:
            m = hard_monsters[rand() % 3];
            break;
        default:
            m = easy_monsters[0];
        }
        
        battle->is_active = 1;
        battle->monster = m;
        battle->monster_hp = m.hp;
        snprintf(message, 256, "⚔ A %s appears! Prepare for battle!", m.name);
        tile->is_looted = 1;  // Monster won't respawn
        return;
    }
    
    case CONTENT_TREASURE: {
        int gold = tile->treasure_value;
        player->gold += gold;
        
        // Chance for bonus item (30%)
        if (rand() % 100 < 30) {
            Item drop;
            int drop_type = rand() % 100;
            
            if (drop_type < 40) {
                drop = (Item){10, ITEM_CONSUMABLE, "Health Potion", 1, {0, 0}, 10};
            } else if (drop_type < 70) {
                const char *weapon_names[] = {"Iron Sword", "Steel Axe", "War Hammer", "Enchanted Blade"};
                int wpn_idx = rand() % 4;
                int dmg_bonus = 5 + rand() % 8 + (tile->treasure_value / 20);
                drop = (Item){20 + wpn_idx, ITEM_WEAPON, weapon_names[wpn_idx], 1, {dmg_bonus, 0}, 20 + dmg_bonus * 2};
            } else {
                const char *armor_names[] = {"Leather Armor", "Chain Mail", "Plate Armor", "Dragon Scale"};
                int arm_idx = rand() % 4;
                int def_bonus = 3 + rand() % 6 + (tile->treasure_value / 25);
                drop = (Item){30 + arm_idx, ITEM_ARMOR, armor_names[arm_idx], 1, {0, def_bonus}, 15 + def_bonus * 2};
            }
            
            player_add_item(player, &drop);
            snprintf(message, 256, "💰 Found treasure chest with %d gold and %s!", gold, drop.name);
        } else {
            snprintf(message, 256, "💰 Found treasure chest with %d gold!", gold);
        }
        tile->is_looted = 1;
        return;
    }
    
    case CONTENT_TRAP: {
        int dist = distance_from_center(pos);
        int dmg = 10 + rand() % 20 + (dist / 3);
        player->health -= dmg;
        if (player->health < 0) player->health = 0;
        snprintf(message, 256, "💥 Trap triggered! Took %d damage.", dmg);
        tile->is_looted = 1;
        return;
    }
    
    case CONTENT_HEALING_FOUNTAIN: {
        int heal = 20 + rand() % 30;
        player->health += heal;
        if (player->health > player->max_health) player->health = player->max_health;
        snprintf(message, 256, "⛲ Found a healing fountain! Recovered %d HP.", heal);
        tile->is_looted = 1;
        return;
    }
    
    case CONTENT_EMPTY:
    default:
        // Empty room - small chance for random events
        if (rand() % 100 < 10) {
            int event = rand() % 3;
            if (event == 0) {
                int gold = 5 + rand() % 10;
                player->gold += gold;
                snprintf(message, 256, "Found %d gold coins on the ground.", gold);
            } else if (event == 1) {
                snprintf(message, 256, "You hear distant echoes through the corridors...");
            } else {
                snprintf(message, 256, "Strange markings on the walls catch your eye.");
            }
        } else {
            snprintf(message, 256, "Nothing of interest found in this area.");
        }
        tile->is_looted = 1;
        return;
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

void handle_inventory_command(Player *player, char *message, GameState *state, char first_char)
{
    char input_buffer[256];
    char command = '\0';
    int slot = -1;
    
    // If we already have the first character from main loop, use it
    // Otherwise read a full line
    if (first_char != '\0') {
        // We got the first character, now read the rest of the line for the slot number
        command = (char)toupper((unsigned char)first_char);
        
        // If command needs a slot number, read it
        if (command == 'U' || command == 'E') {
            if (scanf("%d", &slot) != 1) {
                // Clear buffer
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                snprintf(message, 256, "Invalid slot number! Use: %c <slot>  (e.g., %c 1)", command, command);
                return;
            }
            // Clear the rest of the line
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        } else if (command == 'Q') {
            // Q doesn't need extra input, just clear the line
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    } else {
        // Read entire line of input
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            snprintf(message, 256, "Invalid input!");
            return;
        }
        
        // Parse the command - sscanf extracts first char and optional number
        int items_read = sscanf(input_buffer, " %c %d", &command, &slot);
        
        if (items_read < 1) {
            snprintf(message, 256, "Invalid input! Could not read command.");
            return;
        }
        
        // Convert command to uppercase
        command = (char)toupper((unsigned char)command);
        
        // Check if slot is needed but not provided
        if ((command == 'U' || command == 'E') && items_read < 2) {
            snprintf(message, 256, "Invalid slot number! Use: %c <slot>  (e.g., %c 1)", command, command);
            return;
        }
    }
    
    // Handle commands
    switch (command) {
    case 'Q':
        // Exit inventory
        *state = STATE_EXPLORING;
        snprintf(message, 256, "Exited inventory.");
        return;
        
    case 'U':
        // Use consumable - requires a slot number
        if (slot < 0) {
            snprintf(message, 256, "Invalid slot number! Use: U <slot>  (e.g., U 1)");
            return;
        }
        
        if (slot < 0 || slot >= player->inv_count) {
            snprintf(message, 256, "Invalid slot %d! Choose 0-%d.", slot, player->inv_count - 1);
            return;
        }
        
        Item *use_item = &player->inventory[slot];
        if (use_item->type != ITEM_CONSUMABLE) {
            snprintf(message, 256, "Cannot use %s - not a consumable!", use_item->name);
            return;
        }
        
        if (use_item->quantity <= 0) {
            snprintf(message, 256, "%s is depleted!", use_item->name);
            return;
        }
        
        // Use the item (heal)
        int heal_amount = 30 + (use_item->value * 2);
        int old_hp = player->health;
        player->health += heal_amount;
        if (player->health > player->max_health) {
            player->health = player->max_health;
        }
        int actual_heal = player->health - old_hp;
        
        use_item->quantity--;
        
        // Remove if depleted
        if (use_item->quantity <= 0) {
            snprintf(message, 256, "Used %s! Healed %d HP. %s depleted!", 
                    use_item->name, actual_heal, use_item->name);
            // Shift inventory
            for (int i = slot; i < player->inv_count - 1; i++) {
                player->inventory[i] = player->inventory[i + 1];
                if (player->equipped.weapon_slot == i + 1) player->equipped.weapon_slot = i;
                if (player->equipped.armor_slot == i + 1) player->equipped.armor_slot = i;
            }
            player->inv_count--;
            if (player->equipped.weapon_slot == slot) player->equipped.weapon_slot = INVALID_SLOT;
            if (player->equipped.armor_slot == slot) player->equipped.armor_slot = INVALID_SLOT;
        } else {
            snprintf(message, 256, "Used %s! Healed %d HP. (%d remaining)", 
                    use_item->name, actual_heal, use_item->quantity);
        }
        return;
        
    case 'E':
        // Equip item - requires a slot number
        if (slot < 0) {
            snprintf(message, 256, "Invalid slot number! Use: E <slot>  (e.g., E 0)");
            return;
        }
        
        if (slot < 0 || slot >= player->inv_count) {
            snprintf(message, 256, "Invalid slot %d! Choose 0-%d.", slot, player->inv_count - 1);
            return;
        }
        
        Item *equip_item = &player->inventory[slot];
        
        if (equip_item->type == ITEM_WEAPON) {
            if (player->equipped.weapon_slot == slot) {
                snprintf(message, 256, "%s is already equipped!", equip_item->name);
            } else {
                player->equipped.weapon_slot = slot;
                player_apply_equipment(player);
                snprintf(message, 256, "Equipped %s! Attack: %d (+%d dmg)", 
                        equip_item->name, player->total_damage, equip_item->stats.damage);
            }
        } else if (equip_item->type == ITEM_ARMOR) {
            if (player->equipped.armor_slot == slot) {
                snprintf(message, 256, "%s is already equipped!", equip_item->name);
            } else {
                player->equipped.armor_slot = slot;
                player_apply_equipment(player);
                snprintf(message, 256, "Equipped %s! Defense: %d (+%d def)", 
                        equip_item->name, player->total_defense, equip_item->stats.defense);
            }
        } else {
            snprintf(message, 256, "Cannot equip %s - not a weapon or armor!", equip_item->name);
        }
        return;
        
    default:
        snprintf(message, 256, "Invalid command '%c'! Use U <slot>, E <slot>, or Q to exit.", command);
        return;
    }
}

void handle_command(char command, int *running, Position *pos, Player *player, char *message, Map *map, GameState *state, BattleState *battle)
{
    // If in battle, handle battle commands
    if (*state == STATE_BATTLE) {
        handle_battle_command(command, player, battle, message, state);
        return;
    }
    
    // If in inventory, handle inventory commands
    if (*state == STATE_INVENTORY) {
        handle_inventory_command(player, message, state, command);
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
        // Show the explored map
        ui_clear_screen();
        print_explored_map(map, pos, 12);  // Show 12 tile radius
        printf("\nPress any key to continue...");
        fflush(stdout);
        getchar();  // Wait for user input
        snprintf(message, 256, "Viewing map...");
        return;
    case 'I':
        *state = STATE_INVENTORY;
        snprintf(message, 256, "Viewing inventory. Use U <slot> to use items, E <slot> to equip, Q to exit.");
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

// Print explored map with 'X' markers on visited tiles
void print_explored_map(const Map *map, const Position *pos, int radius) {
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                      EXPLORED MAP                            ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Legend: @ = You  X = Visited  ? = Unexplored  # = Wall  \n");
    printf("        M = Monster  T = Treasure  ! = Trap  + = Healing\n");
    printf("        B = Boss  S = Shrine\n\n");
    
    // Display a section of the map around the player
    int start_y = pos->y - radius;
    int end_y = pos->y + radius;
    int start_x = pos->x - radius;
    int end_x = pos->x + radius;
    
    // Clamp to map boundaries
    if (start_y < 0) start_y = 0;
    if (end_y >= MAP_SIZE) end_y = MAP_SIZE - 1;
    if (start_x < 0) start_x = 0;
    if (end_x >= MAP_SIZE) end_x = MAP_SIZE - 1;
    
    // Print column numbers
    printf("    ");
    for (int x = start_x; x <= end_x; x++) {
        printf("%2d", x % 100);
    }
    printf("\n    ");
    for (int x = start_x; x <= end_x; x++) {
        printf("──");
    }
    printf("\n");
    
    // Print map
    for (int y = start_y; y <= end_y; y++) {
        printf("%2d │ ", y);
        for (int x = start_x; x <= end_x; x++) {
            // Current player position
            if (x == pos->x && y == pos->y) {
                printf("@ ");
                continue;
            }
            
            // Wall
            if (map->tiles[y][x] == TILE_WALL) {
                printf("# ");
                continue;
            }
            
            // Visited tile - show what was there or X if looted
            if (map->visited[y][x]) {
                // If already looted, show X
                if (map->data[y][x].is_looted) {
                    printf("X ");
                } else {
                    // Show what's there
                    switch (map->data[y][x].content) {
                    case CONTENT_MONSTER:
                        printf("M ");
                        break;
                    case CONTENT_TREASURE:
                        printf("T ");
                        break;
                    case CONTENT_TRAP:
                        printf("! ");
                        break;
                    case CONTENT_HEALING_FOUNTAIN:
                        printf("+ ");
                        break;
                    case CONTENT_BOSS:
                        printf("B ");
                        break;
                    case CONTENT_SHRINE:
                        printf("S ");
                        break;
                    default:
                        printf("X ");
                        break;
                    }
                }
            } else {
                // Unvisited - show as unknown
                printf("? ");
            }
        }
        printf("\n");
    }
    
    printf("\n");
    printf("Current Position: (%d, %d)\n", pos->x, pos->y);
    printf("Distance from Center: %d tiles\n", distance_from_center(pos));
    
    // Count statistics
    int total_visited = 0;
    int total_walkable = 0;
    int monsters_remaining = 0;
    int treasures_remaining = 0;
    
    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            if (map->tiles[y][x] != TILE_WALL) {
                total_walkable++;
                if (map->visited[y][x]) {
                    total_visited++;
                }
                if (!map->data[y][x].is_looted) {
                    if (map->data[y][x].content == CONTENT_MONSTER || 
                        map->data[y][x].content == CONTENT_BOSS) {
                        monsters_remaining++;
                    }
                    if (map->data[y][x].content == CONTENT_TREASURE) {
                        treasures_remaining++;
                    }
                }
            }
        }
    }
    
    printf("\nExploration: %d/%d tiles (%.1f%%)\n", 
           total_visited, total_walkable, 
           (100.0 * total_visited) / total_walkable);
    printf("Monsters remaining: %d\n", monsters_remaining);
    printf("Treasures remaining: %d\n", treasures_remaining);
    printf("\n");
}

