#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  Rectangle rect;
  // Texture2D texture;
  Color color;
} Sprite;

constexpr int CanvasWidth = 640;
constexpr int CanvasHeight = 360;

int scale = 0;

void move_player(Sprite *player) {
  if (IsKeyDown(KEY_A)) {
    player->rect.x -= 100 * GetFrameTime();
  }
  if (IsKeyDown(KEY_D)) {
    player->rect.x += 100 * GetFrameTime();
  }
  if (IsKeyDown(KEY_W)) {
    player->rect.y -= 100 * GetFrameTime();
  }
  if (IsKeyDown(KEY_S)) {
    player->rect.y += 100 * GetFrameTime();
  }
}

int main(void) {
  InitWindow(CanvasWidth << scale, CanvasHeight << scale,
             "Raylib 6.x Game Jam");

  int currentFps = 60;
  // bool tutorial_completed = true;
  // bool paused = false;

  Sprite player = {.rect = (Rectangle){0, 0, 40, 40},
                   .color = (Color){40, 120, 40, 255}};

  SetTargetFPS(currentFps);

  while (!WindowShouldClose()) {
    // Input
    move_player(&player);

    // Drawing
    BeginDrawing();
    ClearBackground((Color){175, 200, 255, 255});

    DrawRectangleRec(player.rect, player.color);

    EndDrawing();
  }
  CloseWindow();

  return 0;
}
