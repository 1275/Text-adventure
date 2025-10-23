# How to Add New Character Classes

The character class system has been refactored to be easily expandable. You only need to modify **TWO places** to add a new class:

## Step 1: Add the class enum to `player.h`

```c
typedef enum {
    CLASS_WARRIOR,
    CLASS_MAGE,
    CLASS_ROGUE,      // <-- Add new class here
    CLASS_COUNT       // Keep this last!
} PlayerClass;
```

## Step 2: Add the class definition to `player.c`

Find the `class_definitions[]` array and add your new class:

```c
static const ClassDefinition class_definitions[] = {
    {
        CLASS_WARRIOR,
        "Warrior",
        "A fierce melee fighter",
        100,  // max_health
        15,   // base_damage
        8     // base_defense
    },
    {
        CLASS_MAGE,
        "Mage",
        "A mystical spellcaster",
        120,  // max_health
        8,    // base_damage
        4     // base_defense
    },
    // Add new classes here:
    {
        CLASS_ROGUE,
        "Rogue",
        "A nimble and evasive fighter",
        90,   // max_health (lower than average)
        12,   // base_damage (moderate)
        10    // base_defense (high - represents evasion)
    }
};
```

## That's it!

The character selection menu will automatically:
- Display the new class option
- Handle the new choice number
- Apply the stats correctly
- Show the class name in the UI

No need to modify:
- ❌ `main.c` selection logic
- ❌ `ui.c` display code
- ❌ Any hardcoded switch statements

Everything is data-driven and scales automatically!

## Example: Adding a Paladin Class

1. In `player.h`, add `CLASS_PALADIN` before `CLASS_COUNT`
2. In `player.c`, add to the array:
```c
{
    CLASS_PALADIN,
    "Paladin",
    "A holy warrior with balanced stats",
    110,  // max_health (above average)
    12,   // base_damage (balanced)
    7     // base_defense (balanced)
}
```

Done! The game will now offer 4 classes to choose from.
