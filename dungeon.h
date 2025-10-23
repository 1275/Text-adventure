#ifndef DUNGEON_H
#define DUNGEON_H

#include "player.h"
#include "enemies.h"

// Map constants
#define MAP_SIZE 500
#define MAP_CENTER (MAP_SIZE / 2)

// Tile types
typedef enum {
    TILE_WALL,
    TILE_FLOOR,
    TILE_CORRIDOR
} TileType;

// Tile content (what's in this location)
typedef enum {
    CONTENT_EMPTY,
    CONTENT_MONSTER,
    CONTENT_TREASURE,
    CONTENT_TRAP,
    CONTENT_HEALING_FOUNTAIN,
    CONTENT_BOSS,
    CONTENT_SHRINE
} TileContent;

// Monster difficulty levels
typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD,
    DIFFICULTY_BOSS
} MonsterDifficulty;

// Position on the map
typedef struct {
    int x;
    int y;
} Position;

// Tile data structure
typedef struct {
    TileContent content;
    MonsterDifficulty difficulty;  // For monsters
    int treasure_value;            // For treasure chests (gold amount)
    int is_looted;                 // Has this tile's content been consumed?
} TileData;

// Map structure
typedef struct {
    TileType tiles[MAP_SIZE][MAP_SIZE];
    TileData data[MAP_SIZE][MAP_SIZE];  // Content of each tile
    int visited[MAP_SIZE][MAP_SIZE];
} Map;

// Game states
typedef enum {
    STATE_EXPLORING,
    STATE_BATTLE,
    STATE_INVENTORY,
    STATE_MAP_VIEW
} GameState;

// Battle state
typedef struct {
    Monster monster;
    int monster_hp;
    int is_active;
} BattleState;

// Map generation and access
void map_generate(Map *map);
int map_can_move(const Map *map, int x, int y);
TileType map_get_tile(const Map *map, int x, int y);

char read_command(void);
void search_room(Player *player, Position *pos, char *message, Map *map, BattleState *battle);
void handle_command(char command, int *running, Position *pos, Player *player, char *message, Map *map, GameState *state, BattleState *battle);
void handle_inventory_command(Player *player, char *message, GameState *state, char first_char);
void print_map(const Position *pos);
void print_explored_map(const Map *map, const Position *pos, int radius);

#endif