#pragma once
#include <raylib.h>
#include <stdint.h>

constexpr int CanvasWidth = 720;
constexpr int CanvasHeight = 720;

// # Environment
constexpr int MAP_SIZE = 15;
constexpr float TILE_SIZE = 32.0;

typedef char GameMap[MAP_SIZE][MAP_SIZE];

typedef enum GameStage : uint8_t {
  StartScreen,

  StartAnimation,

  // Tutorial
  Movement,
  Dashing,
  Attacking,
  CastTime,
  AdvanceFloor,

  // Gameplay
  FloorChange,
  Floors,
  Victory,
  Loss
} GameStage;

Vector2 gen_map(GameMap map);

void draw_map(GameMap map, Vector2 fragment_location);

// # Spells
typedef enum SpellClasses : uint8_t {
  Slowdown = 1 << 1,
  Knockback = 1 << 2,
  Damage = 1 << 3,
  Speedup = 1 << 4,
} SpellClasses;

typedef enum SpellTypess : uint8_t {
  CircleAOE,
  FrontAttack,
  FrontArea,
} SpellTypes;

typedef struct Spell {
  const SpellClasses class;
  float castTime;
  float damage;
  SpellTypes type;
} Spell;

Spell merge(Spell lhs, Spell rhs);

// # Sprites

typedef struct Sprite {
  Vector2 center;
  float radius;
  Color color;
  Vector2 dir;
  float speedMultiplier;
  uint8_t health;
} Sprite;

void move_sprite(Sprite *player, GameMap map, Vector2 velocity);

// # Animations
bool draw_start_animation(float timePassed);

// # Utils
void DrawLabel(const char *text, Vector2 pos, float fontSize, Color color);
