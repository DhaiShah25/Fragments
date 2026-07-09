#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

Font iosevka;

typedef enum : uint8_t {
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
} GameStage;

inline void DrawLabel(const char *text, Vector2 pos, float fontSize, Color color) {
  DrawTextEx(iosevka, text, pos, fontSize, 0, color);
}

inline void draw_start_animation(float *timePassed, GameStage *stage);

typedef struct {
  Rectangle rect;
  // Texture2D texture;
  Color color;
} Sprite;

constexpr int CanvasWidth = 720;
constexpr int CanvasHeight = 720;

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
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
  InitWindow(CanvasWidth, CanvasHeight, "Raylib 6.x Game Jam");

  iosevka = LoadFontEx("resources/Iosevka.ttf", 40, nullptr, 0);

  int currentFps = 60;
  GameStage stage = Animation;

  Sprite player = {.rect = (Rectangle){0, 0, 40, 40}, .color = (Color){40, 120, 40, 255}};

  float timePassed = 0.0f;

  SetTargetFPS(currentFps);

  while (!WindowShouldClose()) {
    // Input
    if (stage >= Movement)
      move_player(&player);

    // Drawing
    BeginDrawing();
    ClearBackground((Color){40, 20, 20, 255});

    if (stage >= Movement) {
      DrawRectangleRec(player.rect, player.color);
    }

    switch (stage) {
      case Animation:
        timePassed += GetFrameTime();
        draw_start_animation(&timePassed, &stage);
        break;
      case Movement:
        if (player.rect.x != 0 || player.rect.y != 0)
          stage++;
        else
          DrawLabel("Use WASD keys to move around", (Vector2){0, 0}, 40, RAYWHITE);
        break;
      case Escape:
        DrawLabel("You have to reach the door in the room", (Vector2){0, 0}, 40, RAYWHITE);
        break;
      case Attacks:
        DrawLabel("Use the JKL keys or the buttons on the bottom\nright to "
                  "attack in the direction you are\nmoving",
                  (Vector2){0, 0}, 40, RAYWHITE);
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

    if (stage > Attacks) {
      DrawPoly((Vector2){610, 640}, 6, 40, 0, BLUE);
      DrawPoly((Vector2){680, 680}, 6, 40, 0, RED);
      DrawPoly((Vector2){680, 600}, 6, 40, 0, BROWN);

      Vector2 mousePoint = GetMousePosition();

      if (CheckCollisionPointCircle(mousePoint, (Vector2){610, 640}, 40) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        TraceLog(LOG_WARNING, "BLUE_PRESSED");
      } else if (CheckCollisionPointCircle(mousePoint, (Vector2){680, 680}, 40) &&
                 IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        TraceLog(LOG_WARNING, "BE_PRESSED");
      } else if (CheckCollisionPointCircle(mousePoint, (Vector2){680, 600}, 40) &&
                 IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        TraceLog(LOG_WARNING, "ESSED");
      }
    }

    EndDrawing();
  }

  UnloadFont(iosevka);
  CloseWindow();

  return 0;
}

enum SpellClasses : uint8_t { Fire, Water, Earth, Air, Shadow, Light };

struct Spell {
  const char *const name;
  const enum SpellClasses class;
  float castTime;
  uint8_t level;
};

void draw_start_animation(float *timePassedptr, GameStage *stage) {
  float timePassed = *timePassedptr;
  if (timePassed > 10.0) {
    (*stage)++;
    timePassed = 0.0;
  } else if (timePassed > 6.0) {
    DrawCircleV((Vector2){360, 360}, 40, (Color){0, 0, 0, 255});

    DrawLabel("Why Am I All Alone?", (Vector2){285, 410}, 20, (Color){245, 245, 245, 255 - (timePassed - 6) * 64});
    DrawLabel("Don't Leave Me All Alone", (Vector2){265, 440}, 20,
              (Color){245, 245, 245, 255 - 255 * 0.25 * (timePassed - 8) * (timePassed - 8)});
    DrawLabel("Why Can't I See Anyone?", (Vector2){270, 470}, 20, (Color){245, 245, 245, (timePassed - 6) * 64});
    DrawRectangleRec((Rectangle){0, 0, 720, 720}, (Color){0, 0, 0, (timePassed - 6) * 64});
  } else if (timePassed > 3.0) {
    DrawCircleV((Vector2){360, 360}, 40, (Color){0, 0, 0, 255});

    // Friends
    DrawCircleV((Vector2){320, 390}, 20, (Color){0, 0, 0, 255 - (timePassed - 3) * 85});
    DrawCircleV((Vector2){400, 310}, 20, (Color){0, 0, 0, 255 - (timePassed - 3) * 85});
  } else {
    DrawCircleV((Vector2){100 + (timePassed - 1.5) * (timePassed - 1.5) * -50, 360}, 40, (Color){0, 0, 0, 255});

    if (timePassed > 1.5) {
      DrawRectangle(140 + 220 / 1.5 * (timePassed - 1.5), 360, 20, 4, RAYWHITE);
    }

    if (timePassed > 2.75) {
      DrawCircle(340 - (timePassed - 2.75) * 200, 320, 5, (Color){100, 50, 100, 255});
      DrawCircle(320 - (timePassed - 2.75) * 200, 380, 5, (Color){100, 50, 100, 255});
      DrawCircle(360 - (timePassed - 2.75) * 200, 340, 5, (Color){100, 50, 100, 255});
      DrawCircle(380 - (timePassed - 2.75) * 200, 340, 5, (Color){100, 50, 100, 255});
      DrawCircle(330 - (timePassed - 2.75) * 200, 350, 5, (Color){100, 50, 100, 255});
      DrawCircle(370 - (timePassed - 2.75) * 200, 390, 5, (Color){100, 50, 100, 255});
    }

    DrawCircleV((Vector2){360, 360}, 40, (Color){0, 0, 0, 255});
    // Friends
    DrawCircleV((Vector2){320, 390}, 20, (Color){0, 0, 0, 255});
    DrawCircleV((Vector2){400, 310}, 20, (Color){0, 0, 0, 255});
  }
}
