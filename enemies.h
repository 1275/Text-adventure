#ifndef ENEMIES_H
#define ENEMIES_H

#include "player.h"

// Monster template definition - stores base stats and scaling info
typedef struct {
    const char *name;
    int level_offset_min;    // Minimum level offset from player (e.g., -2 means player_level - 2)
    int level_offset_max;    // Maximum level offset from player (e.g., +2 means player_level + 2)
    int base_hp;             // Base HP at level 1
    int hp_per_level;        // HP gained per level
    int base_attack;         // Base attack at level 1
    int attack_per_level;    // Attack gained per level
    int base_defense;        // Base defense at level 1
    int defense_per_level;   // Defense gained per level
    int min_loot;
    int max_loot;
    int exp_reward_base;     // Base exp reward
} MonsterTemplate;

// Actual monster instance with generated stats
typedef struct {
    const char *name;
    int level;
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