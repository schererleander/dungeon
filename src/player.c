#include "player.h"
#include "entities.h"
#include "raylib.h"
#include "raymath.h"

Vector2 GetInputVector() {
  Vector2 input = {0};
  if (IsKeyDown(KEY_W))
    input.y -= 1;
  if (IsKeyDown(KEY_S))
    input.y += 1;
  if (IsKeyDown(KEY_A))
    input.x -= 1;
  if (IsKeyDown(KEY_D))
    input.x += 1;
  return input;
}

static void CollectPickups(Player *player, Entities *entities) {
  for (int i = 0; i < entities->pickupsCount; i++) {
    Pickup *p = &entities->pickups[i];
    if (!p->active)
      continue;
    if (!CheckCollisionRecs(player->bounds, p->bounds))
      continue;

    switch (p->type) {
    case PICKUP_FULL_HEART:
      if (player->health >= player->maxHealth)
        continue;
      player->health = Clamp(player->health + 2, 0, player->maxHealth);
      break;
    case PICKUP_HALF_HEART:
      if (player->health >= player->maxHealth)
        continue;
      player->health = Clamp(player->health + 1, 0, player->maxHealth);
      break;
    case PICKUP_COIN:
      player->inventory.currency += 1;
      break;
    case PICKUP_GOLD_NUGGET:
      player->inventory.currency += 5;
      break;
    case PICKUP_GOLD_BAR:
      player->inventory.currency += 10;
      break;
    case PICKUP_POTION:
      player->inventory.potions += 1;
      break;
    case PICKUP_BOMB:
      player->inventory.bombs += 1;
      break;
    }

    p->active = false;
  }
}

void AnimatePlayer(Player *player) {
  int baseTileId = 0;
  if (player->state == 0)
    baseTileId = player->facingRight ? 0 : 7;
  else if (player->state == 1)
    baseTileId = player->facingRight ? 14 : 21;

  TmxTilesetTile *tileData = NULL;
  for (uint32_t i = 0; i < player->tileset.tilesLength; i++) {
    if (player->tileset.tiles[i].id == (uint32_t)baseTileId) {
      tileData = &player->tileset.tiles[i];
      break;
    }
  }

  if (tileData && tileData->hasAnimation) {
    player->frameTime += GetFrameTime(); // raytmx stores durations in seconds
    float duration = tileData->animation.frames[player->currentFrame].duration;
    if (player->frameTime >= duration) {
      player->frameTime -= duration;
      player->currentFrame =
          (player->currentFrame + 1) % tileData->animation.framesLength;
    }
  }
}

void UpdatePlayer(Player *player, Map *map, struct Entities *entities) {
  Vector2 move = GetInputVector();

  PlayerState newState = (Vector2Length(move) > 0) ? PLAYER_WALK : PLAYER_IDLE;
  bool newFacingRight = player->facingRight;
  if (move.x > 0)
    newFacingRight = true;
  else if (move.x < 0)
    newFacingRight = false;

  if (newState != player->state || newFacingRight != player->facingRight) {
    player->state = newState;
    player->facingRight = newFacingRight;
    player->currentFrame = 0;
    player->frameTime = 0;
  }

  if (Vector2Length(move) > 0) {
    move = Vector2Scale(Vector2Normalize(move), player->speed * GetFrameTime());

    Rectangle nextX = player->bounds;
    nextX.x += move.x;

    if (!isCollision(map, nextX)) {
      player->position.x += move.x;
      player->bounds.x += move.x;
    }

    Rectangle nextY = player->bounds;
    nextY.y += move.y;

    if (!isCollision(map, nextY)) {
      player->position.y += move.y;
      player->bounds.y += move.y;
    }
  }

  if (entities)
    CollectPickups(player, entities);

  AnimatePlayer(player);
}

void DrawPlayer(Player *player) {
  int baseTileId = 0;
  if (player->state == 0)
    baseTileId = player->facingRight ? 0 : 7;
  else if (player->state == 1)
    baseTileId = player->facingRight ? 14 : 21;

  TmxTilesetTile *tileData = NULL;
  for (uint32_t i = 0; i < player->tileset.tilesLength; i++) {
    if (player->tileset.tiles[i].id == (uint32_t)baseTileId) {
      tileData = &player->tileset.tiles[i];
      break;
    }
  }

  int displayTileId = baseTileId;
  if (tileData && tileData->hasAnimation) {
    displayTileId = tileData->animation.frames[player->currentFrame].gid;
  }

  int col = displayTileId % player->tileset.columns;
  int row = displayTileId / player->tileset.columns;

  Rectangle source = {(float)(col * player->tileset.tileWidth),
                      (float)(row * player->tileset.tileHeight),
                      (float)player->tileset.tileWidth,
                      (float)player->tileset.tileHeight};

  Vector2 pos = {player->position.x - (float)player->tileset.tileWidth / 2.0f,
                 player->position.y - (float)player->tileset.tileHeight / 2.0f};
  DrawTextureRec(player->tileset.image.texture, source, pos, WHITE);
}
