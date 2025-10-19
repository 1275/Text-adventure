#ifndef ENEMIES_H
#define ENEMIES_H

#include "player.h"

// Monster definition
typedef struct {
    const char *name;
    int hp;
    int attack;
    int defense;
    int min_loot;
    int max_loot;
    int exp_reward;
} Monster;

// Returns gold looted; mutates player->health and may add items
int battle_monster(Player *player);

#endif