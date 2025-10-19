/*
 * ui.c - User Interface and Terminal Rendering
 * 
 * This file handles all visual display using ANSI escape codes.
 * 
 * Features:
 * - Terminal control (cursor, clearing, positioning)
 * - Full screen game interface with borders
 * - Status panels (player stats, equipment, map)
 * - Message display with word wrapping
 * - Box-drawing characters for clean UI
 * 
 * ANSI Escape Codes Used:
 * - \033[2J - Clear screen
 * - \033[H - Move cursor to home (1,1)
 * - \033[row;colH - Move cursor to position
 * - \033[?25l/h - Hide/show cursor
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ui.h"
#include "dungeon.h"

/*
 * Terminal Control Functions
 * These use ANSI escape codes to manipulate the terminal display
 */

/* Clear the entire screen and reset cursor to top-left */
void ui_clear_screen(void) {
    printf("\033[2J");  /* ANSI: Clear entire screen */
    printf("\033[H");   /* ANSI: Move cursor to home position (row 1, col 1) */
    fflush(stdout);     /* Force output to display immediately */
}

/* Move cursor to a specific row and column position (1-indexed) */
void ui_move_cursor(int row, int col) {
    printf("\033[%d;%dH", row, col);  /* ANSI: Position cursor */
    fflush(stdout);
}

/* Hide the cursor from view (cleaner display) */
void ui_hide_cursor(void) {
    printf("\033[?25l");  /* ANSI: Hide cursor */
    fflush(stdout);
}

/* Show the cursor (needed for text input) */
void ui_show_cursor(void) {
    printf("\033[?25h");  /* ANSI: Show cursor */
    fflush(stdout);
}

/*
 * Check if a position is a special location (boss or shrine)
 * Note: This is duplicated from dungeon.c to avoid circular dependencies
 * 
 * Returns:
 *   1 - Boss location (corners)
 *   2 - Shrine location (cardinal points)
 *   0 - Normal location
 */
static int is_special_location(const Position *pos) {
    /* Check for boss locations at four corners */
    if ((pos->x == 0 && pos->y == 0) ||
        (pos->x == 0 && pos->y == MAP_SIZE - 1) ||
        (pos->x == MAP_SIZE - 1 && pos->y == 0) ||
        (pos->x == MAP_SIZE - 1 && pos->y == MAP_SIZE - 1)) {
        return 1; // Boss location
    }
    
    /* Check for shrine locations at cardinal directions */
    if ((pos->x == MAP_CENTER && pos->y == 0) ||
        (pos->x == MAP_CENTER && pos->y == MAP_SIZE - 1) ||
        (pos->x == 0 && pos->y == MAP_CENTER) ||
        (pos->x == MAP_SIZE - 1 && pos->y == MAP_CENTER)) {
        return 2; // Shrine location
    }
    
    return 0;  /* Normal location */
}

/*
 * Render the complete game interface
 * 
 * This is the main rendering function that draws the entire screen layout:
 * 
 * Layout (80 columns wide):
 * ┌────────────────────────────────────────┐
 * │           TITLE BAR                    │
 * ├────────────────────────────────────────┤
 * │ MESSAGE LOG                            │
 * ├──────────────────┬─────────────────────┤
 * │ PLAYER STATUS    │  LOCAL MAP VIEW     │
 * │ - HP, Gold, XP   │  (15x15 area)       │
 * │ - Attack/Defense │                     │
 * ├──────────────────┤                     │
 * │ EQUIPMENT        │                     │
 * │ - Weapon         │                     │
 * │ - Armor          │                     │
 * └──────────────────┴─────────────────────┘
 * │ CONTROLS (help text)                   │
 * └────────────────────────────────────────┘
 * │ Command: _                             │
 * 
 * Uses Unicode box-drawing characters for borders
 */
void ui_render_game(const Player *player, const Position *pos, const char *message, const Map *map) {
    /* Clear screen and hide cursor for clean display */
    ui_clear_screen();
    ui_hide_cursor();
    
    int row, col;  /* Variables for cursor positioning */

    /* ═══════════════════════════════════════════
     * TITLE BAR - Game name centered at top
     * ═══════════════════════════════════════════ */
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
    
    /* ═══════════════════════════════════════════
     * MESSAGE LOG - Displays recent events
     * Shows combat results, treasure finds, etc.
     * ═══════════════════════════════════════════ */
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
    
    /* Word wrap the message across multiple lines if needed
     * Messages longer than 76 characters are split into multiple lines
     * This ensures long event descriptions display properly */
    if (message && strlen(message) > 0) {
        int msg_len = strlen(message);
        int max_width = 76;  /* Maximum characters per line (80 total - 4 for borders) */
        int start = 0;       /* Current position in message string */
        
        /* Process message in chunks that fit within max_width */
        while (start < msg_len) {
            /* If not first line, close previous line border and start new line */
            if (start > 0) {
                printf(" │");  /* Close previous line */
                row++;
                ui_move_cursor(row, col);
                printf("│ ");  /* Start new line with border */
            }
            
            /* Calculate how many characters to print on this line
             * If remaining message is shorter than max_width, use remaining length */
            int remaining = msg_len - start;
            int line_len = (remaining > max_width) ? max_width : remaining;
            
            /* Print this line's content, padded to max_width for alignment
             * %-*.*s format: left-align, max_width total, line_len actual chars */
            printf("%-*.*s", max_width, line_len, message + start);
            
            /* Move to next chunk of the message */
            start += line_len;
        }
    } else {
        /* No message - print empty line with proper padding */
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

    /* ═══════════════════════════════════════════
     * PLAYER STATUS - Left panel showing character stats
     * Displays: Level, HP, XP, Gold, Attack, Defense
     * ═══════════════════════════════════════════ */
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
    
    /* ═══════════════════════════════════════════
     * EQUIPMENT - Shows currently equipped items
     * Displays weapon and armor slots
     * ═══════════════════════════════════════════ */
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
    
    /* ═══════════════════════════════════════════
     * MAP VIEW - Right panel showing local area
     * Displays 15x15 tile area centered on player
     * Shows: @ (player), + (spawn), B (boss), S (shrine), # (wall), · (floor)
     * ═══════════════════════════════════════════ */
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
    
    /* Render the visible map area, tile by tile
     * Each tile is represented by a two-character symbol */
    row = 11;  /* Start rendering map at row 11 */
    for (int y = min_y; y <= max_y; y++) {
        ui_move_cursor(row, col);
        printf("│ ");  /* Left border */
        
        /* Render each tile in this row */
        for (int x = min_x; x <= max_x; x++) {
            /* Determine what symbol to display for this tile
             * Priority: player > spawn > special locations > tile type */
            
            if (x == pos->x && y == pos->y) {
                /* Player's current position */
                printf(" @");
            } else if (x == MAP_CENTER && y == MAP_CENTER) {
                /* Spawn point at center of map */
                printf(" +");
            } else {
                /* Check for special locations and tile types */
                Position check = {x, y};
                int special = is_special_location(&check);
                TileType tile = map_get_tile(map, x, y);
                
                if (special == 1) {
                    printf(" B");  /* Boss lair (corners) */
                } else if (special == 2) {
                    printf(" S");  /* Ancient shrine (cardinal points) */
                } else if (tile == TILE_WALL) {
                    printf(" #");  /* Impassable wall */
                } else {
                    printf(" ·");  /* Walkable floor or corridor */
                }
            }
        }
        printf(" │");  /* Right border */
        row++;
    }
    
    ui_move_cursor(row, col);
    printf("└────────────────────────────────────┘");
    
    /* Display map legend explaining symbols */
    row++;
    ui_move_cursor(row, col);
    printf("  @ = You  + = Spawn  B = Boss");
    row++;
    ui_move_cursor(row, col);
    printf("  S = Shrine  · = Empty");
    
    /* ═══════════════════════════════════════════
     * CONTROLS - Help text showing available commands
     * ═══════════════════════════════════════════ */
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

    /* ═══════════════════════════════════════════
     * COMMAND PROMPT - Where player enters commands
     * ═══════════════════════════════════════════ */
    row = 35;
    col = 2;
    ui_move_cursor(row, col);
    printf("Command: ");
    ui_show_cursor();
    fflush(stdout);
}