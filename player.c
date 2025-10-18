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
    printf("HP: %d/%d, Gold: %d, Dmg: %d, Def: %d\n",
           p->health, p->max_health, p->gold, p->total_damage, p->total_defense);
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