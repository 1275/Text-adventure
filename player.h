#ifndef PLAYER_H
#define PLAYER_H

enum { MAX_INVENTORY = 24, INVALID_SLOT = -1 };

typedef enum {
    CLASS_WARRIOR,
    CLASS_MAGE
} PlayerClass;

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
    // Player class
    PlayerClass player_class;
    
    // Core resources
    int max_health;
    int health;
    int gold;

    // Experience and leveling
    int level;
    int experience;
    int exp_to_next_level;

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

// Initialization and equipment
void player_init(Player *p, PlayerClass class);
void player_apply_equipment(Player *p);

// Utility function for class name
const char* player_class_name(PlayerClass class);

// Experience and leveling
void player_gain_exp(Player *p, int exp);
void player_level_up(Player *p);

// Inventory management
int player_add_item(Player *p, const Item *item);
int player_use_item(Player *p);  // Returns 1 if item was used, 0 otherwise
void player_equip_item(Player *p, int slot);

// Utility display
void player_print_status(const Player *p);
void player_print_inventory(const Player *p);

#endif