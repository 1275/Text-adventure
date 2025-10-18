#include <stdio.h>
#include <stdlib.h>
#include "enemies.h"

int battle_monster(int *player_health)
{
    static const char *names[] = {"Goblin", "Skeleton", "Giant Spider", "Orc", "Troll"};
    static const int base_hp[] = {30, 40, 50, 60, 80};

    int monster_type = rand() % 5;
    const char *name = names[monster_type];
    int monster_health = base_hp[monster_type];

    printf("A %s appears with %d HP!\n", name, monster_health);

    while (monster_health > 0 && *player_health > 0)
    {
        int player_attack = rand() % 20 + 5;  // 5..24
        int monster_attack = rand() % 15 + 3; // 3..17

        monster_health -= player_attack;
        printf("You hit the %s for %d. Monster HP: %d\n", name, player_attack, monster_health > 0 ? monster_health : 0);

        if (monster_health <= 0)
        {
            printf("You have defeated the %s!\n", name);
            int loot = (monster_type + 1) * 10 + (rand() % 11); // 10..60-ish
            return loot;
        }

        *player_health -= monster_attack;
        printf("The %s hits you for %d. Your HP: %d\n", name, monster_attack, *player_health > 0 ? *player_health : 0);
    }

    return 0;
}