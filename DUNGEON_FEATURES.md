# Enhanced Dungeon Features

## Overview
The dungeon now features a **pre-populated world** with monsters, treasures, traps, and special locations, plus comprehensive **exploration tracking** with visual markers.

## New Features

### 1. Pre-Populated Dungeon Content

The dungeon is now generated with content **already placed** when the game starts, rather than randomly generated when you enter a room. This creates a persistent world you can explore.

#### Content Types:

- **Monsters** 🗡️ (Symbol: `M` on map)
  - Easy monsters near the center (Goblins, Skeletons, Giant Rats)
  - Medium difficulty monsters mid-range (Orcs, Giant Spiders, Zombies)
  - Hard monsters in outer regions (Trolls, Dark Knights, Demons)
  - Bosses at the four corners (Ancient Dragons)

- **Treasure Chests** 💰 (Symbol: `T` on map)
  - Amount scales with distance from center
  - 30% chance for bonus items (weapons/armor/potions)
  - Near center: 20-60 gold
  - Mid-range: 40-100 gold
  - Far regions: 60-160 gold

- **Traps** 💥 (Symbol: `!` on map)
  - Damage scales with distance from center
  - 5% chance near center
  - 10% chance mid-range
  - 15% chance in outer regions

- **Healing Fountains** ⛲ (Symbol: `+` on map)
  - Restore 20-50 HP
  - More common near the center (safer zones)
  - Become rare in dangerous outer regions

- **Special Locations**:
  - **Bosses** (Symbol: `B`) - Ancient Dragons at all four corners
  - **Shrines** (Symbol: `S`) - At cardinal directions from center
    - Can heal, grant gold, or give experience
    - One-time use per shrine

### 2. Exploration Tracking System

#### Visual Markers:
- `@` = Your current position
- `X` = Tiles you've visited and looted
- `?` = Unexplored tiles (you haven't been there yet)
- `#` = Walls (impassable)
- `M`, `T`, `!`, `+`, `B`, `S` = Content you can see but haven't looted yet

#### Map View (Press 'M'):
When you press 'M' during gameplay, you'll see:
1. A local area map (12-tile radius around you)
2. Legend showing all symbols
3. Your current coordinates
4. Distance from dungeon center
5. **Statistics**:
   - Exploration percentage (tiles visited / total walkable tiles)
   - Number of monsters remaining
   - Number of treasure chests remaining

### 3. Content Distribution Strategy

The dungeon uses a **distance-based difficulty system**:

#### Near Center (< 10 tiles):
- **15% monsters** (Easy difficulty)
- **15% treasure**
- **10% healing fountains** (safest zone)
- **5% traps**
- **55% empty** (lots of safe passages)

#### Mid-Range (10-20 tiles):
- **30% monsters** (Mix of Easy/Medium)
- **15% treasure**
- **8% healing fountains**
- **10% traps**
- **37% empty**

#### Far Regions (> 20 tiles):
- **40% monsters** (Medium/Hard difficulty)
- **15% treasure** (higher value)
- **5% healing fountains** (scarce healing)
- **15% traps** (very dangerous)
- **25% empty**

### 4. Gameplay Changes

#### Exploration Rewards:
- You can now **plan your route** using the map
- See where dangers and rewards are located
- Track your exploration progress
- Monster and treasure locations are **persistent** - they don't change

#### Strategic Decisions:
- Avoid monster-heavy areas if low on health
- Seek out healing fountains when needed
- Plan treasure hunting routes
- Know where bosses are before attempting them

#### One-Time Content:
- Once you loot a treasure, fight a monster, or trigger a trap, it's **marked as looted**
- The tile shows as `X` on the map
- No re-spawning enemies or infinite treasure
- Creates a sense of **persistent exploration**

## Commands

### During Exploration:
- `N` - Move North
- `S` - Move South
- `E` - Move East
- `W` - Move West
- `M` - **View explored map** (NEW!)
- `I` - View inventory
- `Q` - Quit game

### During Battle:
- `A` - Attack
- `Q` - Attempt to flee (30% chance)
- `I` - Use item (coming soon)

## Technical Details

### New Data Structures:

```c
// Content types for tiles
typedef enum {
    CONTENT_EMPTY,
    CONTENT_MONSTER,
    CONTENT_TREASURE,
    CONTENT_TRAP,
    CONTENT_HEALING_FOUNTAIN,
    CONTENT_BOSS,
    CONTENT_SHRINE
} TileContent;

// Monster difficulty levels
typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD,
    DIFFICULTY_BOSS
} MonsterDifficulty;

// Data stored for each tile
typedef struct {
    TileContent content;        // What's on this tile
    MonsterDifficulty difficulty; // For monsters
    int treasure_value;         // For treasure (gold amount)
    int is_looted;             // Has it been consumed?
} TileData;
```

### Enhanced Map Structure:

```c
typedef struct {
    TileType tiles[MAP_SIZE][MAP_SIZE];     // Wall/floor/corridor
    TileData data[MAP_SIZE][MAP_SIZE];      // Content of each tile
    int visited[MAP_SIZE][MAP_SIZE];        // Exploration tracking
} Map;
```

## Tips for Playing

1. **Start Safe**: Explore the center area first to gain levels and equipment
2. **Use the Map**: Press 'M' frequently to plan your route
3. **Track Progress**: Check exploration percentage to see how much you've discovered
4. **Plan Carefully**: You can see monsters before engaging them
5. **Heal Strategically**: Look for healing fountains (`+`) when low on HP
6. **Save Shrines**: Shrine benefits are powerful but one-time use
7. **Build Up**: Fight easier monsters before tackling harder ones
8. **Boss Preparation**: Know where bosses are (`B`) and prepare before fighting them

## Future Enhancements

Potential additions:
- Mini-map in the corner during normal play
- Quest markers and objectives
- Secret rooms and hidden passages
- Item shops in safe zones
- Multiple dungeon levels
- Save/load game functionality
- NPC encounters (friendly merchants, quest givers)
- Environmental hazards (lava, water, etc.)

---

Enjoy exploring your dungeon! The world is dangerous but rewarding for brave adventurers! 🗡️⚔️🛡️
