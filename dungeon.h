#ifndef DUNGEON_H
#define DUNGEON_H

#include "player.h"

// Map constants
#define MAP_SIZE 50
#define MAP_CENTER (MAP_SIZE / 2)

// Tile types
typedef enum {
    TILE_WALL,
    TILE_FLOOR,
    TILE_CORRIDOR
} TileType;

// Position on the map
typedef struct {
    int x;
    int y;
} Position;

// Map structure
typedef struct {
    TileType tiles[MAP_SIZE][MAP_SIZE];
    int visited[MAP_SIZE][MAP_SIZE];
} Map;

// Map generation and access
void map_generate(Map *map);
int map_can_move(const Map *map, int x, int y);
TileType map_get_tile(const Map *map, int x, int y);

char read_command(void);
void search_room(Player *player, Position *pos, char *message);
void handle_command(char command, int *running, Position *pos, Player *player, char *message, Map *map);
void print_map(const Position *pos);

#endif