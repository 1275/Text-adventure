#ifndef ENEMIES_H
#define ENEMIES_H

#include "player.h"

// Returns gold looted; mutates player->health and may add items
int battle_monster(Player *player);

#endif