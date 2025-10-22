#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dungeon.h"
#include "player.h"
#include "ui.h"

int main(void)
{
    srand((unsigned int)time(NULL)); // Seed random number generator

    // Display class selection menu
    printf("\n╔════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                        DUNGEON CRAWLER ADVENTURE                               ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════╝\n\n");
    printf("Choose your character class:\n\n");
    printf("  [1] Warrior - A fierce melee fighter\n");
    printf("      • High base damage: 15 (vs. 10 default)\n");
    printf("      • High base defense: 8 (vs. 5 default)\n");
    printf("      • Standard health: 100\n\n");
    printf("  [2] Mage - A mystical spellcaster\n");
    printf("      • High max health: 120 (vs. 100 default)\n");
    printf("      • Lower base damage: 8 (vs. 10 default)\n");
    printf("      • Lower base defense: 4 (vs. 5 default)\n\n");
    
    // Get class selection with input validation
    PlayerClass selected_class = CLASS_WARRIOR;
    int valid_input = 0;
    while (!valid_input) {
        printf("Enter your choice (1 or 2): ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            // Clear invalid input
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Invalid input! Please enter 1 or 2.\n");
            continue;
        }
        
        if (choice == 1) {
            selected_class = CLASS_WARRIOR;
            valid_input = 1;
            printf("\nYou have chosen the Warrior class!\n");
        } else if (choice == 2) {
            selected_class = CLASS_MAGE;
            valid_input = 1;
            printf("\nYou have chosen the Mage class!\n");
        } else {
            printf("Invalid choice! Please enter 1 or 2.\n");
        }
    }
    
    printf("\nPress Enter to begin your adventure...");
    getchar(); // consume newline from scanf
    getchar(); // wait for user to press Enter
    
    Player player; // Initialize player
    player_init(&player, selected_class);  // Set starting stats and inventory based on class
    
    Map map;
    map_generate(&map);

    Position pos = {MAP_CENTER, MAP_CENTER};
    GameState state = STATE_EXPLORING;
    BattleState battle = {0};
    battle.is_active = 0;
    
    int running = 1;
    char message[256] = "Whoa! You trigger a magical portal and find yourself in a mysterious dungeon...";

    ui_render_game(&player, &pos, message, &map);

    while (running)
    {
        char command;
        if (scanf(" %c", &command) != 1) {
            break;
        }
        
        handle_command(command, &running, &pos, &player, message, &map, &state, &battle);
        
        if (running) {
            if (state == STATE_BATTLE) {
                ui_render_battle(&player, &battle, message);
            } else {
                ui_render_game(&player, &pos, message, &map);
            }
        }

        if (player.health <= 0)
        {
            ui_clear_screen();
            printf("\n╔════════════════════════════════════════╗\n");
            printf("║         GAME OVER                      ║\n");
            printf("╚════════════════════════════════════════╝\n\n");
            printf("You have perished in the dungeon.\n");
            printf("Final Level: %d\n", player.level);
            printf("Gold Collected: %d\n", player.gold);
            printf("Final Position: [%d, %d]\n\n", pos.x, pos.y);
            running = 0;
        }
    }

    ui_show_cursor();
    return 0;
}