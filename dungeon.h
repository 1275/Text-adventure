#ifndef DUNGEON_H
#define DUNGEON_H

#include "player.h"

// Map constants
#define MAP_SIZE 50
#define MAP_CENTER (MAP_SIZE / 2)

// Position on the map
typedef struct {
    int x;
    int y;
} Position;

char read_command(void);
void search_room(Player *player, Position *pos);
void handle_command(char command, int *running, Position *pos, Player *player);
void print_map(const Position *pos);

#endif