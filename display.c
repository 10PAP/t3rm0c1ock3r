#include "display.h"

void init_display(void) {
  player.x = 3;
  player.y = 3;
  display_clear();
}

uint8_t display[8] = {
    0b10011001,
    0b10111101,
    0b01011010,
    0b01111110,
    0b01000010,
    0b00111100,
    0b11011011,
    0b10000001
};

Position player;

int8_t cursor_dx[] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 3, 3};
int8_t cursor_dy[] = {0, 1, 2, 3, 0, 3, 0, 3, 0, 1, 2, 3};

void display_clear(void) {
    for (int i = 0; i < 8; ++i)
        display[i] = 0;
}

static void flip_bit_pos(Position* pos) {
    display[pos->x] ^= (1 << pos->y);
}

void display_draw_player(Position* player) {
  for (int c = 0; c < sizeof(cursor_dx); ++c) {
    Position pixel = *player;
    pixel.x = (pixel.x + cursor_dx[c]) & 0x7;
    pixel.y = (pixel.y + cursor_dy[c]) & 0x7;
    flip_bit_pos(&pixel);
  }
}

void draw_level(uint16_t val) {
  // [0, 0x0FFF] -> [0, 0x111]
  for (int i = 8; i >= 1; --i)
    display[i] = display[i-1];
  display[0] = (1 << (val >> 9));
}