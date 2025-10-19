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

void ui_move_cursor(int row, int col) { // Move the terminal cursor to the specified row and column
    printf("\033[%d;%dH", row, col); // Print ANSI escape sequence to position cursor at row;col
    fflush(stdout); // Flush stdout so the escape sequence is sent immediately
} // End of ui_move_cursor function

void ui_hide_cursor(void) { // Hide the cursor (function start)
    printf("\033[?25l"); // Send ANSI escape sequence to hide the cursor
    fflush(stdout); // Flush stdout to ensure the sequence is output immediately
} // End of ui_hide_cursor function

void ui_show_cursor(void) { // Enable the terminal cursor (function start)
    printf("\033[?25h"); // Send ANSI escape sequence to show the cursor
    fflush(stdout); // Flush stdout to ensure the sequence is output immediately
} // End of ui_show_cursor function

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
    ui_clear_screen(); // clear terminal screen
    ui_hide_cursor();  // hide cursor while rendering
    
    int row, col; // cursor position helpers

    // Title bar
    row = 1;
    col = 1;
    ui_move_cursor(row, col); // move to top-left
    printf("╔════════════════════════════════════════════════════════════════════════════════╗"); // top border
    row++;
    ui_move_cursor(row, col);
    printf("║                        DUNGEON CRAWLER ADVENTURE                               ║"); // title
    row++;
    ui_move_cursor(row, col);
    printf("╚════════════════════════════════════════════════════════════════════════════════╝"); // bottom border
    
    // Message/log area
    row = 5;
    col = 2;
    ui_move_cursor(row, col);
    printf("┌─ MESSAGE LOG "); // message panel header
    // Total width should be 80 characters (to match the bottom border)
    // "┌─ MESSAGE LOG " is 15 characters, "┐" is 1 character
    // So we need 80 - 15 - 1 = 64 more "─" characters
    for (int i = 0; i < 64; i++) {
        printf("─"); // extend header line to full width
    }
    printf("┐\n");
    col = 2;
    row++;
    ui_move_cursor(row, col);
    printf("│ "); // left border for message content
    
    // Word wrap the message across multiple lines
    if (message && strlen(message) > 0) {
        int msg_len = strlen(message); // length of provided message
        int max_width = 76;  // Maximum characters per line (80 - border chars)
        int start = 0;       // Current position in message string
        
        // Process message in chunks that fit within max_width
        while (start < msg_len) {
            // If not first line, close previous line and start new one
            if (start > 0) {
                printf(" │"); // right border of previous line
                row++;
                ui_move_cursor(row, col);
                printf("│ "); // left border for next line
            }
            
            // Calculate how many characters to print on this line
            int remaining = msg_len - start;
            int line_len = (remaining > max_width) ? max_width : remaining; // clamp to max_width
            
            // Print this line's content, padded to max_width
            printf("%-*.*s", max_width, line_len, message + start); // print substring, left-padded
             
            // Move to next chunk
            start += line_len; // advance cursor in message
        }
    } else {
        // No message - print empty line
        printf("%-76s", ""); // blank padded message area
    }
    
    printf(" │"); // close the last message line with right border
    
    row++;
    ui_move_cursor(row, col);
    printf("└"); // bottom-left corner of message box
    for (int i = 0; i < 78; i++)
    {
        printf("─"); // bottom border of message box
    }

    printf("┘"); // bottom-right corner

    // Player stats section (left side)
    row = 10;
    col = 2;
    ui_move_cursor(row, col);
    printf("┌─ PLAYER STATUS ─────────────────────┐"); // player status box header
    row++;
    ui_move_cursor(row, col);
    printf("│ Level: %-2d     HP: %3d/%-3d         │", player->level, player->health, player->max_health); // level and hp
    row++;
    ui_move_cursor(row, col);
    printf("│ XP: %4d/%4d                     │", player->experience, player->exp_to_next_level); // xp progress
    row++;
    ui_move_cursor(row, col);
    printf("│ Gold: %-6d                       │", player->gold); // gold amount
    row++;
    ui_move_cursor(row, col);
    printf("│                                      │"); // spacer line
    row++;
    ui_move_cursor(row, col);
    printf("│ Attack:  %-3d  (base %-2d)          │", player->total_damage, player->base_damage); // attack stats
    row++;
    ui_move_cursor(row, col);
    printf("│ Defense: %-3d  (base %-2d)          │", player->total_defense, player->base_defense); // defense stats
    row++;
    ui_move_cursor(row, col);
    printf("└──────────────────────────────────────┘"); // close player status box
    
    // Equipment section
    row = 19;
    col = 2;
    ui_move_cursor(row, col);
    printf("┌─ EQUIPMENT ─────────────────────────┐"); // equipment header
    row++;
    ui_move_cursor(row, col);
    if (player->equipped.weapon_slot != INVALID_SLOT) {
        const Item *w = &player->inventory[player->equipped.weapon_slot];
        printf("│ Weapon: %-28s │", w->name); // display weapon name if equipped
    } else {
        printf("│ Weapon: (none)                       │"); // no weapon
    }
    row++;
    ui_move_cursor(row, col);
    if (player->equipped.armor_slot != INVALID_SLOT) {
        const Item *a = &player->inventory[player->equipped.armor_slot];
        printf("│ Armor:  %-28s │", a->name); // display armor name if equipped
    } else {
        printf("│ Armor:  (none)                       │"); // no armor
    }
    row++;
    ui_move_cursor(row, col);
    printf("└──────────────────────────────────────┘"); // close equipment box
    
    // Map section (right side)
    int map_start_col = 45; // column where map box starts
    int view_range = 7; // half-size of view window
    int min_x = pos->x - view_range;
    int max_x = pos->x + view_range;
    int min_y = pos->y - view_range;
    int max_y = pos->y + view_range;
    
    if (min_x < 0) min_x = 0; // clamp to map bounds
    if (max_x >= MAP_SIZE) max_x = MAP_SIZE - 1;
    if (min_y < 0) min_y = 0;
    if (max_y >= MAP_SIZE) max_y = MAP_SIZE - 1;
    
    row = 10;
    col = map_start_col;
    ui_move_cursor(row, col);
    printf("┌─ MAP (Position: %2d, %2d) ─────┐", pos->x, pos->y); // map header showing player pos
    
    row++;
    // Map section (right side) - update the rendering loop
    row = 11;  // starting row for map rows
    for (int y = min_y; y <= max_y; y++) {
        ui_move_cursor(row, col);
        printf("│ "); // left border for map row
        for (int x = min_x; x <= max_x; x++) {
            if (x == pos->x && y == pos->y) {
                printf(" @");  // Player marker
            } else if (x == MAP_CENTER && y == MAP_CENTER) {
                printf(" +");  // Spawn marker
            } else {
                Position check = {x, y};
                int special = is_special_location(&check); // check for boss/shrine corners
                TileType tile = map_get_tile(map, x, y); // get tile type
                
                if (special == 1) {
                    printf(" B");  // Boss location
                } else if (special == 2) {
                    printf(" S");  // Shrine location
                } else if (tile == TILE_WALL) {
                    printf(" #");  // Wall tile
                } else {
                    printf(" ·");  // Floor/corridor
                }
            }
        }
        printf(" │"); // right border for map row
        row++;
    }
    
    ui_move_cursor(row, col);
    printf("└────────────────────────────────────┘"); // close map box
    
    // Legend
    row++;
    ui_move_cursor(row, col);
    printf("  @ = You  + = Spawn  B = Boss"); // legend line 1
    row++;
    ui_move_cursor(row, col);
    printf("  S = Shrine  · = Empty  # = Wall"); // legend line 2
    
    // Controls
    row = 30;
    col = 2;
    ui_move_cursor(row, col);
    printf("┌─ CONTROLS ");
    for (int i = 0; i < 67; i++)
    {
        printf("─"); // controls header extension
    }
    printf("┐");
    row++;
    ui_move_cursor(row, col);
    printf("│ N/S/E/W = Move   I = Inventory   M = Full Map   Q = Quit"); // control descriptions
    col = 80;
    ui_move_cursor(row, col);
    printf(" │"); // right border padding (keeps layout consistent)
    col = 2;
    row++;
    ui_move_cursor(row, col);
    printf("└");
    for (int i = 0; i < 78; i++)
    {
        printf("─"); // bottom border of controls
    }

    printf("┘");

    // Command prompt
    row = 35;
    col = 2;
    ui_move_cursor(row, col);
    printf("Command: "); // prompt for user input
    ui_show_cursor(); // re-enable cursor for input
    fflush(stdout); // flush output so the UI appears immediately
}

// Render battle interface
void ui_render_battle(const Player *player, const BattleState *battle, const char *message) {
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
    printf("║                              ⚔  BATTLE  ⚔                                      ║");
    row++;
    ui_move_cursor(row, col);
    printf("╚════════════════════════════════════════════════════════════════════════════════╝");
    
    // Monster display
    row = 7;
    col = 25;
    ui_move_cursor(row, col);
    printf("┌──────────────────────────────┐");
    row++;
    ui_move_cursor(row, col);
    printf("│    %s", battle->monster.name);
    // Pad to 30 chars
    int name_len = strlen(battle->monster.name);
    for (int i = name_len; i < 25; i++) printf(" ");
    printf("│");
    row++;
    ui_move_cursor(row, col);
    printf("│                              │");
    row++;
    ui_move_cursor(row, col);
    printf("│  HP: %3d / %3d              │", battle->monster_hp, battle->monster.hp);
    row++;
    ui_move_cursor(row, col);
    printf("│  ATK: %-3d  DEF: %-3d        │", battle->monster.attack, battle->monster.defense);
    row++;
    ui_move_cursor(row, col);
    printf("└──────────────────────────────┘");
    
    // Player stats
    row = 15;
    col = 25;
    ui_move_cursor(row, col);
    printf("┌──────────────────────────────┐");
    row++;
    ui_move_cursor(row, col);
    printf("│         YOUR STATUS          │");
    row++;
    ui_move_cursor(row, col);
    printf("│                              │");
    row++;
    ui_move_cursor(row, col);
    printf("│  HP: %3d / %3d              │", player->health, player->max_health);
    row++;
    ui_move_cursor(row, col);
    printf("│  ATK: %-3d  DEF: %-3d        │", player->total_damage, player->total_defense);
    row++;
    ui_move_cursor(row, col);
    printf("└──────────────────────────────┘");
    
    // Battle log
    row = 24;
    col = 2;
    ui_move_cursor(row, col);
    printf("┌─ BATTLE LOG ");
    for (int i = 0; i < 64; i++) printf("─");
    printf("┐");
    row++;
    ui_move_cursor(row, col);
    printf("│ %-76s │", message);
    row++;
    ui_move_cursor(row, col);
    printf("└");
    for (int i = 0; i < 78; i++) printf("─");
    printf("┘");
    
    // Controls
    row = 30;
    col = 2;
    ui_move_cursor(row, col);
    printf("┌─ BATTLE COMMANDS ");
    for (int i = 0; i < 60; i++) printf("─");
    printf("┐");
    row++;
    ui_move_cursor(row, col);
    printf("│ A = Attack   I = Use Item   Q = Attempt to Flee");
    for (int i = 0; i < 29; i++) printf(" ");
    printf("│");
    row++;
    ui_move_cursor(row, col);
    printf("└");
    for (int i = 0; i < 78; i++) printf("─");
    printf("┘");
    
    // Command prompt
    row = 35;
    col = 2;
    ui_move_cursor(row, col);
    printf("Command: ");
    ui_show_cursor();
    fflush(stdout);
}