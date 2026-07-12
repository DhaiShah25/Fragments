#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#include "global.h"

typedef struct GameContext {
  // Re-Arrange these based on access to optimize cache locality
  GameStage stage;
  int floor;
  GameMap map;
  Vector2 fragment_location;
  bool open_inventory;
  Camera2D camera;
  Sprite player;

  Spell equippedSpell;

  float timePassed;
  float spellCooldown;
  float castCooldown;
  float dashCooldown;
} GameContext;

void UpdateDrawFrame(GameContext *ctx);

Font iosevka40;
Font iosevka20;

Vector2 player_input(Sprite *player, GameMap map) {
  Vector2 velocity = (Vector2){0, 0};
  if (IsKeyDown(KEY_A))
    velocity.x -= 1;
  if (IsKeyDown(KEY_D))
    velocity.x += 1;
  if (IsKeyDown(KEY_W))
    velocity.y -= 1;
  if (IsKeyDown(KEY_S))
    velocity.y += 1;

  if (velocity.x != 0 || velocity.y != 0) {
    velocity = Vector2Normalize(velocity);
    player->dir = velocity;
  }
  velocity = Vector2Scale(velocity, 100);

  return velocity;
}

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
  InitWindow(CanvasWidth, CanvasHeight, "Raylib 6.x Game Jam");

  iosevka40 = LoadFontEx("resources/Iosevka.ttf", 40, nullptr, 0);
  iosevka20 = LoadFontEx("resources/Iosevka.ttf", 20, nullptr, 0);

  GameContext ctx = {
      .stage = StartScreen,
      .open_inventory = false,
      .player = {.center = {TILE_SIZE * 1.5, TILE_SIZE * 1.5},
                 .color = (Color){40, 120, 40, 255},
                 .radius = TILE_SIZE / 8,
                 .speedMultiplier = 1.0f},
      .camera = {.target = {220, 220}, .offset = {360, 360}, .zoom = 2.0f},
      .equippedSpell =
          {
              .class = Damage,
              .castTime = 2.,
              .damage = 10.,
              .type = FrontAttack,
          },
      .castCooldown = 4.0f,
  };

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop_arg(UpdateDrawFrame, &ctx, 0, 1);
#else
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    UpdateDrawFrame(&ctx);
  }
#endif

  UnloadFont(iosevka40);
  UnloadFont(iosevka20);
  CloseWindow();

  return 0;
}

void UpdateDrawFrame(GameContext *ctx) {
  ctx->dashCooldown -= GetFrameTime();
  ctx->spellCooldown -= GetFrameTime();
  ctx->spellCooldown -= GetFrameTime();

  if (ctx->stage >= Movement && ctx->stage != FloorChange) {
    Vector2 velocity = player_input(&ctx->player, ctx->map);
    move_sprite(&ctx->player, ctx->map, velocity);
  }

  if (IsKeyPressed(KEY_E)) {
    ctx->open_inventory = !ctx->open_inventory;
  }

  // Drawing
  BeginDrawing();
  if (ctx->stage == StartAnimation)
    ClearBackground((Color){40, 20, 20, 255});
  else
    ClearBackground(BLACK);

  if (ctx->stage > StartAnimation && ctx->stage < Victory && ctx->stage != FloorChange) {
    ctx->camera.target = ctx->player.center;
    BeginMode2D(ctx->camera);
    draw_map(ctx->map, ctx->fragment_location);
    DrawCircleV(ctx->player.center, ctx->player.radius, ctx->player.color);
    EndMode2D();

    if (CheckCollisionPointCircle(ctx->player.center, ctx->fragment_location, TILE_SIZE * 0.4)) {
      ctx->stage = FloorChange;
      ctx->fragment_location = gen_map(ctx->map);
      ctx->timePassed = 0;

      ctx->player.center = (Vector2){TILE_SIZE * 1.5, TILE_SIZE * 1.5};

      if (ctx->floor == 9) {
        ctx->stage = Victory;
      }
    }
  }

  switch (ctx->stage) {
    case StartScreen:
      Rectangle tutRec = {290, 310, 150, 60};
      DrawRectangleRec(tutRec, (Color){20, 20, 20, 255});
      DrawLabel("Tutorial", (Vector2){300, 320}, 40, RAYWHITE);
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), tutRec)) {
        ctx->stage = StartAnimation;
      }

      Rectangle gameRect = {210, 390, 340, 60};
      DrawRectangleRec(gameRect, (Color){20, 20, 20, 255});
      DrawLabel("Straight To Gameplay", (Vector2){220, 400}, 40, RAYWHITE);
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), gameRect)) {
        ctx->fragment_location = gen_map(ctx->map);
        ctx->stage = Floors;
      }
      break;
    case StartAnimation:
      ctx->timePassed += GetFrameTime();
      if (draw_start_animation(ctx->timePassed)) {
        ctx->timePassed = 0;
        ctx->stage++;
        ctx->fragment_location = gen_map(ctx->map);
      }
      break;
    case Movement:
      float coord = TILE_SIZE + TILE_SIZE / 8;
      if (ctx->player.center.x != coord && ctx->player.center.y != coord) {
        ctx->stage = Dashing;
      }
      DrawLabel("Use WASD keys to move around", (Vector2){0, 0}, 40, RAYWHITE);
      break;
    case Dashing:
      DrawLabel("Right click or use the dash button to dash", (Vector2){0, 0}, 40, RAYWHITE);
      break;
    case Attacking:
      DrawLabel("Click in the direction you want to attack", (Vector2){0, 0}, 40, RAYWHITE);
      break;
    case CastTime:
      DrawLabel("Every spell has a cast time", (Vector2){0, 0}, 40, RAYWHITE);
      break;
    case AdvanceFloor:
      DrawLabel("Collect the picture frame fragment in order\nto move to the next room", (Vector2){0, 0}, 40, RAYWHITE);
      break;
    case FloorChange:
      ctx->timePassed += GetFrameTime();
      if (ctx->timePassed > 4) {
        ctx->floor++;
        ctx->stage = Floors;
        ctx->fragment_location = gen_map(ctx->map);
      }
      DrawRectangle(0, 0, CanvasWidth, CanvasWidth,
                    (Color){20, 10, 10, 255 - 255 * 0.25 * (ctx->timePassed - 2) * (ctx->timePassed - 2)});
      DrawLabel("Completed Floor", (Vector2){260, 360}, 40,
                (Color){255, 255, 255, 255 - 255 * 0.25 * (ctx->timePassed - 2) * (ctx->timePassed - 2)});
      break;
    case Floors:
      char str[9] = "Floor ";
      snprintf(&str[6], 3, "%d", ctx->floor + 1);
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

  if (ctx->stage >= Attacking) {
    if (ctx->dashCooldown <= 0)
      DrawPoly((Vector2){680, 680}, 6, 40, 0, (Color){200, 41, 55, 255});
    else
      DrawPoly((Vector2){680, 680}, 6, 40, 0, (Color){200, 41, 55, 200});

    Vector2 mousePoint = GetMousePosition();

    // This is for dashing
    if (((CheckCollisionPointCircle(mousePoint, (Vector2){680, 680}, 40) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ||
         IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) &&
        ctx->dashCooldown <= 0) {
      if (ctx->stage == Dashing)
        ctx->stage = Attacking;
      ctx->dashCooldown = 1.0;
      Vector2 velocity = Vector2Scale(ctx->player.dir, 2000);
      move_sprite(&ctx->player, ctx->map, velocity);
    }
    // This casts the spell
    else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && ctx->spellCooldown <= 0) {
      if (ctx->stage == Attacking)
        ctx->stage = CastTime;
      if (ctx->equippedSpell.castTime >= 0.2 && GetRandomValue(0, 10) == 10)
        ctx->equippedSpell.castTime -= 0.1;
      if (GetRandomValue(0, 10) == 10)
        ctx->castCooldown -= 0.1;
      ctx->spellCooldown = ctx->castCooldown;
    }
  }
  EndDrawing();
}

void DrawLabel(const char *text, Vector2 pos, float fontSize, Color color) {
  if (fontSize > 30)
    DrawTextEx(iosevka40, text, pos, fontSize, 0, color);
  else
    DrawTextEx(iosevka20, text, pos, fontSize, 0, color);
}
