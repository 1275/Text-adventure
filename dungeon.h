#ifndef DUNGEON_H
#define DUNGEON_H

char read_command(void);
void print_status(int hp, int gold);
void search_room(int *player_health, int *player_gold, int *current_room);
void handle_command(char command, int *running, int *current_room, int *player_health, int *player_gold);

#endif