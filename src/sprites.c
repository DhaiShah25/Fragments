#include "global.h"
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>

void move_sprite(Sprite *sprite, uint8_t *map, Vector2 velocity, int size) {
  float factor = GetFrameTime() * sprite->speedMultiplier;

  velocity = Vector2Scale(velocity, factor);

  int row = sprite->center.y / CELL_SIZE;
  int col = sprite->center.x / CELL_SIZE;

  while ((int)velocity.x != 0) {
    col = sprite->center.x / CELL_SIZE;

    if (velocity.x > 0) {
      if (!(map[row * size + col] & WALL_RIGHT) &&
          sprite->center.x + sprite->radius > (col + 1) * CELL_SIZE - WALL_WIDTH)
        break;
      velocity.x--;
      sprite->center.x++;
    } else {
      if (!(map[row * size + col] & WALL_LEFT) &&
          sprite->center.x - sprite->radius < col * CELL_SIZE + WALL_WIDTH)
        break;
      velocity.x++;
      sprite->center.x--;
    }
  }

  while ((int)velocity.y != 0) {
    row = sprite->center.y / CELL_SIZE;

    if (velocity.y > 0) {
      if (!(map[row * size + col] & WALL_DOWN) &&
          sprite->center.y + sprite->radius > (row + 1) * CELL_SIZE - WALL_WIDTH)
        break;
      velocity.y--;
      sprite->center.y++;
    } else {
      if (!(map[row * size + col] & WALL_UP) &&
          sprite->center.y - sprite->radius < row * CELL_SIZE + WALL_WIDTH)
        break;
      velocity.y++;
      sprite->center.y--;
    }
  }
}
