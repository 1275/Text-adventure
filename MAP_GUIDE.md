# Map Display Examples

## Example 1: Starting Position

When you first start the game at the center:

```
    23 24 25 26 27 28
    ────────────────────
23 │ ?  ?  ?  #  ?  ? 
24 │ ?  X  X  X  ?  # 
25 │ #  X  @  T  X  ? 
26 │ ?  X  X  X  M  ? 
27 │ ?  ?  #  ?  ?  ? 
28 │ #  ?  ?  ?  #  # 

Current Position: (25, 25)
Distance from Center: 0 tiles

Exploration: 7/1250 tiles (0.6%)
Monsters remaining: 156
Treasures remaining: 89
```

**Meaning**:
- `@` = You are here (center of map)
- `X` = Already explored empty rooms
- `T` = Treasure chest visible (you can see it but haven't opened it)
- `M` = Monster visible (you can see it but haven't fought it)
- `?` = Unknown/unexplored
- `#` = Wall

## Example 2: After Some Exploration

After exploring a bit and looting some rooms:

```
    21 22 23 24 25 26 27 28 29 30
    ────────────────────────────────
21 │ #  #  ?  ?  #  ?  ?  #  ?  ? 
22 │ #  X  X  X  X  T  ?  ?  ?  ? 
23 │ ?  X  X  X  #  X  X  ?  ?  # 
24 │ ?  X  X  @  X  X  M  X  ?  ? 
25 │ #  X  T  X  X  X  X  X  +  ? 
26 │ ?  X  X  X  M  X  X  X  X  ? 
27 │ ?  X  #  X  X  !  X  #  ?  ? 
28 │ #  ?  ?  X  X  X  ?  ?  ?  # 
29 │ ?  ?  ?  ?  #  ?  ?  ?  ?  ? 

Current Position: (24, 24)
Distance from Center: 1 tiles

Exploration: 42/1250 tiles (3.4%)
Monsters remaining: 154
Treasures remaining: 87
```

**What you can see**:
- Many `X` tiles = You've explored this area thoroughly
- `T` at (25, 22) and (22, 25) = Uncollected treasure chests
- `M` at (26, 24) and (24, 26) = Monsters you haven't fought yet
- `!` at (25, 27) = Trap you can see (but haven't triggered)
- `+` at (28, 25) = Healing fountain available

## Example 3: Approaching a Boss

When you get near a corner boss lair:

```
     0  1  2  3  4  5  6  7  8  9 10
    ─────────────────────────────────
 0 │ B  X  X  #  ?  ?  #  ?  ?  ?  ? 
 1 │ X  X  M  X  X  ?  ?  ?  ?  #  # 
 2 │ X  X  X  X  #  T  ?  ?  ?  ?  ? 
 3 │ #  X  @  X  X  X  X  ?  #  ?  ? 
 4 │ ?  X  X  X  M  X  !  X  ?  ?  ? 
 5 │ ?  ?  #  X  X  X  X  X  X  ?  # 
 6 │ #  ?  ?  X  !  X  #  X  ?  ?  ? 
 7 │ ?  ?  ?  ?  X  X  X  ?  ?  ?  ? 
 8 │ ?  ?  ?  #  #  ?  ?  ?  #  #  # 

Current Position: (2, 3)
Distance from Center: 32 tiles

Exploration: 89/1250 tiles (7.1%)
Monsters remaining: 148
Treasures remaining: 82
```

**Strategic View**:
- `B` at (0, 0) = BOSS LOCATION! Ancient Dragon awaits
- You're only 3 tiles away from a boss fight
- `M` symbols show remaining monsters in the area
- `T` shows treasure you might want to grab first
- `!` symbols warn you about traps to avoid

## Example 4: Fully Explored Area

After clearing out an entire section:

```
    20 21 22 23 24 25 26 27 28 29 30
    ─────────────────────────────────
20 │ X  X  X  #  X  X  X  #  X  X  X 
21 │ X  X  X  X  X  X  X  X  X  X  # 
22 │ X  X  X  X  #  X  X  X  X  X  X 
23 │ #  X  X  X  X  X  @  X  X  #  X 
24 │ X  X  X  X  X  X  X  X  X  X  X 
25 │ X  X  T  X  X  X  X  X  X  X  # 
26 │ X  X  X  X  X  X  X  X  #  X  X 
27 │ X  #  X  X  X  X  X  #  X  X  X 
28 │ #  X  X  X  X  X  ?  ?  X  X  ? 
29 │ X  X  X  #  #  ?  ?  ?  ?  #  ? 

Current Position: (26, 23)
Distance from Center: 3 tiles

Exploration: 168/1250 tiles (13.4%)
Monsters remaining: 142
Treasures remaining: 81
```

**Progress**:
- Mostly `X` = This area is thoroughly explored and looted
- Only one `T` remaining (treasure at 22, 25)
- The `?` symbols show the frontier of your exploration
- You can plan which direction to explore next

## Example 5: Near a Shrine

Approaching one of the special shrine locations:

```
    23 24 25 26 27 28 29 30 31 32
    ────────────────────────────────
 0 │ ?  ?  #  S  #  ?  ?  ?  #  ? 
 1 │ ?  ?  X  X  X  ?  ?  ?  ?  ? 
 2 │ #  X  X  X  X  X  ?  #  ?  ? 
 3 │ ?  X  X  @  X  M  X  ?  ?  # 
 4 │ ?  X  X  X  X  X  T  X  ?  ? 
 5 │ #  ?  #  X  X  X  X  X  ?  ? 
 6 │ ?  ?  ?  X  !  X  #  ?  ?  ? 

Current Position: (25, 3)
Distance from Center: 25 tiles

Exploration: 45/1250 tiles (3.6%)
Monsters remaining: 155
Treasures remaining: 88
```

**Special Location**:
- `S` at (25, 0) = Ancient Shrine (top center of map)
- Shrines give powerful one-time bonuses (heal/gold/XP)
- You can see it from a distance and plan your approach
- Clear out monsters and traps before visiting

## Legend Reference

### Terrain:
- `@` = **Your Position** (always shows where you are)
- `#` = **Wall** (impassable barrier)
- `?` = **Unexplored** (you haven't been there yet)
- `X` = **Visited & Looted** (cleared out)

### Content (Before Looting):
- `M` = **Monster** (enemy waiting)
- `T` = **Treasure** (chest with gold/items)
- `!` = **Trap** (will damage you)
- `+` = **Healing Fountain** (restores HP)
- `B` = **Boss** (very dangerous!)
- `S` = **Shrine** (powerful one-time bonus)

### Map Statistics Explained:

**Exploration: 45/1250 tiles (3.6%)**
- 45 = Number of tiles you've visited
- 1250 = Total walkable tiles in the dungeon
- 3.6% = Percentage of dungeon explored

**Monsters remaining: 155**
- Total number of monsters still alive in the dungeon
- Decreases when you defeat them
- Includes regular monsters and bosses

**Treasures remaining: 88**
- Total number of treasure chests not yet opened
- Decreases when you loot them
- Each contains gold and possibly items

## Tips for Using the Map:

1. **Press 'M' often** - Check the map frequently to plan your route
2. **Mark your targets** - Mentally note treasure locations (`T`) to collect
3. **Avoid danger** - See monsters (`M`) and traps (`!`) before encountering them
4. **Find healing** - Look for fountains (`+`) when health is low
5. **Track progress** - Watch your exploration % grow as you adventure
6. **Plan boss fights** - Know where bosses (`B`) are and prepare accordingly
7. **Save shrines** - Shrines (`S`) are one-time use, so choose wisely when to visit

---

**Pro Tip**: The map shows a 12-tile radius around you. As you move, the view shifts to keep you centered!
