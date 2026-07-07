#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  Rectangle rect;
  // Texture2D texture;
  Color color;
} Sprite;

constexpr int CanvasWidth = 720;
constexpr int CanvasHeight = 720;

enum TutorialStage {
  Movement,
  Attacks,
  Effects,
  Weaknesses,
  CastTime,
  Merging,
};

void move_player(Sprite *player) {
  // Normalize Player Movement Velocity
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

  if (player->rect.x + player->rect.width > CanvasWidth) {
    player->rect.x = CanvasWidth - player->rect.width;
  } else if (player->rect.x < 0) {
    player->rect.x = 0;
  }
  if (player->rect.y + player->rect.height > CanvasHeight) {
    player->rect.y = CanvasHeight - player->rect.height;
  } else if (player->rect.y < 0) {
    player->rect.y = 0;
  }
}

int main(void) {
  InitWindow(CanvasWidth, CanvasHeight, "Raylib 6.x Game Jam");

  Font iosevka = LoadFontEx("resources/Iosevka.ttf", 40, nullptr, 0);

  printf("%d, %d, %d\n", iosevka.baseSize, iosevka.glyphCount,
         iosevka.glyphPadding);

  int currentFps = 60;
  enum TutorialStage stage = Movement;

  Sprite player = {.rect = (Rectangle){0, 0, 40, 40},
                   .color = (Color){40, 120, 40, 255}};

  SetTargetFPS(currentFps);

  while (!WindowShouldClose()) {
    // Input
    move_player(&player);

    // Drawing
    BeginDrawing();
    ClearBackground((Color){25, 10, 10, 255});

    if (IsKeyPressed(KEY_SPACE))
      stage++;

    DrawRectangleRec(player.rect, player.color);

    switch (stage) {
      case Movement:
        // DrawText("Use WASD keys to move around", 0, 0, 20, LIGHTGRAY);
        DrawTextEx(iosevka, "Use WASD keys to move around", (Vector2){0, 0}, 40,
                   0, LIGHTGRAY);
        break;
      case Attacks:
        DrawText("Use the JKL keys to attack\nYou have 3 attacks you can equip "
                 "at any point in time",
                 0, 0, 40, RAYWHITE);
        break;
      case Effects:
        break;
      case Weaknesses:
        break;
      case CastTime:
        break;
      case Merging:
        break;
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
