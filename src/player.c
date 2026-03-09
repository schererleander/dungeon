#include "player.h"
#include "raymath.h"
#include "globals.h"

void UpdatePlayer(Player *p, MapManager *mapMgr) {
  Vector2 move = {0};
  if (IsKeyDown(KEY_W)) move.y -= 1;
  if (IsKeyDown(KEY_S)) move.y += 1;
  if (IsKeyDown(KEY_A)) move.x -= 1;
  if (IsKeyDown(KEY_D)) move.x += 1;

  int newState = (Vector2Length(move) > 0) ? 1 : 0;
  bool newFacingRight = p->facingRight;
  if (move.x > 0) newFacingRight = true;
  else if (move.x < 0) newFacingRight = false;

  if (newState != p->state || newFacingRight != p->facingRight) {
    p->state = newState;
    p->facingRight = newFacingRight;
    p->currentFrame = 0;
    p->frameTime = 0;
  }

  // Update bounds based on current position (Feet hitbox: 8x6 centered horizontally, offset bottom)
  p->bounds = (Rectangle){ p->position.x - 4, p->position.y + 2, 8, 6 };

  if (Vector2Length(move) > 0) {
    move = Vector2Scale(Vector2Normalize(move), p->speed * GetFrameTime());

    // Try moving X
    Rectangle nextX = p->bounds;
    nextX.x += move.x;
    if (!IsWallCollision(mapMgr, nextX)) {
      p->position.x += move.x;
      p->bounds.x += move.x;
    }

    // Try moving Y
    Rectangle nextY = p->bounds;
    nextY.y += move.y;
    if (!IsWallCollision(mapMgr, nextY)) {
      p->position.y += move.y;
      p->bounds.y += move.y;
    }
  }

  // Determine base tile ID from tileset for current animation
  int baseTileId = 0;
  if (p->state == 0) baseTileId = p->facingRight ? 0 : 7;
  else if (p->state == 1) baseTileId = p->facingRight ? 14 : 21;

  // Animation Update using Tiled metadata
  TmxTilesetTile *tileData = NULL;
  for (uint32_t i = 0; i < p->tileset.tilesLength; i++) {
    if (p->tileset.tiles[i].id == (uint32_t)baseTileId) {
      tileData = &p->tileset.tiles[i];
      break;
    }
  }

  if (tileData && tileData->hasAnimation) {
    p->frameTime += GetFrameTime(); // raytmx stores durations in seconds
    float duration = tileData->animation.frames[p->currentFrame].duration;
    if (p->frameTime >= duration) {
      p->frameTime -= duration;
      p->currentFrame = (p->currentFrame + 1) % tileData->animation.framesLength;
    }
  }
}

void DrawPlayer(Player *p) {
  int baseTileId = 0;
  if (p->state == 0) baseTileId = p->facingRight ? 0 : 7;
  else if (p->state == 1) baseTileId = p->facingRight ? 14 : 21;

  TmxTilesetTile *tileData = NULL;
  for (uint32_t i = 0; i < p->tileset.tilesLength; i++) {
    if (p->tileset.tiles[i].id == (uint32_t)baseTileId) {
      tileData = &p->tileset.tiles[i];
      break;
    }
  }

  int displayTileId = baseTileId;
  if (tileData && tileData->hasAnimation) {
    displayTileId = tileData->animation.frames[p->currentFrame].gid;
  }

  // Calculate source rectangle from tile ID
  int col = displayTileId % p->tileset.columns;
  int row = displayTileId / p->tileset.columns;

  Rectangle source = {
    (float)(col * TILE_SIZE),
    (float)(row * TILE_SIZE),
    (float)TILE_SIZE,
    (float)TILE_SIZE
  };
  
  Vector2 pos = { p->position.x - (float)TILE_SIZE / 2.0f, p->position.y - (float)TILE_SIZE / 2.0f };
  DrawTextureRec(p->tileset.image.texture, source, pos, WHITE);

  if (debugMode) {
    DrawRectangleLinesEx(p->bounds, 1, GREEN);
  }
}
