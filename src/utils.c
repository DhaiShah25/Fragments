#include "global.h"
#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>

void setup_ctx(GameContext *ctx) {
  ctx->stage = StartScreen;
  ctx->floor = 0;
  ctx->fragment_location = (Vector2){0, 0};

  ctx->mapsize = 15;
  ctx->map = malloc(ctx->mapsize * ctx->mapsize * sizeof(uint8_t));

  ctx->open_inventory = false;
  ctx->camera = (Camera2D){.target = {220, 220}, .offset = {360, 360}, .zoom = 0.5f};
  ctx->player = (Sprite){
      .center = {TILE_SIZE * 1.5, TILE_SIZE * 1.5},
      .color = (Color){40, 120, 40, 255},
      .radius = TILE_SIZE / 8,
      .speedMultiplier = 1.0f,
      .health = 100.0f,
  };
  ctx->equippedSpell = (Spell){
      .class = Damage,
      .castTime = 2.,
      .damage = 10.,
      .type = FrontAttack,
  };
  ctx->timePassed = 0.0;
  ctx->castCooldown = 0.0;
  ctx->dashCooldown = 0.0;
  ctx->spellCooldown = 4.0;
}
