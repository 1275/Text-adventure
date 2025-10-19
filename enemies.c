/*
 * enemies.c - Combat System
 * 
 * This file implements the turn-based combat system.
 * 
 * Combat Features:
 * - Turn-based: player acts, then monster acts
 * - Attack: Deal damage based on attack stat minus enemy defense
 * - Flee: 20% chance to escape without fighting
 * - Item Use: Consume potions during combat
 * - Item Drops: 5% chance for monsters to drop equipment or consumables
 * - Experience: Gain XP for defeating enemies
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "enemies.h"
#include "player.h"

/*
 * Battle a random monster in turn-based combat
 * 
 * Returns: Amount of gold looted (0 if player fled or died)
 * Side effects: 
 *   - Modifies player health
 *   - Awards gold and experience on victory
 *   - May add items to player inventory (5% drop chance)
 */
int battle_monster(Player *player)
{
    /* Monster templates with increasing difficulty
     * Format: {Name, HP, Attack, Defense, Min_Loot, Max_Loot, XP_Reward}
     * These represent all possible enemies that can be encountered */
    static const Monster monsters[] = {
        {"Goblin",       30,  7, 1,  8, 20,  25},   /* Weakest enemy */
        {"Skeleton",     40,  9, 2, 12, 26,  35},
        {"Giant Spider", 50, 11, 3, 15, 30,  45},
        {"Orc",          60, 13, 4, 20, 35,  60},
        {"Troll",        80, 16, 5, 30, 45,  80},
        {"Dark Knight", 100, 20, 7, 40, 60, 120},
        {"Dragon",      150, 25, 9, 60, 100, 200},  /* Strongest enemy */
    };
    
    /* Count how many monster types we have */
    const int mcount = (int)(sizeof(monsters)/sizeof(monsters[0]));
    
    /* Select a random monster from the array */
    int idx = rand() % mcount;
    Monster m = monsters[idx];  /* Copy monster data */

    /* Track monster's current HP (separate from template HP) */
    int mhp = m.hp;
    printf("A %s appears with %d HP!\n", m.name, mhp);

    /* Combat loop - continues until monster or player dies */
    while (mhp > 0 && player->health > 0)
    {
        /* Display current battle status */
        printf("Battle Status -- Your HP: %d | %s HP: %d\n", player->health, m.name, mhp);
        
        /* Prompt for player action */
        char command;
        printf("Enter 'A' to attack, 'Q' to flee, 'I' to use an item: ");
        if (scanf(" %c", &command) != 1) {
            /* Input failure: skip this turn and try again */
            continue;
        }
        /* Convert to uppercase for case-insensitive matching */
        command = (char)toupper((unsigned char)command);

        /* Process player action */
        switch (command)
        {
        case 'A':  /* ATTACK - Deal damage to the monster */
        {
            /* Calculate player's attack damage
             * Base damage + random roll (0-5) for variety */
            int p_roll = rand() % 6;
            int p_attack = player->total_damage + p_roll;

            /* Calculate actual damage dealt (attack - defense)
             * Minimum 1 damage to prevent zero-damage attacks */
            int dmg_to_mon = p_attack - m.defense;
            if (dmg_to_mon < 1) dmg_to_mon = 1;

            /* Apply damage to monster */
            mhp -= dmg_to_mon;
            if (mhp < 0) mhp = 0;  /* Don't show negative HP */
            printf("You hit the %s for %d. Monster HP: %d\n", m.name, dmg_to_mon, mhp);

            /* Monster counterattacks only if still alive
             * If monster died, skip to victory (handled after switch) */
            if (mhp > 0) {
                /* Calculate monster's attack damage
                 * Base attack + random roll (0-3) */
                int m_roll = rand() % 4;
                int m_attack = m.attack + m_roll;
                
                /* Calculate damage to player (attack - defense)
                 * Minimum 1 damage */
                int dmg_to_player = m_attack - player->total_defense;
                if (dmg_to_player < 1) dmg_to_player = 1;

                /* Apply damage to player */
                player->health -= dmg_to_player;
                if (player->health < 0) player->health = 0;  /* Don't show negative HP */
                printf("The %s hits you for %d. Your HP: %d\n", m.name, dmg_to_player, player->health);
            }
            break;
        }

        case 'Q':  /* FLEE - Attempt to escape from combat */
            /* 20% chance to successfully flee */
            if (rand() % 100 < 20) {
                printf("You successfully fled from the %s!\n", m.name);
                return 0;  /* Exit combat with no loot */
            } else {
                printf("You failed to flee!\n");
                
                /* Monster gets a free attack as punishment for failed flee */
                int m_roll = rand() % 4;
                int m_attack = m.attack + m_roll;
                int dmg_to_player = m_attack - player->total_defense;
                if (dmg_to_player < 1) dmg_to_player = 1;

                player->health -= dmg_to_player;
                if (player->health < 0) player->health = 0;
                printf("The %s punishes your retreat for %d. Your HP: %d\n", m.name, dmg_to_player, player->health);
            }
            break;

        case 'I':  /* USE ITEM - Consume a potion or other usable item */
            if (player_use_item(player)) {
                /* Successfully used an item
                 * This consumes your turn - monster doesn't attack this round
                 * Continue skips the rest of the loop iteration */
                continue;
            } else {
                /* No items available or action cancelled
                 * This doesn't consume your turn - prompt for action again */
                printf("No usable items available!\n");
            }
            break;

        default:  /* INVALID COMMAND */
            printf("Invalid command. Please try again.\n");
            continue;  /* Don't advance combat, just re-prompt */
        }

        /* Check if monster was defeated this turn */
        if (mhp <= 0) {
            printf("You have defeated the %s!\n", m.name);
            
            /* Calculate gold loot - random amount between min and max */
            int loot = m.min_loot + (rand() % (m.max_loot - m.min_loot + 1));
            
            /* Award experience points (may trigger level up) */
            player_gain_exp(player, m.exp_reward);

            /* Random item drops - 5% chance to drop equipment or consumables
             * This gives players a reason to fight more monsters */
            if (rand() % 100 < 5) {
                Item drop;
                int drop_type = rand() % 100;  /* Roll for item type */

                /* 40% chance: Health Potion (consumable) */
                if (drop_type < 40) {
                    drop = (Item){10, ITEM_CONSUMABLE, "Health Potion", 1, (ItemStats){0, 0}, 10};
                } 
                /* 20% chance: Weapon with random damage bonus */
                else if (drop_type < 60) {
                    const char *weapon_names[] = {"Iron Sword", "Steel Axe", "War Hammer", "Enchanted Blade"};
                    int wpn_idx = rand() % 4;  /* Pick random weapon type */
                    int dmg_bonus = 8 + rand() % 10;  /* Damage bonus: 8-17 */
                    drop = (Item){20 + wpn_idx, ITEM_WEAPON, weapon_names[wpn_idx], 1, (ItemStats){dmg_bonus, 0}, 20 + dmg_bonus * 2};
                }
                /* 20% chance: Armor with random defense bonus */
                else if (drop_type < 80) {
                    const char *armor_names[] = {"Leather Armor", "Chain Mail", "Plate Armor", "Dragon Scale"};
                    int arm_idx = rand() % 4;  /* Pick random armor type */
                    int def_bonus = 4 + rand() % 8;  /* Defense bonus: 4-11 */
                    drop = (Item){30 + arm_idx, ITEM_ARMOR, armor_names[arm_idx], 1, (ItemStats){0, def_bonus}, 15 + def_bonus * 2};
                }
                /* 20% chance: Valuable gem (misc item) */
                else {
                    drop = (Item){40, ITEM_MISC, "Gem", 1, (ItemStats){0, 0}, 50 + rand() % 50};
                }

                /* Add the item to player's inventory
                 * Note: We pass the address of 'drop' so player_add_item copies it */
                player_add_item(player, &drop);
            }

            /* Return gold amount looted */
            return loot;
        }
    }

    /* Combat ended without victory (player died or fled)
     * Return 0 gold */
    return 0;
}