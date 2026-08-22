#include "global.h"
#include <raylib.h>
#include <stdint.h>
#include <string.h>

Vector2 gen_map(uint8_t *map, int size) {
  memset(map, 1, size * size * sizeof(uint8_t));
  for (int j = 1; j < size - 1; j++) {
    map[CoordToIdx(1, j, size)] = 0;
  }

  for (int i = 3; i < size - 1; i += 2) {
    int row_start = 1;
    int row_size = 1;
    for (int j = row_start; j < size - 1; j += 2) {
      map[CoordToIdx(i, j, size)] = 0;
      if (GetRandomValue(0, 1) == 0 && j != size - 2) {
        map[CoordToIdx(i, j + 1, size)] = 0;
        row_size++;
      } else {
        int col = row_start + 2 * GetRandomValue(0, row_size - 1);
        map[CoordToIdx(i - 1, col, size)] = 0;
        row_size = 1;
        row_start = j + 2;
      }
    }
  }

  int x = (GetRandomValue(1, size / 2 - 1) * 2 + 1) * TILE_SIZE + TILE_SIZE / 2;
  int y = (GetRandomValue(1, size / 2 - 1) * 2 + 1) * TILE_SIZE + TILE_SIZE / 2;

  return (Vector2){x, y};
}

void draw_map(uint8_t *map, int size) {
  for (int x = 0; x < size; x++) {
    for (int y = 0; y < size; y++) {
      if (map[CoordToIdx(x, y, size)]) {
        DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, (Color){100, 20, 20, 255});
      } else
        DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, (Color){40, 20, 20, 255});
    }
  }
}

int CoordToIdx(int x, int y, int size) { return x + y * size; }
