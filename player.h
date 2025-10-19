#ifndef PLAYER_H
#define PLAYER_H

/*
 * Player inventory constants
 * MAX_INVENTORY: Maximum number of items a player can carry
 * INVALID_SLOT: Used to indicate no item is equipped in a slot
 */
enum { MAX_INVENTORY = 24, INVALID_SLOT = -1 };

/*
 * ItemType: Categories of items in the game
 * ITEM_CONSUMABLE: Items that can be used (like potions)
 * ITEM_WEAPON: Weapons that increase attack damage
 * ITEM_ARMOR: Armor that increases defense
 * ITEM_MISC: Other items (like gems, quest items)
 */
typedef enum {
    ITEM_CONSUMABLE,
    ITEM_WEAPON,
    ITEM_ARMOR,
    ITEM_MISC
} ItemType;

/*
 * ItemStats: Bonuses provided by an item when equipped
 * damage: Additional attack power
 * defense: Additional defense against attacks
 */
typedef struct {
    int damage;
    int defense;
} ItemStats;

/*
 * Item: Represents a single item in the game
 * id: Unique identifier for the item
 * type: Category of item (consumable, weapon, armor, misc)
 * name: Display name of the item
 * quantity: How many of this item (mainly for consumables)
 * stats: Stat bonuses this item provides
 * value: Gold value of the item
 */
typedef struct {
    int id;
    ItemType type;
    const char *name;
    int quantity;
    ItemStats stats;
    int value;
} Item;

/*
 * Equipment: Tracks which inventory slots contain equipped items
 * weapon_slot: Index into inventory array for equipped weapon (or INVALID_SLOT)
 * armor_slot: Index into inventory array for equipped armor (or INVALID_SLOT)
 */
typedef struct {
    int weapon_slot; // index into inventory or -1
    int armor_slot;  // index into inventory or -1
} Equipment;

/*
 * Player: Contains all player state and statistics
 * This is the main structure representing the player character
 */
typedef struct {
    // Core resources - health and currency
    int max_health;              // Maximum HP the player can have
    int health;                  // Current HP (player dies at 0)
    int gold;                    // Currency collected

    // Experience and leveling system
    int level;                   // Current character level (starts at 1)
    int experience;              // Current XP points
    int exp_to_next_level;       // XP needed to reach next level

    // Base stats (without equipment bonuses)
    int base_damage;             // Attack power from level alone
    int base_defense;            // Defense from level alone

    // Derived stats (base + equipment bonuses)
    int total_damage;            // Final attack power used in combat
    int total_defense;           // Final defense used in combat

    // Inventory & equipment system
    Item inventory[MAX_INVENTORY]; // Array of items player is carrying
    int inv_count;                 // Number of items currently in inventory
    Equipment equipped;            // Which items are currently equipped
} Player;

/*
 * Player Initialization and Equipment Functions
 */

/* Initialize a new player with starting stats and equipment */
void player_init(Player *p);

/* Recalculate total_damage and total_defense based on equipped items */
void player_apply_equipment(Player *p);

/*
 * Experience and Leveling Functions
 */

/* Award experience points to player and handle level ups */
void player_gain_exp(Player *p, int exp);

/* Level up the player, increasing stats and fully healing */
void player_level_up(Player *p);

/*
 * Inventory Management Functions
 */

/* Add an item to inventory; returns 1 on success, 0 if inventory full */
int player_add_item(Player *p, const Item *item);

/* Use a consumable item from inventory; returns 1 if used, 0 otherwise */
int player_use_item(Player *p);

/* Equip a weapon or armor from the given inventory slot */
void player_equip_item(Player *p, int slot);

/*
 * Display Functions
 */

/* Display player's current HP, gold, damage, defense, level, and XP */
void player_print_status(const Player *p);

/* Display all items in inventory with equipment indicators */
void player_print_inventory(const Player *p);

#endif