#pragma once
#include <raylib.h>
#include <stdint.h>

constexpr int CanvasWidth = 640;
constexpr int CanvasHeight = 360;

// # Environment
constexpr int CELL_SIZE = 32;
constexpr int WALL_WIDTH = 4;

int CoordToIdx(int x, int y, int size);

#define WALL_LEFT (1 << 0)
#define WALL_RIGHT (1 << 1)
#define WALL_UP (1 << 2)
#define WALL_DOWN (1 << 3)
#define VISITED (1 << 4)
#define FRONTIER (1 << 5)

typedef struct {
  uint8_t row;
  uint8_t col;
} Coord;

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

Vector2 gen_map(uint8_t *map, int size);

void draw_map(uint8_t *map, int size);

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
  SpellClasses class;
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
  float health;
} Sprite;

void move_sprite(Sprite *player, uint8_t *map, Vector2 velocity, int mapsize);

// # Animations
bool draw_start_animation(float timePassed);

// # Utils

typedef struct GameContext {
  // Re-Arrange these based on access to optimize cache locality
  GameStage stage;
  int floor;
  Vector2 fragment_location;
  bool open_inventory;
  Camera2D camera;
  Sprite player;

  Spell equippedSpell;

  float timePassed;
  float spellCooldown;
  float castCooldown;
  float dashCooldown;

  uint8_t *map;
  int mapsize;
} GameContext;

void DrawLabel(const char *text, Vector2 pos, float fontSize, Color color);

void setup_ctx(GameContext *ctx);
