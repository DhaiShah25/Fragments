#include "global.h"
#include <assert.h>
#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint8_t *data;
  int length;
} CoordArray;

void mark_neighbor(Coord neighbor, int row, int col, int size, uint8_t *map, int map_idx) {
  if (neighbor.row > row) {
    map[map_idx] |= WALL_DOWN;
    map[neighbor.row * size + neighbor.col] |= WALL_UP;
  } else if (neighbor.row < row) {
    map[map_idx] |= WALL_UP;
    map[neighbor.row * size + neighbor.col] |= WALL_DOWN;
  } else if (neighbor.col > col) {
    map[map_idx] |= WALL_RIGHT;
    map[neighbor.row * size + neighbor.col] |= WALL_LEFT;
  } else {
    map[map_idx] |= WALL_LEFT;
    map[neighbor.row * size + neighbor.col] |= WALL_RIGHT;
  }
}

void add_frontier(int row, int col, int size, uint8_t *map, CoordArray *frontier_cells) {
  if (row >= 0 && col >= 0 && col < size && row < size && !(map[row * size + col] & FRONTIER) &&
      !(map[row * size + col] & VISITED)) {
    map[row * size + col] |= FRONTIER;
    frontier_cells->data[frontier_cells->length] = row * size + col;
    frontier_cells->length++;
  }
}

void mark(int row, int col, int size, uint8_t *map, CoordArray *frontier_cells) {
  map[row * size + col] |= VISITED;
  add_frontier(row - 1, col, size, map, frontier_cells);
  add_frontier(row + 1, col, size, map, frontier_cells);
  add_frontier(row, col - 1, size, map, frontier_cells);
  add_frontier(row, col + 1, size, map, frontier_cells);
}

Vector2 gen_map(uint8_t *map, int size) {
  assert(size <= 15 && "Map is too large");
  // Walls are default on, setting a true bit means the wall is open / gone
  memset(map, 0, size * size * sizeof(uint8_t));

  int row = GetRandomValue(0, size - 1);
  int col = GetRandomValue(0, size - 1);

  CoordArray frontier_cells;
  frontier_cells.data = malloc(size * size);
  frontier_cells.length = 0;
  mark(row, col, size, map, &frontier_cells);

  Coord neighbors[4] = {0};
  int n_idx = 0;

  while (frontier_cells.length != 0) {
    int frontier_idx = GetRandomValue(0, frontier_cells.length - 1);
    int map_idx = frontier_cells.data[frontier_idx];
    row = map_idx / size;
    col = map_idx % size;

    if (row > 0 && map[(row - 1) * size + col] & VISITED) {
      neighbors[n_idx] = (Coord){.row = row - 1, .col = col};
      n_idx++;
    }
    if (row + 1 < size && map[(row + 1) * size + col] & VISITED) {
      neighbors[n_idx] = (Coord){.row = row + 1, .col = col};
      n_idx++;
    }
    if (col > 0 && map[row * size + col - 1] & VISITED) {
      neighbors[n_idx] = (Coord){.row = row, .col = col - 1};
      n_idx++;
    }
    if (col + 1 < size && map[row * size + col + 1] & VISITED) {
      neighbors[n_idx] = (Coord){.row = row, .col = col + 1};
      n_idx++;
    }

    mark_neighbor(neighbors[GetRandomValue(0, n_idx - 1)], row, col, size, map, map_idx);
    if (GetRandomValue(0, 1))
      mark_neighbor(neighbors[GetRandomValue(0, n_idx - 1)], row, col, size, map, map_idx);

    frontier_cells.data[frontier_idx] = frontier_cells.data[frontier_cells.length - 1];
    frontier_cells.length--;
    mark(row, col, size, map, &frontier_cells);
    n_idx = 0;
  }

  free(frontier_cells.data);

  int x = GetRandomValue(0, size - 1) * CELL_SIZE + CELL_SIZE / 2;
  int y = GetRandomValue(0, size - 1) * CELL_SIZE + CELL_SIZE / 2;

  return (Vector2){x, y};
}

void draw_map(uint8_t *map, int size) {
  for (int i = 0; i < size * size; i++) {
    int x = i % size * CELL_SIZE;
    int y = i / size * CELL_SIZE;

    uint8_t cell = map[i];
    DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, (Color){30, 10, 10, 255});
    if ((cell & WALL_LEFT) == 0)
      DrawRectangle(x, y, WALL_WIDTH, CELL_SIZE, (Color){80, 50, 50, 255});
    if ((cell & WALL_RIGHT) == 0)
      DrawRectangle(x + CELL_SIZE - WALL_WIDTH, y, WALL_WIDTH, CELL_SIZE, (Color){80, 50, 50, 255});
    if ((cell & WALL_UP) == 0)
      DrawRectangle(x, y, CELL_SIZE, WALL_WIDTH, (Color){80, 50, 50, 255});
    if ((cell & WALL_DOWN) == 0)
      DrawRectangle(x, y + CELL_SIZE - WALL_WIDTH, CELL_SIZE, WALL_WIDTH, (Color){80, 50, 50, 255});
    x += CELL_SIZE;
    if (x > CELL_SIZE * size) {
      x = 0;
      y += CELL_SIZE;
    }
  }
}

int CoordToIdx(int x, int y, int size) { return x + y * size; }
