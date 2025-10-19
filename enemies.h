#ifndef ENEMIES_H
#define ENEMIES_H

#include "player.h"

/*
 * Monster: Represents an enemy creature in the dungeon
 * name: Display name of the monster (e.g., "Goblin", "Dragon")
 * hp: Hit points - how much damage needed to defeat
 * attack: Attack power - base damage dealt to player
 * defense: Defense stat - reduces damage taken
 * min_loot: Minimum gold dropped when defeated
 * max_loot: Maximum gold dropped when defeated
 * exp_reward: Experience points awarded for defeating this monster
 */
typedef struct {
    const char *name;
    int hp;
    int attack;
    int defense;
    int min_loot;
    int max_loot;
    int exp_reward;
} Monster;

/*
 * Battle System
 */

/* 
 * Start a turn-based battle with a random monster
 * Returns: Amount of gold looted from defeated monster (0 if player fled or died)
 * Side effects: Updates player's health, may add items to inventory
 */
int battle_monster(Player *player);

#endif