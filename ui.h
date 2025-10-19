#ifndef UI_H
#define UI_H

#include "player.h"
#include "dungeon.h"

/*
 * Terminal Control Functions
 * These functions use ANSI escape codes to control the terminal display
 */

/* Clear the entire terminal screen */
void ui_clear_screen(void);

/* Move the cursor to a specific row and column position */
void ui_move_cursor(int row, int col);

/* Hide the cursor from view (cleaner display) */
void ui_hide_cursor(void);

/* Show the cursor (needed for input) */
void ui_show_cursor(void);

/*
 * Game Rendering Functions
 */

/* 
 * Render the complete game interface including:
 * - Title bar
 * - Message log showing recent events
 * - Player status (HP, gold, level, stats)
 * - Equipment display
 * - Local map view centered on player
 * - Controls help
 */
void ui_render_game(const Player *player, const Position *pos, const char *message, const Map *map);

#endif