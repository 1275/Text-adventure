#ifndef UI_H
#define UI_H

#include "player.h"
#include "dungeon.h"

// Terminal control
void ui_clear_screen(void);
void ui_move_cursor(int row, int col);
void ui_hide_cursor(void);
void ui_show_cursor(void);

// Full screen rendering
void ui_render_game(const Player *player, const Position *pos, const char *message, const Map *map);

#endif