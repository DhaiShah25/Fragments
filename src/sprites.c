#include "global.h"
#include <raylib.h>
#include <stdint.h>

void move_sprite(Sprite *sprite, uint8_t *map, Vector2 velocity, int size) {
  float factor = GetFrameTime() * sprite->speedMultiplier;

  float new_x = sprite->center.x + velocity.x * factor;
  float new_y = sprite->center.y + velocity.y * factor;

  int left = (new_x - sprite->radius) / TILE_SIZE;
  int top = (sprite->center.y - sprite->radius) / TILE_SIZE;
  int right = (new_x + sprite->radius) / TILE_SIZE;
  int bottom = (sprite->center.y + sprite->radius) / TILE_SIZE;

  if (!(map[CoordToIdx(left, top, size)] || map[CoordToIdx(right, top, size)] || map[CoordToIdx(left, bottom, size)] ||
        map[CoordToIdx(right, bottom, size)])) {
    sprite->center.x = new_x;
  }

  left = (sprite->center.x - sprite->radius) / TILE_SIZE;
  top = (new_y - sprite->radius) / TILE_SIZE;
  right = (sprite->center.x + sprite->radius) / TILE_SIZE;
  bottom = (new_y + sprite->radius) / TILE_SIZE;

  if (!(map[CoordToIdx(left, top, size)] || map[CoordToIdx(right, top, size)] || map[CoordToIdx(left, bottom, size)] ||
        map[CoordToIdx(right, bottom, size)])) {
    sprite->center.y = new_y;
  }
}
