#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "enemies.h"
#include "player.h"

// Helper function to generate a monster instance based on player level
static Monster generate_monster(const MonsterTemplate *template, int player_level)
{
    Monster m;
    
    // Calculate monster level within the allowed range
    int level_min = player_level + template->level_offset_min;
    int level_max = player_level + template->level_offset_max;
    
    // Ensure minimum level of 1
    if (level_min < 1) level_min = 1;
    if (level_max < 1) level_max = 1;
    
    // Randomize within the level range
    m.level = level_min + (rand() % (level_max - level_min + 1));
    
    // Calculate stats based on level with some randomization (±20% variance)
    int hp_base = template->base_hp + (m.level - 1) * template->hp_per_level;
    int hp_variance = hp_base / 5; // ±20%
    m.hp = hp_base + (rand() % (hp_variance * 2 + 1)) - hp_variance;
    if (m.hp < 1) m.hp = 1;
    
    int attack_base = template->base_attack + (m.level - 1) * template->attack_per_level;
    int attack_variance = attack_base / 5;
    m.attack = attack_base + (rand() % (attack_variance * 2 + 1)) - attack_variance;
    if (m.attack < 1) m.attack = 1;
    
    int defense_base = template->base_defense + (m.level - 1) * template->defense_per_level;
    int defense_variance = defense_base / 5;
    m.defense = defense_base + (rand() % (defense_variance * 2 + 1)) - defense_variance;
    if (m.defense < 0) m.defense = 0;
    
    m.name = template->name;
    m.min_loot = template->min_loot;
    m.max_loot = template->max_loot;
    m.exp_reward = template->exp_reward_base + (m.level - 1) * 10; // Exp scales with level
    
    return m;
}

int battle_monster(Player *player)
{
    // Monster templates with level ranges and scaling
    static const MonsterTemplate templates[] = {
        {
            .name = "Goblin",
            .level_offset_min = -2,
            .level_offset_max = 2,
            .base_hp = 25,
            .hp_per_level = 8,
            .base_attack = 5,
            .attack_per_level = 2,
            .base_defense = 0,
            .defense_per_level = 1,
            .min_loot = 8,
            .max_loot = 20,
            .exp_reward_base = 20
        },
        {
            .name = "Skeleton",
            .level_offset_min = -1,
            .level_offset_max = 3,
            .base_hp = 35,
            .hp_per_level = 10,
            .base_attack = 7,
            .attack_per_level = 2,
            .base_defense = 1,
            .defense_per_level = 1,
            .min_loot = 12,
            .max_loot = 26,
            .exp_reward_base = 30
        },
        {
            .name = "Giant Spider",
            .level_offset_min = 0,
            .level_offset_max = 3,
            .base_hp = 40,
            .hp_per_level = 12,
            .base_attack = 8,
            .attack_per_level = 3,
            .base_defense = 2,
            .defense_per_level = 1,
            .min_loot = 15,
            .max_loot = 30,
            .exp_reward_base = 40
        },
        {
            .name = "Orc",
            .level_offset_min = 0,
            .level_offset_max = 4,
            .base_hp = 50,
            .hp_per_level = 15,
            .base_attack = 10,
            .attack_per_level = 3,
            .base_defense = 2,
            .defense_per_level = 1,
            .min_loot = 20,
            .max_loot = 35,
            .exp_reward_base = 50
        },
        {
            .name = "Troll",
            .level_offset_min = 1,
            .level_offset_max = 5,
            .base_hp = 70,
            .hp_per_level = 20,
            .base_attack = 12,
            .attack_per_level = 4,
            .base_defense = 3,
            .defense_per_level = 2,
            .min_loot = 30,
            .max_loot = 45,
            .exp_reward_base = 70
        },
        {
            .name = "Dark Knight",
            .level_offset_min = 2,
            .level_offset_max = 6,
            .base_hp = 90,
            .hp_per_level = 25,
            .base_attack = 15,
            .attack_per_level = 5,
            .base_defense = 5,
            .defense_per_level = 2,
            .min_loot = 40,
            .max_loot = 60,
            .exp_reward_base = 100
        },
        {
            .name = "Dragon",
            .level_offset_min = 3,
            .level_offset_max = 7,
            .base_hp = 120,
            .hp_per_level = 30,
            .base_attack = 18,
            .attack_per_level = 6,
            .base_defense = 7,
            .defense_per_level = 2,
            .min_loot = 60,
            .max_loot = 100,
            .exp_reward_base = 180
        }
    };
    const int tcount = (int)(sizeof(templates)/sizeof(templates[0]));
    
    // Select a random monster template
    int idx = rand() % tcount;
    
    // Generate the actual monster based on player level
    Monster m = generate_monster(&templates[idx], player->level);
    
    int mhp = m.hp;
    printf("\nA level %d %s appears with %d HP!\n", m.level, m.name, mhp);
    printf("(Attack: %d, Defense: %d)\n", m.attack, m.defense);

    while (mhp > 0 && player->health > 0)
    {
        printf("Battle Status -- Your HP: %d | %s HP: %d\n", player->health, m.name, mhp);
        char command;
        printf("Enter 'A' to attack, 'Q' to flee, 'I' to use an item: ");
        if (scanf(" %c", &command) != 1) {
            // Input failure: treat as no-op
            continue;
        }
        command = (char)toupper((unsigned char)command);

        switch (command)
        {
        case 'A': {
            int p_roll = rand() % 6; // 0..5
            int p_attack = player->total_damage + p_roll;

            int dmg_to_mon = p_attack - m.defense;
            if (dmg_to_mon < 1) dmg_to_mon = 1;

            mhp -= dmg_to_mon;
            if (mhp < 0) mhp = 0;
            printf("You hit the %s for %d. Monster HP: %d\n", m.name, dmg_to_mon, mhp);

            // If monster died, skip its counterattack; victory handled after switch
            if (mhp > 0) {
                int m_roll = rand() % 4; // 0..3
                int m_attack = m.attack + m_roll;
                int dmg_to_player = m_attack - player->total_defense;
                if (dmg_to_player < 1) dmg_to_player = 1;

                player->health -= dmg_to_player;
                if (player->health < 0) player->health = 0;
                printf("The %s hits you for %d. Your HP: %d\n", m.name, dmg_to_player, player->health);
            }
            break;
        }

        case 'Q':
            if (rand() % 100 < 20) { // 20% flee chance
                printf("You successfully fled from the %s!\n", m.name);
                return 0;
            } else {
                printf("You failed to flee!\n");
                // Monster gets a free hit on failed flee
                int m_roll = rand() % 4;
                int m_attack = m.attack + m_roll;
                int dmg_to_player = m_attack - player->total_defense;
                if (dmg_to_player < 1) dmg_to_player = 1;

                player->health -= dmg_to_player;
                if (player->health < 0) player->health = 0;
                printf("The %s punishes your retreat for %d. Your HP: %d\n", m.name, dmg_to_player, player->health);
            }
            break;

        case 'I':
            if (player_use_item(player)) {
                // Item use consumes your turn; monster does not attack this round
                continue;
            } else {
                printf("No usable items available!\n");
            }
            break;

        default:
            printf("Invalid command. Please try again.\n");
            continue;
        }

        if (mhp <= 0) {
            printf("You have defeated the %s!\n", m.name);
            int loot = m.min_loot + (rand() % (m.max_loot - m.min_loot + 1));
            player_gain_exp(player, m.exp_reward);

            // Random item drops (5% chance)
            if (rand() % 100 < 5) {
                Item drop;
                int drop_type = rand() % 100;

                if (drop_type < 40) {
                    drop = (Item){10, ITEM_CONSUMABLE, "Health Potion", 1, (ItemStats){0, 0}, 10};
                } else if (drop_type < 60) {
                    const char *weapon_names[] = {"Iron Sword", "Steel Axe", "War Hammer", "Enchanted Blade"};
                    int wpn_idx = rand() % 4;
                    int dmg_bonus = 8 + rand() % 10;
                    drop = (Item){20 + wpn_idx, ITEM_WEAPON, weapon_names[wpn_idx], 1, (ItemStats){dmg_bonus, 0}, 20 + dmg_bonus * 2};
                } else if (drop_type < 80) {
                    const char *armor_names[] = {"Leather Armor", "Chain Mail", "Plate Armor", "Dragon Scale"};
                    int arm_idx = rand() % 4;
                    int def_bonus = 4 + rand() % 8;
                    drop = (Item){30 + arm_idx, ITEM_ARMOR, armor_names[arm_idx], 1, (ItemStats){0, def_bonus}, 15 + def_bonus * 2};
                } else {
                    drop = (Item){40, ITEM_MISC, "Gem", 1, (ItemStats){0, 0}, 50 + rand() % 50};
                }

                // Ensure this copies the item (do not store the pointer!)
                player_add_item(player, &drop);
            }

            return loot;
        }
    }

    return 0;
}