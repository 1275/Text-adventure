#include <stdio.h>
#include <stdlib.h>
#include "enemies.h"
#include "player.h"

typedef struct {
    const char *name;
    int hp;
    int attack;
    int defense;
    int min_loot;
    int max_loot;
} Monster;

int battle_monster(Player *player)
{
    static const Monster monsters[] = {
        {"Goblin",       30,  7, 1,  8, 20},
        {"Skeleton",     40,  9, 2, 12, 26},
        {"Giant Spider", 50, 11, 3, 15, 30},
        {"Orc",          60, 13, 4, 20, 35},
        {"Troll",        80, 16, 5, 30, 45},
    };
    const int mcount = (int)(sizeof(monsters)/sizeof(monsters[0]));
    int idx = rand() % mcount;
    Monster m = monsters[idx];

    int mhp = m.hp;
    printf("A %s appears with %d HP!\n", m.name, mhp);

    while (mhp > 0 && player->health > 0)
    {
        int p_roll = rand() % 6;         // 0..5 variation
        int m_roll = rand() % 4;         // 0..3 variation

        int p_attack = player->total_damage + p_roll;
        int m_attack = m.attack + m_roll;

        int dmg_to_mon = p_attack - m.defense;
        if (dmg_to_mon < 1) dmg_to_mon = 1;

        mhp -= dmg_to_mon;
        if (mhp < 0) mhp = 0;
        printf("You hit the %s for %d. Monster HP: %d\n", m.name, dmg_to_mon, mhp);

        if (mhp <= 0) {
            printf("You have defeated the %s!\n", m.name);
            int loot = m.min_loot + (rand() % (m.max_loot - m.min_loot + 1));
            return loot;
        }

        int dmg_to_player = m_attack - player->total_defense;
        if (dmg_to_player < 1) dmg_to_player = 1;

        player->health -= dmg_to_player;
        if (player->health < 0) player->health = 0;
        printf("The %s hits you for %d. Your HP: %d\n", m.name, dmg_to_player, player->health);
    }

    return 0;
}