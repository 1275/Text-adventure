# Monster Scaling System

## Overview
Monsters now scale dynamically based on the player's level, with each monster type having:
- **Level ranges** relative to player level
- **Stat intervals** with randomization for variety
- **Progressive difficulty** as player advances

## How It Works

### Monster Templates
Each monster type is defined by a template with:
- **Level Offset Range**: How much the monster's level can vary from the player's
  - Example: Goblin has -2 to +2, so if player is level 5, goblin will be level 3-7
- **Base Stats**: Starting stats at level 1
- **Per-Level Scaling**: How much each stat increases per level
- **Stat Variance**: ±20% randomization on final stats for variety

### Monster Types and Their Ranges

| Monster      | Level Range | HP Scaling | Attack Scaling | Defense Scaling |
|--------------|-------------|------------|----------------|-----------------|
| Goblin       | Player -2 to +2 | 25 + 8/lvl | 5 + 2/lvl | 0 + 1/lvl |
| Skeleton     | Player -1 to +3 | 35 + 10/lvl | 7 + 2/lvl | 1 + 1/lvl |
| Giant Spider | Player +0 to +3 | 40 + 12/lvl | 8 + 3/lvl | 2 + 1/lvl |
| Orc          | Player +0 to +4 | 50 + 15/lvl | 10 + 3/lvl | 2 + 1/lvl |
| Troll        | Player +1 to +5 | 70 + 20/lvl | 12 + 4/lvl | 3 + 2/lvl |
| Dark Knight  | Player +2 to +6 | 90 + 25/lvl | 15 + 5/lvl | 5 + 2/lvl |
| Dragon       | Player +3 to +7 | 120 + 30/lvl | 18 + 6/lvl | 7 + 2/lvl |

### Examples

**Player Level 5:**
- **Goblin**: Level 3-7, HP 41-89, Attack 9-17, Defense 3-8
- **Skeleton**: Level 4-8, HP 59-123, Attack 12-21, Defense 4-9
- **Dragon**: Level 8-12, HP 282-498, Attack 55-89, Defense 19-29

**Player Level 1:**
- **Goblin**: Level 1-3, HP 20-50, Attack 4-10, Defense 0-3
- **Dragon**: Level 4-8, HP 163-307, Attack 33-57, Defense 13-21

## Key Features

1. **Dynamic Scaling**: All stats scale with monster level
2. **Variance**: ±20% randomization prevents repetitive encounters
3. **Progressive Difficulty**: Stronger monsters have higher level offsets
4. **Level Display**: Monster level is now shown in battle messages
5. **EXP Scaling**: Experience rewards increase with monster level (+10 exp per level)

## Balancing Notes

- **Early Game (Levels 1-3)**: Goblins and Skeletons are common, manageable threats
- **Mid Game (Levels 4-7)**: Mix of all monster types, increasing challenge
- **Late Game (Levels 8+)**: Dragons and Dark Knights become very powerful

## Modifying the System

To adjust monster difficulty, edit the templates in `enemies.c`:
- Increase/decrease base stats for overall difficulty
- Adjust per-level scaling for growth rate
- Change level offsets to modify when monsters appear
- Modify variance calculation (currently ±20%) for more/less randomization

### Example Template Entry:
```c
{"Goblin",    -2,    2,    25,    8,    5,    2,    0,    1,    8,   20,   20},
//   name   lvlMin lvlMax baseHP HP/lvl baseAtk Atk/lvl baseDef Def/lvl minGold maxGold baseExp
```
