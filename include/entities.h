#pragma once

#include "raylib.h"
#include "raytmx.h"

#include "map.h"
#include "player.h"

#define MAX_ENTITIES 120

typedef enum {
  PICKUP_COIN,
  PICKUP_POTION,
  PICKUP_BOMB,
  PICKUP_FULL_HEART,
  PICKUP_HALF_HEART,
  PICKUP_GOLD_BAR,
  PICKUP_GOLD_NUGGET
} PickupType;

static const int PICKUP_TILE_ID[] = {0, 4, 8, 12, 16, 20, 24};

typedef enum { STATE_IDLE, STATE_PATROL, STATE_CHASE, STATE_ATTACK } EnemyState;

typedef struct {
  PickupType type;
  Vector2 position;
  Rectangle bounds;
  int value;
  bool active;
  uint32_t gid;
} Pickup;

typedef struct {
  Vector2 position;
  Rectangle bounds;
  int uniqueId;
  bool active;
  uint32_t gid;
} Key;

typedef struct {
  Vector2 position;
  Rectangle bounds;
  int health;
  EnemyState state;

  TmxTileset tileset;
  int currentFrame;
  float frameTime;
  bool facingRight;

  bool active;
} Bat;

typedef struct {
  Vector2 position;
  Rectangle bounds;
  int health;
  EnemyState state;

  TmxTileset tileset;
  int currentFrame;
  float frameTime;
  bool facingRight;

  bool active;
} Slime;

typedef struct {
  Vector2 position;
  Rectangle bounds;
  int health;
  EnemyState state;

  TmxTileset tileset;
  int currentFrame;
  float frameTime;
  bool facingRight;

  bool active;
} FlyingSkull;

typedef struct {
  Vector2 position;
  Rectangle bounds;
  bool active;
} Vase;

typedef struct {
  Vector2 position;
  Rectangle bounds;
  bool active;
} Crate;

struct Entities {
  Pickup pickups[MAX_ENTITIES];
  int pickupsCount;

  Key keys[MAX_ENTITIES];
  int keysCount;

  Bat bats[MAX_ENTITIES];
  int batsCount;

  Slime slimes[MAX_ENTITIES];
  int slimesCount;

  FlyingSkull flyingSkulls[MAX_ENTITIES];
  int flyingSkullsCount;

  Vase vases[MAX_ENTITIES];
  int vasesCount;

  Crate crates[MAX_ENTITIES];
  int cratesCount;
};

void UpdateBat(Bat *bat, Player *player, Map *map);
void DrawBat(Bat *bat);

void UpdateSlime(Slime *slime, Player *player, Map *map);
void DrawSlime(Slime *slime);

void UpdateFlyingSkull(FlyingSkull *skull, Player *player, Map *map);
void DrawFlyingSkull(FlyingSkull *skull);
