#include <raylib.h>
#include <stddef.h>
#include <stdint.h>

constexpr size_t MAP_SIZE = 15;
constexpr float TILE_SIZE = 32.0;

typedef enum GameStage : uint8_t {
  GUI,
  // Starting Animation
  Animation,
  // Tutorial
  Movement,
  Attacks,
  Escape,
  // Gameplay
  FloorChange,
  Floors,
  Victory,
  Loss
} GameStage;

typedef enum TileType {
  Floor,
  Bookshelf,
} TileType;

Vector2 gen_map(uint8_t map[MAP_SIZE][MAP_SIZE]) {
  for (size_t i = 0; i < MAP_SIZE; i++) {
    for (size_t j = 0; j < MAP_SIZE; j++) {
      map[i][j] = 1;
    }
  }
  for (size_t j = 0; j < MAP_SIZE - 1; j++) {
    map[1][j] = 0;
  }
  map[1][0] = 1;
  map[1][MAP_SIZE - 1] = 1;

  for (size_t i = 3; i < MAP_SIZE - 1; i += 2) {
    size_t row_start = 1;
    size_t row_size = 1;
    for (size_t j = row_start; j < MAP_SIZE - 1; j += 2) {
      map[i][j] = 0;
      if (GetRandomValue(0, 1) == 0 && j != MAP_SIZE - 2) {
        map[i][j + 1] = 0;
        row_size++;
      } else {
        size_t col = row_start + 2 * GetRandomValue(0, row_size - 1);
        map[i - 1][col] = 0;
        row_size = 1;
        row_start = j + 2;
      }
    }
  }

  size_t x = (GetRandomValue(1, 6) * 2 + 1) * TILE_SIZE + TILE_SIZE / 2;
  size_t y = (GetRandomValue(1, 6) * 2 + 1) * TILE_SIZE + TILE_SIZE / 2;

  return (Vector2){x, y};
}

void draw_map(uint8_t map[MAP_SIZE][MAP_SIZE], Vector2 fragment_location) {
  // TODO: Stop copying the 2D map array
  for (int x = 0; x < MAP_SIZE; x++) {
    for (int y = 0; y < MAP_SIZE; y++) {
      if (map[x][y]) {
        DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, (Color){100, 20, 20, 255});
      } else
        DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, (Color){40, 20, 20, 255});
    }
  }
  DrawCircleV(fragment_location, TILE_SIZE / 4, (Color){245, 235, 235, 255});
}
