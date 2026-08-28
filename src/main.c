#include "global.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

void update_draw_frame(void *ctx);

Font iosevka40;
Font iosevka20;

Vector2 player_input(Sprite *player) {
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

  GameContext ctx = {0};
  setup_ctx(&ctx);

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop_arg(update_draw_frame, &ctx, 0, 1);
#else
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    update_draw_frame(&ctx);
  }
#endif

  UnloadFont(iosevka40);
  UnloadFont(iosevka20);
  CloseWindow();

  return 0;
}

void update_draw_frame(void *ctxptr) {
  GameContext *ctx = (GameContext *)ctxptr;
  ctx->dashCooldown -= GetFrameTime();
  ctx->spellCooldown -= GetFrameTime();
  ctx->spellCooldown -= GetFrameTime();

  if (ctx->stage >= StartScreen && ctx->stage != FloorChange) {
    Vector2 velocity = player_input(&ctx->player);
    move_sprite(&ctx->player, ctx->map, velocity, ctx->mapsize);
  }

  if (IsKeyPressed(KEY_E)) {
    ctx->open_inventory = !ctx->open_inventory;
  }

  if (ctx->player.health <= 0) {
    ctx->stage = Loss;
  }

  // Drawing
  BeginDrawing();
  ClearBackground(BLACK);

  if (ctx->stage > StartScreen && ctx->stage < Victory && ctx->stage != FloorChange) {
    ctx->camera.target.x = ctx->player.center.x +
                           (ctx->camera.target.x - ctx->player.center.x) * exp(-4 * GetFrameTime());
    ctx->camera.target.y = ctx->player.center.y +
                           (ctx->camera.target.y - ctx->player.center.y) * exp(-4 * GetFrameTime());
    BeginMode2D(ctx->camera);
    draw_map(ctx->map, ctx->mapsize);
    DrawCircleV(ctx->fragment_location, 4.0, (Color){245, 235, 235, 255});
    DrawCircleV(ctx->player.center, ctx->player.radius, ctx->player.color);
    EndMode2D();

    if (CheckCollisionPointCircle(ctx->player.center, ctx->fragment_location, CELL_SIZE * 0.4)) {
      ctx->stage = FloorChange;
      ctx->fragment_location = gen_map(ctx->map, ctx->mapsize);
      ctx->timePassed = 0;

      ctx->player.center = (Vector2){CELL_SIZE * 1.5, CELL_SIZE * 1.5};

      if (ctx->floor == 9) {
        ctx->stage = Victory;
      }
    }
  }

  switch (ctx->stage) {
    case StartScreen:
      Vector2 points[] = {
          // Left Cover
          {0.0f, 0.0f},     //
          {0.0f, 320.0f},   //
          {310.0f, 40.0f},  //
          {310.0f, 360.0f}, //
          // Right Cover
          {330.0f, 40.0f},  // Point 6
          {330.0f, 360.0f}, // Point 7
          {640.0f, 0.0f},   //
          {640.0f, 320.0f}, //
          // Left Pages
          {10.0f, 5.0f},    //
          {10.0f, 315.0f},  //
          {310.0f, 45.0f},  //
          {310.0f, 355.0f}, //
          // Right Pages
          {330.0f, 45.0f},  // Point 6
          {330.0f, 355.0f}, // Point 7
          {630.0f, 5.0f},   //
          {630.0f, 315.0f}  //
      };
      DrawTriangleStrip(points, 4, DARKGREEN);
      DrawRectangle(310, 40, 20, 320, (Color){0, 100, 44, 255});
      DrawTriangleStrip(&points[4], 4, DARKGREEN);
      DrawRectangle(310, 45, 20, 310, (Color){201, 166, 121, 255});
      DrawTriangleStrip(&points[8], 4, BEIGE);
      DrawTriangleStrip(&points[12], 4, BEIGE);

      Rectangle gameRect = {340, 110, 230, 60};
      DrawLabel("Start Playing", (Vector2){350, 120}, 40, (Color){131, 96, 51, 255});
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
          CheckCollisionPointRec(GetMousePosition(), gameRect)) {
        ctx->fragment_location = gen_map(ctx->map, ctx->mapsize);
        ctx->stage = Floors;
      }
      break;
    case FloorChange:
      ctx->timePassed += GetFrameTime();
      if (ctx->timePassed > 4) {
        ctx->floor++;
        ctx->stage = Floors;
        ctx->mapsize = 5 + ctx->floor * 2;
        ctx->fragment_location = gen_map(ctx->map, ctx->mapsize);
      }
      DrawRectangle(
          0, 0, CanvasWidth, CanvasWidth,
          (Color){20, 10, 10, 255 - 255 * 0.25 * (ctx->timePassed - 2) * (ctx->timePassed - 2)});
      DrawLabelCentered(
          "Completed Floor", (Vector2){320, 180}, 80,
          (Color){255, 255, 255, 255 - 255 * 0.25 * (ctx->timePassed - 2) * (ctx->timePassed - 2)});
      break;
    case Floors:
      char str[9] = "Floor ";
      snprintf(&str[6], 3, "%d", ctx->floor + 1);
      DrawLabel(str, (Vector2){0, 0}, 40, RAYWHITE);
      break;
    case Victory:
      DrawLabelCentered("You Won", (Vector2){320, 140}, 40, RAYWHITE);
      DrawLabelCentered("You Broke The Hex", (Vector2){320, 200}, 40, RAYWHITE);
      break;
    case Loss:
      Rectangle lossRect = {290, 310, 150, 60};
      DrawRectangleRec(lossRect, (Color){20, 20, 20, 255});
      DrawLabelCentered("Restart!", (Vector2){300, 320}, 40, RAYWHITE);
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
          CheckCollisionPointRec(GetMousePosition(), lossRect)) {
        setup_ctx(ctx);
      }
      break;
  }

  if (ctx->dashCooldown <= 0)
    DrawPoly((Vector2){680, 680}, 6, 40, 0, (Color){200, 41, 55, 255});
  else
    DrawPoly((Vector2){680, 680}, 6, 40, 0, (Color){200, 41, 55, 200});

  // This is for dashing
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && ctx->dashCooldown <= 0) {
    ctx->dashCooldown = 1.0;
    Vector2 velocity = Vector2Scale(ctx->player.dir, 2000);
    move_sprite(&ctx->player, ctx->map, velocity, ctx->mapsize);
  }
  // This casts the spell
  else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && ctx->spellCooldown <= 0) {
    if (ctx->equippedSpell.castTime >= 0.2 && GetRandomValue(0, 10) == 10)
      ctx->equippedSpell.castTime -= 0.1;
    if (GetRandomValue(0, 10) == 10)
      ctx->castCooldown -= 0.1;
    ctx->spellCooldown = ctx->castCooldown;
  }
  EndDrawing();
}

void DrawLabel(const char *text, Vector2 pos, float fontSize, Color color) {
  if (fontSize > 30)
    DrawTextEx(iosevka40, text, pos, fontSize, 0, color);
  else
    DrawTextEx(iosevka20, text, pos, fontSize, 0, color);
}

void DrawLabelCentered(const char *text, Vector2 pos, float fontSize, Color color) {
  if (fontSize > 30) {
    Vector2 size = MeasureTextEx(iosevka40, text, fontSize, 0);
    DrawTextEx(iosevka40, text, Vector2Subtract(pos, Vector2Scale(size, 0.5)), fontSize, 0, color);
  } else {
    Vector2 size = MeasureTextEx(iosevka20, text, fontSize, 0);
    DrawTextEx(iosevka20, text, Vector2Subtract(pos, Vector2Scale(size, 0.5)), fontSize, 0, color);
  }
}
