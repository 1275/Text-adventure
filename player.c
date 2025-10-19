/*
 * player.c - Player Character Management
 * 
 * This file handles all player-related functionality:
 * - Character initialization with starting stats
 * - Experience and leveling system
 * - Inventory management (adding, using, equipping items)
 * - Equipment system with stat bonuses
 * - Status display functions
 */

#include <stdio.h>
#include "player.h"

/*
 * Helper function: Convert item type enum to readable string
 * Used for displaying inventory contents
 */
static const char* item_type_name(ItemType t) {
    switch (t) {
        case ITEM_CONSUMABLE: return "Consumable";
        case ITEM_WEAPON:     return "Weapon";
        case ITEM_ARMOR:      return "Armor";
        case ITEM_MISC:       return "Misc";
        default:              return "Unknown";
    }
}

/**
 * Recalculate player's total damage and defense based on base stats + equipped items
 * 
 * This should be called whenever:
 * - Player equips/unequips an item
 * - Player levels up (base stats change)
 * - Player's equipment is modified
 */
void player_apply_equipment(Player *p) {
    /* Start with base stats from character level
     * These improve when player levels up */
    p->total_damage  = p->base_damage;
    p->total_defense = p->base_defense;

    /* Add weapon stat bonuses if a weapon is equipped
     * Weapons primarily increase damage, but may also add defense */
    if (p->equipped.weapon_slot != INVALID_SLOT) {
        const Item *w = &p->inventory[p->equipped.weapon_slot];
        p->total_damage  += w->stats.damage;   /* Add weapon's damage bonus */
        p->total_defense += w->stats.defense;  /* Add weapon's defense bonus (if any) */
    }
    
    /* Add armor stat bonuses if armor is equipped
     * Armor primarily increases defense, but may also add damage */
    if (p->equipped.armor_slot != INVALID_SLOT) {
        const Item *a = &p->inventory[p->equipped.armor_slot];
        p->total_damage  += a->stats.damage;   /* Add armor's damage bonus (if any) */
        p->total_defense += a->stats.defense;  /* Add armor's defense bonus */
    }
}

/**
 * Initialize player with starting stats and equipment
 * 
 * Sets up a new player character ready for adventure with:
 * - Starting health and resources
 * - Level 1 with base stats
 * - Starter weapon, armor, and potions
 */
void player_init(Player *p) {
    /* Set initial health and resources */
    p->max_health   = 100;              /* Starting max HP */
    p->health       = p->max_health;    /* Start at full health */
    p->gold         = 0;                /* No starting gold */

    /* Set starting level and experience system */
    p->level        = 1;                /* Start at level 1 */
    p->experience   = 0;                /* No starting XP */
    p->exp_to_next_level = 100;         /* Need 100 XP to reach level 2 */

    /* Set base combat stats (before equipment bonuses)
     * These values will increase as player levels up */
    p->base_damage  = 10;               /* Base attack power */
    p->base_defense = 5;                /* Base defense */

    /* Give starter items to new players
     * Format: {ID, Type, Name, Quantity, {Damage, Defense}, Value}
     * These provide a foundation for early game survival */
    p->inventory[0] = (Item){1, ITEM_WEAPON,    "Rusty Sword",  1, {6, 0},  5};  /* Basic weapon */
    p->inventory[1] = (Item){2, ITEM_CONSUMABLE,"Small Potion", 3, {0, 0},  3};  /* 3 healing potions */
    p->inventory[2] = (Item){3, ITEM_ARMOR,     "Cloth Tunic",  1, {0, 2},  4};  /* Basic armor */
    p->inv_count = 3;  /* 3 items in inventory */

    /* Equip the starter weapon and armor
     * Slot 0 = Rusty Sword, Slot 2 = Cloth Tunic */
    p->equipped.weapon_slot = 0;  /* Equip Rusty Sword */
    p->equipped.armor_slot  = 2;  /* Equip Cloth Tunic */

    /* Calculate final stats including equipment bonuses
     * This sets total_damage and total_defense */
    player_apply_equipment(p);
}

/**
 * Display current player status (HP, gold, stats, level)
 */
void player_print_status(const Player *p) {
    printf("HP: %d/%d, Gold: %d, Dmg: %d, Def: %d | Level: %d, XP: %d/%d\n",
           p->health, p->max_health, p->gold, p->total_damage, p->total_defense,
           p->level, p->experience, p->exp_to_next_level);
}

/**
 * Display all items in inventory with equipment markers
 */
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
               
        // Show stat bonuses for weapons/armor
        if (it->stats.damage || it->stats.defense) {
            printf("  (dmg:%d def:%d)", it->stats.damage, it->stats.defense);
        }
        printf("  value:%d\n", it->value);
    }
    printf("\n");
}

/**
 * Level up the player, increasing stats and healing to full
 * 
 * Called when player gains enough experience points
 * Provides significant power increases and full heal
 */
void player_level_up(Player *p) {
    p->level++;  /* Increment character level */
    
    /* Apply stat increases per level
     * These bonuses make the player progressively stronger */
    p->max_health += 20;              /* +20 max HP per level */
    p->health = p->max_health;        /* Full heal as level up bonus */
    p->base_damage += 3;              /* +3 attack per level */
    p->base_defense += 2;             /* +2 defense per level */
    
    /* Calculate XP needed for next level
     * Formula: 100 + (level-1) * 50
     * Level 2 needs 100 XP, Level 3 needs 150 XP, Level 4 needs 200 XP, etc.
     * This creates exponential growth - higher levels take more XP */
    p->exp_to_next_level = 100 + (p->level - 1) * 50;
    
    /* Display level up message with new stats */
    printf("\n*** LEVEL UP! You are now level %d! ***\n", p->level);
    printf("Max HP +20 (now %d), Damage +3 (now %d), Defense +2 (now %d)\n",
           p->max_health, p->base_damage, p->base_defense);
    printf("HP fully restored!\n\n");
    
    /* Recalculate total stats including equipment bonuses
     * Base stats changed, so totals need updating */
    player_apply_equipment(p);
}

/**
 * Award experience points and handle level ups
 * 
 * Adds XP to player and automatically levels up if threshold reached
 * Can handle multiple level ups if enough XP is awarded at once
 */
void player_gain_exp(Player *p, int exp) {
    /* Add experience points */
    p->experience += exp;
    printf("You gained %d experience! (%d/%d)\n", exp, p->experience, p->exp_to_next_level);
    
    /* Check if player has enough XP to level up
     * Loop handles case where player gains enough XP for multiple levels
     * (e.g., defeating a boss might give enough XP to jump 2 levels) */
    while (p->experience >= p->exp_to_next_level) {
        /* Subtract XP cost from current XP (carry over excess)
         * Example: If you need 100 XP and have 150, you'll have 50 XP toward next level */
        p->experience -= p->exp_to_next_level;
        
        /* Perform level up (increases stats, heals to full) */
        player_level_up(p);
    }
}

/**
 * Add an item to inventory, stacking consumables if possible
 * 
 * Returns: 1 on success, 0 if inventory full
 * 
 * Behavior:
 * - Consumables with same ID stack together (increase quantity)
 * - Weapons and armor create new inventory slots
 * - Fails if inventory is at maximum capacity (24 slots)
 */
int player_add_item(Player *p, const Item *item) {
    /* Check if inventory has reached maximum capacity */
    if (p->inv_count >= MAX_INVENTORY) {
        printf("Your inventory is full! Cannot pick up %s.\n", item->name);
        return 0;  /* Failure - inventory full */
    }
    
    /* Try to stack consumables with existing items of same type
     * This prevents inventory clutter from multiple potions */
    if (item->type == ITEM_CONSUMABLE) {
        /* Search for existing item with same ID */
        for (int i = 0; i < p->inv_count; i++) {
            if (p->inventory[i].id == item->id) {
                /* Found matching item - add to its quantity instead of creating new slot */
                p->inventory[i].quantity += item->quantity;
                printf("Picked up %s x%d (now have %d)\n", 
                       item->name, item->quantity, p->inventory[i].quantity);
                return 1;  /* Success - stacked with existing item */
            }
        }
    }
    
    /* Item doesn't stack or is not consumable - add as new inventory entry
     * Copy the entire item structure into the next available slot */
    p->inventory[p->inv_count] = *item;  /* Note: *item copies the struct by value */
    printf("Picked up %s!\n", item->name);
    p->inv_count++;  /* Increment item count */
    return 1;  /* Success - new item added */
}

/**
 * Equip a weapon or armor from inventory
 * 
 * Validates slot, checks item type, and updates equipped slots
 * Automatically unequips old item in the same slot
 * Recalculates stats to apply new equipment bonuses
 */
void player_equip_item(Player *p, int slot) {
    /* Validate that slot number is within inventory bounds */
    if (slot < 0 || slot >= p->inv_count) {
        printf("Invalid inventory slot!\n");
        return;
    }
    
    /* Get pointer to the item at specified slot */
    Item *item = &p->inventory[slot];
    
    /* Handle weapon equipping */
    if (item->type == ITEM_WEAPON) {
        /* If a different weapon is already equipped, announce unequipping
         * (Don't announce if re-equipping the same weapon) */
        if (p->equipped.weapon_slot != INVALID_SLOT && p->equipped.weapon_slot != slot) {
            printf("Unequipped %s\n", p->inventory[p->equipped.weapon_slot].name);
        }
        /* Set this slot as the equipped weapon */
        p->equipped.weapon_slot = slot;
        printf("Equipped %s (Damage +%d)\n", item->name, item->stats.damage);
    } 
    /* Handle armor equipping */
    else if (item->type == ITEM_ARMOR) {
        /* If different armor is already equipped, announce unequipping */
        if (p->equipped.armor_slot != INVALID_SLOT && p->equipped.armor_slot != slot) {
            printf("Unequipped %s\n", p->inventory[p->equipped.armor_slot].name);
        }
        /* Set this slot as the equipped armor */
        p->equipped.armor_slot = slot;
        printf("Equipped %s (Defense +%d)\n", item->name, item->stats.defense);
    } 
    /* Item is not equippable (consumable or misc) */
    else {
        printf("Cannot equip %s (not a weapon or armor)\n", item->name);
        return;
    }
    
    /* Recalculate combat stats to include new equipment bonuses
     * This updates total_damage and total_defense */
    player_apply_equipment(p);
}

/**
 * Use a consumable item from inventory
 * 
 * Returns: 1 if an item was used, 0 otherwise
 * 
 * Process:
 * 1. Display all consumable items
 * 2. Prompt user for slot selection
 * 3. Apply item effect (healing)
 * 4. Decrease quantity
 * 5. Remove from inventory if quantity reaches 0
 * 6. Update equipment slots if inventory shifted
 */
int player_use_item(Player *p) {
    /* Display all available consumable items */
    int consumable_count = 0;
    printf("\nAvailable consumables:\n");
    for (int i = 0; i < p->inv_count; i++) {
        /* Only show consumables with quantity > 0 */
        if (p->inventory[i].type == ITEM_CONSUMABLE && p->inventory[i].quantity > 0) {
            printf("  [%d] %s x%d\n", i, p->inventory[i].name, p->inventory[i].quantity);
            consumable_count++;
        }
    }
    
    /* Check if player has any consumables */
    if (consumable_count == 0) {
        return 0;  /* No items available */
    }
    
    /* Prompt user to select an item slot */
    printf("Enter slot number to use (or -1 to cancel): ");
    int slot;
    if (scanf("%d", &slot) != 1) {
        return 0;  /* Input error */
    }
    
    /* Allow player to cancel item use */
    if (slot == -1) {
        printf("Cancelled.\n");
        return 0;
    }
    
    /* Validate slot is within inventory bounds */
    if (slot < 0 || slot >= p->inv_count) {
        printf("Invalid slot!\n");
        return 0;
    }
    
    /* Get pointer to selected item */
    Item *item = &p->inventory[slot];
    
    /* Verify item is consumable and has quantity remaining */
    if (item->type != ITEM_CONSUMABLE || item->quantity <= 0) {
        printf("That item cannot be used!\n");
        return 0;
    }
    
    /* Apply item effect (currently all consumables heal HP)
     * Healing amount scales with item value: base 30 + (value * 2)
     * More valuable potions heal more */
    int heal_amount = 30 + (item->value * 2);
    p->health += heal_amount;
    /* Cap health at maximum (no overhealing) */
    if (p->health > p->max_health) {
        p->health = p->max_health;
    }
    
    printf("Used %s! Restored %d HP. Current HP: %d/%d\n", 
           item->name, heal_amount, p->health, p->max_health);
    
    /* Decrease item quantity */
    item->quantity--;
    
    /* If item is depleted, remove it from inventory
     * This requires shifting all items after it down by one slot */
    if (item->quantity <= 0) {
        printf("%s depleted!\n", item->name);
        
        /* Shift all items after this one down to fill the gap
         * Example: If slot 2 is removed, move slot 3->2, 4->3, etc. */
        for (int i = slot; i < p->inv_count - 1; i++) {
            p->inventory[i] = p->inventory[i + 1];
            
            /* Update equipment slot references if they shifted down
             * If weapon/armor was in slot i+1, it's now in slot i */
            if (p->equipped.weapon_slot == i + 1) p->equipped.weapon_slot = i;
            if (p->equipped.armor_slot == i + 1) p->equipped.armor_slot = i;
        }
        
        /* Decrease total item count */
        p->inv_count--;
        
        /* If the removed item was equipped (shouldn't happen with consumables,
         * but handle it for safety), mark that slot as empty */
        if (p->equipped.weapon_slot == slot) p->equipped.weapon_slot = INVALID_SLOT;
        if (p->equipped.armor_slot == slot) p->equipped.armor_slot = INVALID_SLOT;
    }
    
    return 1;  /* Success - item was used */
}