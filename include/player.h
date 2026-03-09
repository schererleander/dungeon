#ifndef PLAYER_H
#define PLAYER_H

#include "map_manager.h"
#include "raylib.h"

typedef struct Player {
  Vector2 position;
  float speed;
  Rectangle bounds;
  
  // Animation state
  TmxTileset tileset;
  int currentFrame;
  float frameTime;
  int state; // 0: idle, 1: walk, etc.
  bool facingRight;
} Player;

void UpdatePlayer(Player *p, MapManager *mapMgr);
void DrawPlayer(Player *p);

#endif
