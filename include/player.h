#pragma once

#include "raylib.h"
#include "raytmx.h"

#include "map.h"

typedef enum { PLAYER_IDLE, PLAYER_WALK, PLAYER_ATTACK, PLAYER_DIE, PLAYER_FALL } PlayerState;

typedef struct {
  int potions;
  int bombs;
  int currency;
} Inventory;

typedef struct {
  Vector2 position;
  float speed;
  Rectangle bounds;

  TmxTileset tileset;
  int currentFrame;
  float frameTime;
  PlayerState state;
  bool facingRight;

  int health;
  int maxHealth;
  Inventory inventory;
} Player;

void UpdatePlayer(Player *player, Map *map, struct Entities *entities);
void DrawPlayer(Player *player);
