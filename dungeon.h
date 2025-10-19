#ifndef DUNGEON_H
#define DUNGEON_H

#include "player.h"

/*
 * Map Configuration Constants
 * MAP_SIZE: The dungeon is a 50x50 grid (coordinates 0-49)
 * MAP_CENTER: Player spawns at the center (25, 25)
 */
#define MAP_SIZE 50
#define MAP_CENTER (MAP_SIZE / 2)

/*
 * TileType: Types of terrain in the dungeon
 * TILE_WALL: Impassable walls (shown as # on map)
 * TILE_FLOOR: Open floor space in rooms
 * TILE_CORRIDOR: Hallways connecting rooms
 */
typedef enum {
    TILE_WALL,
    TILE_FLOOR,
    TILE_CORRIDOR
} TileType;

/*
 * Position: Represents a location on the map
 * x: Horizontal coordinate (0 to MAP_SIZE-1)
 * y: Vertical coordinate (0 to MAP_SIZE-1)
 */
typedef struct {
    int x;
    int y;
} Position;

/*
 * Map: The complete dungeon map
 * tiles: 2D array of tile types defining the dungeon layout
 * visited: Tracks which tiles have been visited during maze generation
 */
typedef struct {
    TileType tiles[MAP_SIZE][MAP_SIZE];
    int visited[MAP_SIZE][MAP_SIZE];
} Map;

/*
 * Map Generation and Access Functions
 */

/* Generate a procedural maze dungeon with rooms and corridors */
void map_generate(Map *map);

/* Check if a position is walkable (not a wall); returns 1 if can move, 0 if blocked */
int map_can_move(const Map *map, int x, int y);

/* Get the tile type at a specific position */
TileType map_get_tile(const Map *map, int x, int y);

/*
 * Game Event Functions
 */

/* Read a single character command from the player */
char read_command(void);

/* Trigger random events when entering a room (combat, traps, treasure, etc.) */
void search_room(Player *player, Position *pos, char *message);

/* Process player commands (movement, quit, map, inventory) */
void handle_command(char command, int *running, Position *pos, Player *player, char *message, Map *map);

/* Display the dungeon map around the player's position */
void print_map(const Position *pos);

#endif