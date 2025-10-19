#include <stdio.h>
#include "player.h"

static const char* item_type_name(ItemType t) {
    switch (t) {
        case ITEM_CONSUMABLE: return "Consumable";
        case ITEM_WEAPON:     return "Weapon";
        case ITEM_ARMOR:      return "Armor";
        case ITEM_MISC:       return "Misc";
        default:              return "Unknown";
    }
}

void player_apply_equipment(Player *p) {
    p->total_damage  = p->base_damage;
    p->total_defense = p->base_defense;

    if (p->equipped.weapon_slot != INVALID_SLOT) {
        const Item *w = &p->inventory[p->equipped.weapon_slot];
        p->total_damage  += w->stats.damage;
        p->total_defense += w->stats.defense;
    }
    if (p->equipped.armor_slot != INVALID_SLOT) {
        const Item *a = &p->inventory[p->equipped.armor_slot];
        p->total_damage  += a->stats.damage;
        p->total_defense += a->stats.defense;
    }
}

void player_init(Player *p) {
    p->max_health   = 100;
    p->health       = p->max_health;
    p->gold         = 0;

    p->level        = 1;
    p->experience   = 0;
    p->exp_to_next_level = 100;

    p->base_damage  = 10;
    p->base_defense = 5;

    // Starter inventory
    p->inventory[0] = (Item){1, ITEM_WEAPON,    "Rusty Sword",  1, {6, 0},  5};
    p->inventory[1] = (Item){2, ITEM_CONSUMABLE,"Small Potion", 3, {0, 0},  3};
    p->inventory[2] = (Item){3, ITEM_ARMOR,     "Cloth Tunic",  1, {0, 2},  4};
    p->inv_count = 3;

    p->equipped.weapon_slot = 0;
    p->equipped.armor_slot  = 2;

    player_apply_equipment(p);
}

void player_print_status(const Player *p) {
    printf("HP: %d/%d, Gold: %d, Dmg: %d, Def: %d | Level: %d, XP: %d/%d\n",
           p->health, p->max_health, p->gold, p->total_damage, p->total_defense,
           p->level, p->experience, p->exp_to_next_level);
}

void player_print_inventory(const Player *p) {
    printf("\nInventory (E = equipped):\n");
    for (int i = 0; i < p->inv_count; ++i) {
        int is_equipped = (p->equipped.weapon_slot == i) || (p->equipped.armor_slot == i);
        const Item *it = &p->inventory[i];
        printf("  [%2d]%s %-14s x%-2d  %-10s",
               i,
               is_equipped ? " [E]" : "    ",
               it->name,
               it->quantity,
               item_type_name(it->type));
        if (it->stats.damage || it->stats.defense) {
            printf("  (dmg:%d def:%d)", it->stats.damage, it->stats.defense);
        }
        printf("  value:%d\n", it->value);
    }
    printf("\n");
}

void player_level_up(Player *p) {
    p->level++;
    
    // Stat increases per level
    p->max_health += 20;
    p->health = p->max_health; // Full heal on level up
    p->base_damage += 3;
    p->base_defense += 2;
    
    // Calculate next level requirement (exponential growth)
    p->exp_to_next_level = 100 + (p->level - 1) * 50;
    
    printf("\n*** LEVEL UP! You are now level %d! ***\n", p->level);
    printf("Max HP +20 (now %d), Damage +3 (now %d), Defense +2 (now %d)\n",
           p->max_health, p->base_damage, p->base_defense);
    printf("HP fully restored!\n\n");
    
    player_apply_equipment(p);
}

void player_gain_exp(Player *p, int exp) {
    p->experience += exp;
    printf("You gained %d experience! (%d/%d)\n", exp, p->experience, p->exp_to_next_level);
    
    while (p->experience >= p->exp_to_next_level) {
        p->experience -= p->exp_to_next_level;
        player_level_up(p);
    }
}