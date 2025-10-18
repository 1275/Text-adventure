#ifndef PLAYER_H
#define PLAYER_H

enum { MAX_INVENTORY = 24, INVALID_SLOT = -1 };

typedef enum {
    ITEM_CONSUMABLE,
    ITEM_WEAPON,
    ITEM_ARMOR,
    ITEM_MISC
} ItemType;

typedef struct {
    int damage;
    int defense;
} ItemStats;

typedef struct {
    int id;
    ItemType type;
    const char *name;
    int quantity;
    ItemStats stats;
    int value;
} Item;

typedef struct {
    int weapon_slot; // index into inventory or -1
    int armor_slot;  // index into inventory or -1
} Equipment;

typedef struct {
    // Core resources
    int max_health;
    int health;
    int gold;

    // Base stats (without equipment)
    int base_damage;
    int base_defense;

    // Derived stats (with equipment)
    int total_damage;
    int total_defense;

    // Inventory & equipment
    Item inventory[MAX_INVENTORY];
    int inv_count;
    Equipment equipped;
} Player;

// Init with starter gear and apply equipment
void player_init(Player *p);

// Recompute total_damage/total_defense from base + equipped items
void player_apply_equipment(Player *p);

// Utility display
void player_print_status(const Player *p);
void player_print_inventory(const Player *p);

#endif