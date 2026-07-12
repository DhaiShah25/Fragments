#include "global.h"

void move_sprite(Sprite *sprite, GameMap map, Vector2 velocity) {
  float factor = GetFrameTime() * sprite->speedMultiplier;

  float new_x = sprite->center.x + velocity.x * factor;
  float new_y = sprite->center.y + velocity.y * factor;

  int left = (new_x - sprite->radius) / TILE_SIZE;
  int top = (sprite->center.y - sprite->radius) / TILE_SIZE;
  int right = (new_x + sprite->radius) / TILE_SIZE;
  int bottom = (sprite->center.y + sprite->radius) / TILE_SIZE;

  if (!(map[left][top] || map[right][top] || map[left][bottom] || map[right][bottom])) {
    sprite->center.x = new_x;
  }

  left = (sprite->center.x - sprite->radius) / TILE_SIZE;
  top = (new_y - sprite->radius) / TILE_SIZE;
  right = (sprite->center.x + sprite->radius) / TILE_SIZE;
  bottom = (new_y + sprite->radius) / TILE_SIZE;

  if (!(map[left][top] || map[right][top] || map[left][bottom] || map[right][bottom])) {
    sprite->center.y = new_y;
  }
}
