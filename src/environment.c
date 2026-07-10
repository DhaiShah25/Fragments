#include <raylib.h>
#include <stddef.h>
#include <stdint.h>

typedef enum GameStage : uint8_t {
  // Starting Animation
  Animation,
  // Tutorial
  Movement,
  Escape,
  Attacks,
  Effects,
  Weaknesses,
  CastTime,
  Merging,
  // Gameplay
  Floors,
  Victory,
  Loss
} GameStage;

typedef enum TileType {
  Floor,
  Bookshelf,
} TileType;

Vector2 gen_map(uint16_t *map) {
  for (size_t i = 0; i < 15; i++) {
    map[i] = -1;
  }
  map[1] = 0x4001;

  size_t x = 1;
  size_t y = 1;

  for (size_t i = 3; i < 14; i += 2) {
    size_t row_start = 1;
    size_t row_size = 1;
    for (size_t j = row_start; j < 14; j += 2) {
      map[i] &= ~(1 << j);
      if (GetRandomValue(0, 1) == 0 && j != 13) {
        map[i] &= ~(1 << (j + 1));
        if (GetRandomValue(0, 10) == 10) {
          x = i;
          y = j;
        }
        row_size++;
      } else {
        size_t col = row_start + 2 * GetRandomValue(0, row_size - 1);
        map[i - 1] &= ~(1 << col);
        row_size = 1;
        row_start = j + 2;
      }
    }
  }

  return (Vector2){x * 160.0, y * 160.0};
}
