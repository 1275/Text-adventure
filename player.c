#include <stdio.h>
#include "player.h"

// Helper: Convert item type to string for display
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
 */
void player_apply_equipment(Player *p) {
    // Start with base stats
    p->total_damage  = p->base_damage;
    p->total_defense = p->base_defense;

    // Add weapon bonuses if equipped
    if (p->equipped.weapon_slot != INVALID_SLOT) {
        const Item *w = &p->inventory[p->equipped.weapon_slot];
        p->total_damage  += w->stats.damage;
        p->total_defense += w->stats.defense;
    }
    
    // Add armor bonuses if equipped
    if (p->equipped.armor_slot != INVALID_SLOT) {
        const Item *a = &p->inventory[p->equipped.armor_slot];
        p->total_damage  += a->stats.damage;
        p->total_defense += a->stats.defense;
    }
}

/**
 * Class definitions - ADD NEW CLASSES HERE
 * To add a new class, simply add a new entry to this array
 */
static const ClassDefinition class_definitions[] = {
    {
        CLASS_WARRIOR,
        "Warrior",
        "A fierce melee fighter",
        100,  // max_health
        15,   // base_damage (+5 from default 10)
        8     // base_defense (+3 from default 5)
    },
    {
        CLASS_MAGE,
        "Mage",
        "A mystical spellcaster",
        120,  // max_health (+20 from default)
        8,    // base_damage (-2 from default)
        4     // base_defense (-1 from default)
    }
    // Add more classes here as needed
};

/**
 * Get all class definitions and their count
 */
const ClassDefinition* get_all_class_definitions(int *count) {
    *count = sizeof(class_definitions) / sizeof(class_definitions[0]);
    return class_definitions;
}

/**
 * Get a specific class definition
 */
const ClassDefinition* get_class_definition(PlayerClass class) {
    int count;
    const ClassDefinition *defs = get_all_class_definitions(&count);
    
    for (int i = 0; i < count; i++) {
        if (defs[i].class_type == class) {
            return &defs[i];
        }
    }
    
    return NULL;  // Class not found
}

/**
 * Get the name of a player class as a string
 */
const char* player_class_name(PlayerClass class) {
    const ClassDefinition *def = get_class_definition(class);
    return def ? def->name : "Unknown";
}

/**
 * Initialize player with starting stats and equipment based on class
 */
void player_init(Player *p, PlayerClass class) {
    // Set player class
    p->player_class = class;
    
    // Set starting level and experience
    p->level        = 1;
    p->experience   = 0;
    p->exp_to_next_level = 100;
    p->gold         = 0;

    // Get class definition and apply stats
    const ClassDefinition *class_def = get_class_definition(class);
    if (class_def) {
        p->max_health   = class_def->max_health;
        p->base_damage  = class_def->base_damage;
        p->base_defense = class_def->base_defense;
    } else {
        // Fallback to default stats if class not found
        p->max_health   = 100;
        p->base_damage  = 10;
        p->base_defense = 5;
    }
    
    p->health = p->max_health;

    // Give starter items
    p->inventory[0] = (Item){1, ITEM_WEAPON,    "Rusty Sword",  1, {6, 0},  5};
    p->inventory[1] = (Item){2, ITEM_CONSUMABLE,"Small Potion", 3, {0, 0},  3};
    p->inventory[2] = (Item){3, ITEM_ARMOR,     "Cloth Tunic",  1, {0, 2},  4};
    p->inv_count = 3;

    // Equip starter gear
    p->equipped.weapon_slot = 0;  // Rusty Sword
    p->equipped.armor_slot  = 2;  // Cloth Tunic

    // Calculate final stats with equipment
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
 */
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
    
    // Recalculate stats with equipment
    player_apply_equipment(p);
}

/**
 * Award experience points and handle level ups
 */
void player_gain_exp(Player *p, int exp) {
    p->experience += exp;
    printf("You gained %d experience! (%d/%d)\n", exp, p->experience, p->exp_to_next_level);
    
    // Handle multiple level ups if enough XP gained
    while (p->experience >= p->exp_to_next_level) {
        p->experience -= p->exp_to_next_level;
        player_level_up(p);
    }
}

/**
 * Add an item to inventory, stacking consumables if possible
 * Returns 1 on success, 0 if inventory full
 */
int player_add_item(Player *p, const Item *item) {
    // Check if inventory is full
    if (p->inv_count >= MAX_INVENTORY) {
        printf("Your inventory is full! Cannot pick up %s.\n", item->name);
        return 0;
    }
    
    // Try to stack with existing consumables
    if (item->type == ITEM_CONSUMABLE) {
        for (int i = 0; i < p->inv_count; i++) {
            if (p->inventory[i].id == item->id) {
                p->inventory[i].quantity += item->quantity;
                printf("Picked up %s x%d (now have %d)\n", 
                       item->name, item->quantity, p->inventory[i].quantity);
                return 1;
            }
        }
    }
    
    // Add new item to inventory
    p->inventory[p->inv_count] = *item;
    printf("Picked up %s!\n", item->name);
    p->inv_count++;
    return 1;
}

/**
 * Equip a weapon or armor from inventory
 */
void player_equip_item(Player *p, int slot) {
    if (slot < 0 || slot >= p->inv_count) {
        printf("Invalid inventory slot!\n");
        return;
    }
    
    Item *item = &p->inventory[slot];
    
    if (item->type == ITEM_WEAPON) {
        // Unequip old weapon if any
        if (p->equipped.weapon_slot != INVALID_SLOT && p->equipped.weapon_slot != slot) {
            printf("Unequipped %s\n", p->inventory[p->equipped.weapon_slot].name);
        }
        p->equipped.weapon_slot = slot;
        printf("Equipped %s (Damage +%d)\n", item->name, item->stats.damage);
    } 
    else if (item->type == ITEM_ARMOR) {
        // Unequip old armor if any
        if (p->equipped.armor_slot != INVALID_SLOT && p->equipped.armor_slot != slot) {
            printf("Unequipped %s\n", p->inventory[p->equipped.armor_slot].name);
        }
        p->equipped.armor_slot = slot;
        printf("Equipped %s (Defense +%d)\n", item->name, item->stats.defense);
    } 
    else {
        printf("Cannot equip %s (not a weapon or armor)\n", item->name);
        return;
    }
    
    // Recalculate stats with new equipment
    player_apply_equipment(p);
}

/**
 * Use a consumable item from inventory
 * Returns 1 if an item was used, 0 otherwise
 */
int player_use_item(Player *p) {
    // Show available consumables
    int consumable_count = 0;
    printf("\nAvailable consumables:\n");
    for (int i = 0; i < p->inv_count; i++) {
        if (p->inventory[i].type == ITEM_CONSUMABLE && p->inventory[i].quantity > 0) {
            printf("  [%d] %s x%d\n", i, p->inventory[i].name, p->inventory[i].quantity);
            consumable_count++;
        }
    }
    
    // No consumables available
    if (consumable_count == 0) {
        return 0;
    }
    
    // Get user choice
    printf("Enter slot number to use (or -1 to cancel): ");
    int slot;
    if (scanf("%d", &slot) != 1) {
        return 0;
    }
    
    // Validate choice
    if (slot == -1) {
        printf("Cancelled.\n");
        return 0;
    }
    
    if (slot < 0 || slot >= p->inv_count) {
        printf("Invalid slot!\n");
        return 0;
    }
    
    Item *item = &p->inventory[slot];
    
    if (item->type != ITEM_CONSUMABLE || item->quantity <= 0) {
        printf("That item cannot be used!\n");
        return 0;
    }
    
    // Use the item (for now, all consumables are health potions)
    int heal_amount = 30 + (item->value * 2);  // Scale healing with value
    p->health += heal_amount;
    if (p->health > p->max_health) {
        p->health = p->max_health;
    }
    
    printf("Used %s! Restored %d HP. Current HP: %d/%d\n", 
           item->name, heal_amount, p->health, p->max_health);
    
    // Decrease quantity
    item->quantity--;
    
    // Remove item from inventory if depleted
    if (item->quantity <= 0) {
        printf("%s depleted!\n", item->name);
        // Shift items down to fill the gap
        for (int i = slot; i < p->inv_count - 1; i++) {
            p->inventory[i] = p->inventory[i + 1];
            
            // Update equipment slots if they shifted
            if (p->equipped.weapon_slot == i + 1) p->equipped.weapon_slot = i;
            if (p->equipped.armor_slot == i + 1) p->equipped.armor_slot = i;
        }
        p->inv_count--;
        
        // Fix equipment slots if the removed item was equipped
        if (p->equipped.weapon_slot == slot) p->equipped.weapon_slot = INVALID_SLOT;
        if (p->equipped.armor_slot == slot) p->equipped.armor_slot = INVALID_SLOT;
    }
    
    return 1;
}