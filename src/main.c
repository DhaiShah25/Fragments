#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
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
  float speedMultiplier;
} Sprite;

Font iosevka40;
Font iosevka20;

void DrawLabel(const char *text, Vector2 pos, float fontSize, Color color) {
  if (fontSize > 30)
    DrawTextEx(iosevka40, text, pos, fontSize, 0, color);
  else
    DrawTextEx(iosevka20, text, pos, fontSize, 0, color);
}

bool draw_start_animation(float timePassed);

void move_player(Sprite *player, uint8_t map[MAP_SIZE][MAP_SIZE]) {
  player->velocity = (Vector2){0, 0};
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

  player->velocity = Vector2Normalize(player->velocity);

  float factor = 100 * GetFrameTime() * player->speedMultiplier;

  float new_x = player->rect.x + player->velocity.x * factor;
  float new_y = player->rect.y + player->velocity.y * factor;

  int left = new_x / TILE_SIZE;
  int top = player->rect.y / TILE_SIZE;
  int right = (new_x + player->rect.width) / TILE_SIZE;
  int bottom = (player->rect.y + player->rect.height) / TILE_SIZE;

  if (!(map[left][top] || map[right][top] || map[left][bottom] || map[right][bottom])) {
    player->rect.x = new_x;
  }

  left = player->rect.x / TILE_SIZE;
  top = new_y / TILE_SIZE;
  right = (player->rect.x + player->rect.width) / TILE_SIZE;
  bottom = (new_y + player->rect.height) / TILE_SIZE;

  if (!(map[left][top] || map[right][top] || map[left][bottom] || map[right][bottom])) {
    player->rect.y = new_y;
  }
  player->speedMultiplier = 1.0;
}

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
  InitWindow(CanvasWidth, CanvasHeight, "Raylib 6.x Game Jam");

  iosevka40 = LoadFontEx("resources/Iosevka.ttf", 40, nullptr, 0);
  iosevka20 = LoadFontEx("resources/Iosevka.ttf", 20, nullptr, 0);

  int currentFps = 60;
  GameStage stage = GUI;
  uint8_t map[MAP_SIZE][MAP_SIZE] = {0};
  int floor = 0;
  Vector2 fragment_location;
  bool open_inventory = false;

  Sprite player = {.rect = (Rectangle){TILE_SIZE, TILE_SIZE, TILE_SIZE / 4, TILE_SIZE / 4},
                   .color = (Color){40, 120, 40, 255}};

  Camera2D camera = {0};
  camera.target = (Vector2){220, 220};
  camera.offset = (Vector2){360, 360};
  camera.rotation = 0.0f;
  camera.zoom = 2.0f;

  float timePassed = 0.0f;

  SetTargetFPS(currentFps);

  while (!WindowShouldClose()) {
    if (stage >= Movement && stage != FloorChange)
      move_player(&player, map);

    if (IsKeyPressed(KEY_E)) {
      open_inventory = !open_inventory;
    }

    // Drawing
    BeginDrawing();
    if (stage != Animation)
      ClearBackground(BLACK);
    else
      ClearBackground((Color){40, 20, 20, 255});

    if (stage > Animation && stage < Victory && stage != FloorChange) {
      camera.target = (Vector2){player.rect.x + 20, player.rect.y + 20};
      BeginMode2D(camera);
      draw_map(map, fragment_location);
      DrawRectangleRec(player.rect, player.color);
      DrawRectangleRec(player.rect, player.color);
      EndMode2D();

      if (CheckCollisionPointCircle((Vector2){player.rect.x, player.rect.y}, fragment_location, TILE_SIZE / 4)) {
        stage = FloorChange;
        fragment_location = gen_map(map);
        timePassed = 0;

        player.rect.x = TILE_SIZE;
        player.rect.y = TILE_SIZE;

        if (floor == 9) {
          stage = Victory;
        }
      }
    }

    switch (stage) {
      case GUI:
        Rectangle tutRec = {290, 310, 150, 60};
        DrawRectangleRec(tutRec, (Color){20, 20, 20, 255});
        DrawLabel("Tutorial", (Vector2){300, 320}, 40, RAYWHITE);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), tutRec)) {
          stage = Animation;
        }

        Rectangle gameRect = {210, 390, 340, 60};
        DrawRectangleRec(gameRect, (Color){20, 20, 20, 255});
        DrawLabel("Straight To Gameplay", (Vector2){220, 400}, 40, RAYWHITE);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), gameRect)) {
          fragment_location = gen_map(map);
          stage = Floors;
        }
        break;
      case Animation:
        timePassed += GetFrameTime();
        if (draw_start_animation(timePassed)) {
          timePassed = 0;
          stage++;
          fragment_location = gen_map(map);
        }
        break;
      case Movement:
        if (player.rect.x != TILE_SIZE || player.rect.y != TILE_SIZE) {
          stage++;
        }
        DrawLabel("Use WASD keys to move around and right\nclick to dash", (Vector2){0, 0}, 40, RAYWHITE);
        break;
      case Attacks:
        DrawLabel("Left click or use the button on the bottom to attack\n"
                  "in the direction you are\nmoving. You can only have \n"
                  "1 spell equipped at a time.",
                  (Vector2){0, 0}, 40, RAYWHITE);
        break;
      case Escape:
        DrawLabel("Collect the picture frame fragment in order\nto move to the next room", (Vector2){0, 0}, 40,
                  RAYWHITE);
        break;
      case FloorChange:
        timePassed += GetFrameTime();
        if (timePassed > 4) {
          floor++;
          stage = Floors;
          fragment_location = gen_map(map);
        }
        DrawRectangle(0, 0, 720, 720, (Color){20, 10, 10, 255 - 255 * 0.25 * (timePassed - 2) * (timePassed - 2)});
        DrawLabel("Completed Floor", (Vector2){260, 360}, 40,
                  (Color){255, 255, 255, 255 - 255 * 0.25 * (timePassed - 2) * (timePassed - 2)});
        break;
      case Floors:
        char str[20] = "Floor ";
        snprintf(&str[6], 14, "%d", floor + 1);
        DrawLabel(str, (Vector2){0, 0}, 40, RAYWHITE);
        break;
      case Victory:
        DrawLabel("You Won", (Vector2){300, 300}, 40, RAYWHITE);
        DrawLabel("You Broke The Hex", (Vector2){220, 360}, 40, RAYWHITE);
        DrawLabel("You Got Your Cats Back", (Vector2){180, 420}, 40, RAYWHITE);
        break;
      case Loss:
        break;
    }

    if (stage >= Attacks) {
      DrawPoly((Vector2){610, 640}, 6, 40, 0, BLUE);
      DrawPoly((Vector2){680, 680}, 6, 40, 0, RED);

      Vector2 mousePoint = GetMousePosition();

      if (CheckCollisionPointCircle(mousePoint, (Vector2){610, 640}, 40) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        TraceLog(LOG_WARNING, "BLUE_PRESSED");
        if (stage == Attacks)
          stage++;
      }
      if (CheckCollisionPointCircle(mousePoint, (Vector2){680, 680}, 40) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        TraceLog(LOG_TRACE, "Dashed");
        player.speedMultiplier = 10;
      }
    }

    EndDrawing();
  }

  UnloadFont(iosevka40);
  UnloadFont(iosevka20);
  CloseWindow();

  return 0;
}

bool draw_start_animation(float timePassed) {
  if (timePassed > 10.0) {
    return true;
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
  return false;
}
