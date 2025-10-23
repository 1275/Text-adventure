#include <stdio.h>
#include <stdlib.h>

// Quick test to verify monster scaling calculations
int main() {
    printf("Monster Scaling Test\n");
    printf("====================\n\n");
    
    // Goblin template: base_hp=25, hp_per_level=8
    printf("GOBLIN (base HP: 25, +8 per level)\n");
    for (int player_lvl = 1; player_lvl <= 10; player_lvl++) {
        int level_min = player_lvl - 2;
        int level_max = player_lvl + 2;
        if (level_min < 1) level_min = 1;
        if (level_max < 1) level_max = 1;
        
        printf("Player Lvl %2d: Monster Lvl %d-%d | ", player_lvl, level_min, level_max);
        
        // Calculate at min and max level
        int hp_min = 25 + (level_min - 1) * 8;
        int hp_max = 25 + (level_max - 1) * 8;
        
        int attack_min = 5 + (level_min - 1) * 2;
        int attack_max = 5 + (level_max - 1) * 2;
        
        printf("HP: %d-%d | Attack: %d-%d\n", hp_min, hp_max, attack_min, attack_max);
    }
    
    printf("\nDRAGON (base HP: 120, +30 per level, offset +3 to +7)\n");
    for (int player_lvl = 1; player_lvl <= 10; player_lvl++) {
        int level_min = player_lvl + 3;
        int level_max = player_lvl + 7;
        
        printf("Player Lvl %2d: Monster Lvl %d-%d | ", player_lvl, level_min, level_max);
        
        int hp_min = 120 + (level_min - 1) * 30;
        int hp_max = 120 + (level_max - 1) * 30;
        
        int attack_min = 18 + (level_min - 1) * 6;
        int attack_max = 18 + (level_max - 1) * 6;
        
        printf("HP: %d-%d | Attack: %d-%d\n", hp_min, hp_max, attack_min, attack_max);
    }
    
    return 0;
}
