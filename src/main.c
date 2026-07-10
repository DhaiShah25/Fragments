#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "environment.c"
#include "spells.c"

constexpr int CanvasWidth = 720;
constexpr int CanvasHeight = 720;

typedef struct Sprite {
  Rectangle rect;
  // Texture2D texture;
  Color color;
  Vector2 velocity;
} Sprite;

Font iosevka;

void DrawLabel(const char *text, Vector2 pos, float fontSize, Color color) {
  DrawTextEx(iosevka, text, pos, fontSize, 0, color);
}

void draw_start_animation(float *timePassed, GameStage *stage);

void move_player(Sprite *player) {
  player->velocity = (Vector2){0, 0};
  // TODO: Normalize Player Movement Velocity
  if (IsKeyDown(KEY_A)) {
    player->velocity.x -= 1;
  }
  if (IsKeyDown(KEY_D)) {
    player->velocity.x += 1;
  }
  if (IsKeyDown(KEY_W)) {
    player->velocity.y -= 1;
  }
  if (IsKeyDown(KEY_S)) {
    player->velocity.y += 1;
  }

  // Vector2 touch_pos = GetTouchPosition(0);
  // if (touch_pos.x != -1 && touch_pos.y != -1 && player->velocity.x == 0 && player->velocity.y == 0)
  //   player->velocity = Vector2Subtract(touch_pos, (Vector2){player->rect.x, player->rect.y});

  player->velocity = Vector2Normalize(player->velocity);
  player->rect.x += player->velocity.x * GetFrameTime() * 100;
  player->rect.y += player->velocity.y * GetFrameTime() * 100;

  // if (player->rect.x + player->rect.width > CanvasWidth) {
  //   player->rect.x = CanvasWidth - player->rect.width;
  // } else if (player->rect.x < 0) {
  //   player->rect.x = 0;
  // }
  // if (player->rect.y + player->rect.height > CanvasHeight) {
  //   player->rect.y = CanvasHeight - player->rect.height;
  // } else if (player->rect.y < 0) {
  //   player->rect.y = 0;
  // }
}

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
  InitWindow(CanvasWidth, CanvasHeight, "Raylib 6.x Game Jam");

  iosevka = LoadFontEx("resources/Iosevka.ttf", 40, nullptr, 0);

  int currentFps = 60;
  GameStage stage = Movement;
  uint16_t map[15] = {0};
  int floor = -1;
  Vector2 fragment_location;

  Sprite player = {.rect = (Rectangle){200, 200, 40, 40}, .color = (Color){40, 120, 40, 255}};

  Camera2D camera = {0};
  camera.target = (Vector2){220, 220};
  camera.offset = (Vector2){360, 360};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  float timePassed = 0.0f;

  SetTargetFPS(currentFps);

  while (!WindowShouldClose()) {
    if (stage >= Movement)
      move_player(&player);

    // Drawing
    BeginDrawing();
    ClearBackground(BLACK);

    switch (stage) {
      case Animation:
        timePassed += GetFrameTime();
        draw_start_animation(&timePassed, &stage);
        break;
      case Movement:
        DrawRectangleRec(player.rect, player.color);
        if (player.rect.x != 200 || player.rect.y != 200) {
          stage++;
          fragment_location = gen_map(map);
          TraceLog(LOG_INFO, "Fragment Location: %f, %f", fragment_location.x, fragment_location.y);
        } else
          DrawLabel("Use WASD keys to move around", (Vector2){0, 0}, 40, RAYWHITE);
        break;
      case Escape:
        camera.target = (Vector2){player.rect.x + 20, player.rect.y + 20};
        BeginMode2D(camera);
        for (int x = 0; x < 15; x++) {
          for (int y = 0; y < 15; y++) {
            if (map[x] & 1 << y) {
              DrawRectangle(x * 160, y * 160, 160, 160, (Color){100, 20, 20, 255});
            } else
              DrawRectangle(x * 160, y * 160, 160, 160, (Color){40, 20, 20, 255});
          }
        }
        DrawRectangleRec(player.rect, player.color);
        EndMode2D();
        DrawLabel("You have to collect the picture frame\nfragment in order to move to the next room", (Vector2){0, 0},
                  40, RAYWHITE);
        break;
      case Attacks:
        DrawLabel("Use the JKL keys or the buttons on the bottom\nright to "
                  "attack in the direction you are\nmoving",
                  (Vector2){0, 0}, 40, RAYWHITE);
        floor++;
        break;
      case Effects:
        break;
      case Weaknesses:
        break;
      case CastTime:
        break;
      case Merging:
        break;
      case Floors:
        break;
      case Victory:
        break;
      case Loss:
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

  gen_map(map);
  gen_map(map);

  UnloadFont(iosevka);
  CloseWindow();

  return 0;
}

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
