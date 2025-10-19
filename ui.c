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
void ui_render_game(const Player *player, const Position *pos, const char *message, const Map *map) {
    ui_clear_screen();
    ui_hide_cursor();
    
    int row, col;

    // Title bar
    row = 1;
    col = 1;
    ui_move_cursor(row, col);
    printf("╔════════════════════════════════════════════════════════════════════════════════╗");
    row++;
    ui_move_cursor(row, col);
    printf("║                        DUNGEON CRAWLER ADVENTURE                               ║");
    row++;
    ui_move_cursor(row, col);
    printf("╚════════════════════════════════════════════════════════════════════════════════╝");
    
    // Message/log area
    row = 5;
    col = 2;
    ui_move_cursor(row, col);
    printf("┌─ MESSAGE LOG ");
    // Total width should be 80 characters (to match the bottom border)
    // "┌─ MESSAGE LOG " is 15 characters, "┐" is 1 character
    // So we need 80 - 15 - 1 = 64 more "─" characters
    for (int i = 0; i < 64; i++) {
        printf("─");
    }
    printf("┐\n");
    col = 2;
    row++;
    ui_move_cursor(row, col);
    printf("│ ");
    
    // Word wrap the message across multiple lines
    if (message && strlen(message) > 0) {
        int msg_len = strlen(message);
        int max_width = 76;  // Maximum characters per line (80 - border chars)
        int start = 0;       // Current position in message string
        
        // Process message in chunks that fit within max_width
        while (start < msg_len) {
            // If not first line, close previous line and start new one
            if (start > 0) {
                printf(" │");
                row++;
                ui_move_cursor(row, col);
                printf("│ ");
            }
            
            // Calculate how many characters to print on this line
            int remaining = msg_len - start;
            int line_len = (remaining > max_width) ? max_width : remaining;
            
            // Print this line's content, padded to max_width
            printf("%-*.*s", max_width, line_len, message + start);
            
            // Move to next chunk
            start += line_len;
        }
    } else {
        // No message - print empty line
        printf("%-76s", "");
    }
    
    printf(" │");
    
    row++;
    ui_move_cursor(row, col);
    printf("└");
    for (int i = 0; i < 78; i++)
    {
        printf("─");
    }

    printf("┘");

    // Player stats section (left side)
    row = 10;
    col = 2;
    ui_move_cursor(row, col);
    printf("┌─ PLAYER STATUS ─────────────────────┐");
    row++;
    ui_move_cursor(row, col);
    printf("│ Level: %-2d     HP: %3d/%-3d         │", player->level, player->health, player->max_health);
    row++;
    ui_move_cursor(row, col);
    printf("│ XP: %4d/%4d                     │", player->experience, player->exp_to_next_level);
    row++;
    ui_move_cursor(row, col);
    printf("│ Gold: %-6d                       │", player->gold);
    row++;
    ui_move_cursor(row, col);
    printf("│                                      │");
    row++;
    ui_move_cursor(row, col);
    printf("│ Attack:  %-3d  (base %-2d)          │", player->total_damage, player->base_damage);
    row++;
    ui_move_cursor(row, col);
    printf("│ Defense: %-3d  (base %-2d)          │", player->total_defense, player->base_defense);
    row++;
    ui_move_cursor(row, col);
    printf("└──────────────────────────────────────┘");
    
    // Equipment section
    row = 19;
    col = 2;
    ui_move_cursor(row, col);
    printf("┌─ EQUIPMENT ─────────────────────────┐");
    row++;
    ui_move_cursor(row, col);
    if (player->equipped.weapon_slot != INVALID_SLOT) {
        const Item *w = &player->inventory[player->equipped.weapon_slot];
        printf("│ Weapon: %-28s │", w->name);
    } else {
        printf("│ Weapon: (none)                       │");
    }
    row++;
    ui_move_cursor(row, col);
    if (player->equipped.armor_slot != INVALID_SLOT) {
        const Item *a = &player->inventory[player->equipped.armor_slot];
        printf("│ Armor:  %-28s │", a->name);
    } else {
        printf("│ Armor:  (none)                       │");
    }
    row++;
    ui_move_cursor(row, col);
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
    
    row = 10;
    col = map_start_col;
    ui_move_cursor(row, col);
    printf("┌─ MAP (Position: %2d, %2d) ─────┐", pos->x, pos->y);
    
    row++;
    // Map section (right side) - update the rendering loop
    row = 11;  // Adjust as needed
    for (int y = min_y; y <= max_y; y++) {
        ui_move_cursor(row, col);
        printf("│ ");
        for (int x = min_x; x <= max_x; x++) {
            if (x == pos->x && y == pos->y) {
                printf(" @");  // Player
            } else if (x == MAP_CENTER && y == MAP_CENTER) {
                printf(" +");  // Spawn
            } else {
                Position check = {x, y};
                int special = is_special_location(&check);
                TileType tile = map_get_tile(map, x, y);
                
                if (special == 1) {
                    printf(" B");  // Boss
                } else if (special == 2) {
                    printf(" S");  // Shrine
                } else if (tile == TILE_WALL) {
                    printf(" #");  // Wall
                } else {
                    printf(" ·");  // Floor/Corridor
                }
            }
        }
        printf(" │");
        row++;
    }
    
    ui_move_cursor(row, col);
    printf("└────────────────────────────────────┘");
    
    // Legend
    row++;
    ui_move_cursor(row, col);
    printf("  @ = You  + = Spawn  B = Boss");
    row++;
    ui_move_cursor(row, col);
    printf("  S = Shrine  · = Empty");
    
    // Controls
    row = 30;
    col = 2;
    ui_move_cursor(row, col);
    printf("┌─ CONTROLS ");
    for (int i = 0; i < 67; i++)
    {
        printf("─");
    }
    printf("┐");
    row++;
    ui_move_cursor(row, col);
    printf("│ N/S/E/W = Move   I = Inventory   M = Full Map   Q = Quit");
    col = 80;
    ui_move_cursor(row, col);
    printf(" │");
    col = 2;
    row++;
    ui_move_cursor(row, col);
    printf("└");
    for (int i = 0; i < 78; i++)
    {
        printf("─");
    }

    printf("┘");

    // Command prompt
    row = 35;
    col = 2;
    ui_move_cursor(row, col);
    printf("Command: ");
    ui_show_cursor();
    fflush(stdout);
}