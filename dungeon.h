#ifndef DUNGEON_H
#define DUNGEON_H

#include "player.h"

char read_command(void);
void search_room(Player *player, int *current_room);
void handle_command(char command, int *running, int *current_room, Player *player);

#endif