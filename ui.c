#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ui.h"
#include "dungeon.h"

// ANSI escape codes for terminal control
void ui_clear_screen(void) {
    printf("\033[2J");  // Clear entire screen
    printf("\033[H");   // Move cursor to home position
    fflush(stdout);
}

void ui_move_cursor(int row, int col) {
    printf("\033[%d;%dH", row, col);
    fflush(stdout);
}

void ui_hide_cursor(void) {
    printf("\033[?25l");
    fflush(stdout);
}

void ui_show_cursor(void) {
    printf("\033[?25h");
    fflush(stdout);
}

// Check if position is a special location (copied from dungeon.c)
static int is_special_location(const Position *pos) {
    if ((pos->x == 0 && pos->y == 0) ||
        (pos->x == 0 && pos->y == MAP_SIZE - 1) ||
        (pos->x == MAP_SIZE - 1 && pos->y == 0) ||
        (pos->x == MAP_SIZE - 1 && pos->y == MAP_SIZE - 1)) {
        return 1; // Boss location
    }
    
    if ((pos->x == MAP_CENTER && pos->y == 0) ||
        (pos->x == MAP_CENTER && pos->y == MAP_SIZE - 1) ||
        (pos->x == 0 && pos->y == MAP_CENTER) ||
        (pos->x == MAP_SIZE - 1 && pos->y == MAP_CENTER)) {
        return 2; // Shrine location
    }
    
    return 0;
}

// Render the complete game interface
void ui_render_game(const Player *player, const Position *pos, const char *message) {
    ui_clear_screen();
    ui_hide_cursor();
    
    // Title bar
    ui_move_cursor(1, 1);
    printf("╔════════════════════════════════════════════════════════════════════════════════╗");
    ui_move_cursor(2, 1);
    printf("║                        DUNGEON CRAWLER ADVENTURE                               ║");
    ui_move_cursor(3, 1);
    printf("╚════════════════════════════════════════════════════════════════════════════════╝");
    
    // Player stats section (left side)
    ui_move_cursor(5, 2);
    printf("┌─ PLAYER STATUS ─────────────────────┐");
    ui_move_cursor(6, 2);
    printf("│ Level: %-2d     HP: %3d/%-3d         │", player->level, player->health, player->max_health);
    ui_move_cursor(7, 2);
    printf("│ XP: %4d/%4d                     │", player->experience, player->exp_to_next_level);
    ui_move_cursor(8, 2);
    printf("│ Gold: %-6d                       │", player->gold);
    ui_move_cursor(9, 2);
    printf("│                                      │");
    ui_move_cursor(10, 2);
    printf("│ Attack:  %-3d  (base %-2d)          │", player->total_damage, player->base_damage);
    ui_move_cursor(11, 2);
    printf("│ Defense: %-3d  (base %-2d)          │", player->total_defense, player->base_defense);
    ui_move_cursor(12, 2);
    printf("└──────────────────────────────────────┘");
    
    // Equipment section
    ui_move_cursor(14, 2);
    printf("┌─ EQUIPMENT ─────────────────────────┐");
    ui_move_cursor(15, 2);
    if (player->equipped.weapon_slot != INVALID_SLOT) {
        const Item *w = &player->inventory[player->equipped.weapon_slot];
        printf("│ Weapon: %-28s │", w->name);
    } else {
        printf("│ Weapon: (none)                       │");
    }
    ui_move_cursor(16, 2);
    if (player->equipped.armor_slot != INVALID_SLOT) {
        const Item *a = &player->inventory[player->equipped.armor_slot];
        printf("│ Armor:  %-28s │", a->name);
    } else {
        printf("│ Armor:  (none)                       │");
    }
    ui_move_cursor(17, 2);
    printf("└──────────────────────────────────────┘");
    
    // Map section (right side)
    int map_start_col = 45;
    int view_range = 7;
    int min_x = pos->x - view_range;
    int max_x = pos->x + view_range;
    int min_y = pos->y - view_range;
    int max_y = pos->y + view_range;
    
    if (min_x < 0) min_x = 0;
    if (max_x >= MAP_SIZE) max_x = MAP_SIZE - 1;
    if (min_y < 0) min_y = 0;
    if (max_y >= MAP_SIZE) max_y = MAP_SIZE - 1;
    
    ui_move_cursor(5, map_start_col);
    printf("┌─ MAP (Position: %2d, %2d) ─────┐", pos->x, pos->y);
    
    int row = 6;
    for (int y = min_y; y <= max_y; y++) {
        ui_move_cursor(row, map_start_col);
        printf("│ ");
        for (int x = min_x; x <= max_x; x++) {
            if (x == pos->x && y == pos->y) {
                printf("@");  // Player
            } else if (x == MAP_CENTER && y == MAP_CENTER) {
                printf("+");  // Spawn
            } else {
                Position check = {x, y};
                int special = is_special_location(&check);
                if (special == 1) {
                    printf("B");  // Boss
                } else if (special == 2) {
                    printf("S");  // Shrine
                } else {
                    printf("·");  // Empty
                }
            }
        }
        printf(" │");
        row++;
    }
    
    ui_move_cursor(row, map_start_col);
    printf("└────────────────────────────────────┘");
    
    // Legend
    ui_move_cursor(row + 1, map_start_col);
    printf("  @ = You  + = Spawn  B = Boss");
    ui_move_cursor(row + 2, map_start_col);
    printf("  S = Shrine  · = Empty");
    
    // Message/log area
    ui_move_cursor(19, 2);
    printf("┌─ MESSAGE LOG ───────────────────────────────────────────────────────────────┐");
    ui_move_cursor(20, 2);
    printf("│                                                                              │");
    
    // Word wrap the message
    if (message && strlen(message) > 0) {
        char buffer[100];
        strncpy(buffer, message, 76);
        buffer[76] = '\0';
        ui_move_cursor(20, 4);
        printf("%s", buffer);
    }
    
    ui_move_cursor(21, 2);
    printf("└──────────────────────────────────────────────────────────────────────────────┘");
    
    // Controls
    ui_move_cursor(23, 2);
    printf("┌─ CONTROLS ──────────────────────────────────────────────────────────────────┐");
    ui_move_cursor(24, 2);
    printf("│ N/S/E/W = Move   I = Inventory   M = Full Map   Q = Quit                    │");
    ui_move_cursor(25, 2);
    printf("└──────────────────────────────────────────────────────────────────────────────┘");
    
    // Command prompt
    ui_move_cursor(27, 2);
    printf("Command: ");
    ui_show_cursor();
    fflush(stdout);
}