#include "entity.h"
#include "map_manager.h"
#include "player.h"
#include "raymath.h"
#include "globals.h"
#include <stdlib.h>
#include <string.h>

EntityManager InitEntityManager(uint32_t maxPerType) {
  EntityManager mgr;
  mgr.enemies = (Enemy *)calloc(maxPerType, sizeof(Enemy));
  mgr.collectibles = (Collectible *)calloc(maxPerType, sizeof(Collectible));
  mgr.movables = (Movable *)calloc(maxPerType, sizeof(Movable));
  mgr.statics = (Static *)calloc(maxPerType, sizeof(Static));

  mgr.enemiesCount = 0;
  mgr.collectiblesCount = 0;
  mgr.movablesCount = 0;
  mgr.staticsCount = 0;

  return mgr;
}

void SpawnEntitiesFromMap(EntityManager *mgr, TmxMap *map) {
  for (uint32_t i = 0; i < map->layersLength; i++) {
    TmxLayer *layer = &map->layers[i];

    // Handle Tile Layer Pickups
    if (layer->type == LAYER_TYPE_TILE_LAYER && 
       (strcmp(layer->name, "Pickups") == 0 || strcmp(layer->name, "Collectible") == 0)) {
      TmxTileLayer *tileLayer = &layer->exact.tileLayer;
      for (uint32_t y = 0; y < tileLayer->height; y++) {
        for (uint32_t x = 0; x < tileLayer->width; x++) {
          uint32_t gid = tileLayer->tiles[y * tileLayer->width + x];
          if (gid != 0) {
            float px = (float)x * map->tileWidth;
            float py = (float)y * map->tileHeight;
            
            // Default bounds (whole tile)
            Rectangle customBounds = { px, py, (float)map->tileWidth, (float)map->tileHeight };

            // Check if the tile has a custom collision shape from Tileset Editor
            if (gid < map->gidsToTilesLength) {
              TmxTile tileData = map->gidsToTiles[gid];
              
              // Use the first animation frame's collision if it exists
              uint32_t targetGid = gid;
              if (tileData.hasAnimation && tileData.animation.framesLength > 0) {
                targetGid = tileData.animation.frames[0].gid;
              }

              // Look up tile data for the frame (accounting for local vs global IDs)
              // Note: raytmx internal gidsToTiles uses global IDs
              if (targetGid < map->gidsToTilesLength) {
                TmxObjectGroup colGroup = map->gidsToTiles[targetGid].objectGroup;
                if (colGroup.objectsLength > 0) {
                  TmxObject colObj = colGroup.objects[0];
                  customBounds = (Rectangle){ 
                    px + (float)colObj.x, 
                    py + (float)colObj.y, 
                    (float)colObj.width, 
                    (float)colObj.height 
                  };
                }
              }
            }

            mgr->collectibles[mgr->collectiblesCount++] = (Collectible){
                .position = {px, py},
                .bounds = customBounds,
                .active = true,
                .gid = gid,
                .tileX = (int)x,
                .tileY = (int)y
            };
          }
        }
      }
    }

    if (layer->type != LAYER_TYPE_OBJECT_GROUP)
      continue;

    TmxObjectGroup group = layer->exact.objectGroup;

    if (strcmp(layer->name, "Static") == 0) {
      for (uint32_t j = 0; j < group.objectsLength; j++) {
        TmxObject object = group.objects[j];
        mgr->statics[mgr->staticsCount++] =
            (Static){.position = {(float)object.x, (float)object.y},
                     .bounds = {(float)object.x, (float)object.y,
                                (float)object.width, (float)object.height},
                     .active = true,
                     .gid = object.gid};
      }
    }
    if (strcmp(layer->name, "Movable") == 0) {
      for (uint32_t j = 0; j < group.objectsLength; j++) {
        TmxObject object = group.objects[j];
        mgr->movables[mgr->movablesCount++] =
            (Movable){.position = {(float)object.x, (float)object.y},
                      .bounds = {(float)object.x, (float)object.y,
                                 (float)object.width, (float)object.height},
                      .active = true,
                      .gid = object.gid};
      }
    }
    if (strcmp(layer->name, "Collectible") == 0) {
      for (uint32_t j = 0; j < group.objectsLength; j++) {
        TmxObject object = group.objects[j];
        mgr->collectibles[mgr->collectiblesCount++] =
            (Collectible){.position = {(float)object.x, (float)object.y},
                          .bounds = {(float)object.x, (float)object.y,
                                     (float)object.width, (float)object.height},
                          .active = true,
                          .gid = object.gid};
      }
    }
    if (strcmp(layer->name, "Enemy") == 0) {
      for (uint32_t j = 0; j < group.objectsLength; j++) {
        TmxObject object = group.objects[j];
        mgr->enemies[mgr->enemiesCount++] =
            (Enemy){.position = {(float)object.x, (float)object.y},
                    .bounds = {(float)object.x, (float)object.y,
                               (float)object.width, (float)object.height},
                    .active = true,
                    .gid = object.gid};
      }
    }
  }
}

void UpdateEntities(EntityManager *mgr, Player *player, MapManager *mapMgr) {
  // 1. Update Pickups (Collision only)
  for (int i = 0; i < mgr->collectiblesCount; i++) {
    if (mgr->collectibles[i].active &&
        CheckCollisionRecs(player->bounds, mgr->collectibles[i].bounds)) {
      mgr->collectibles[i].active = false;
      
      // Visually remove from map if it's a tile pickup
      if (mapMgr->pickupLayer && mapMgr->pickupLayer->type == LAYER_TYPE_TILE_LAYER) {
        TmxTileLayer *tileLayer = &mapMgr->pickupLayer->exact.tileLayer;
        int idx = mgr->collectibles[i].tileY * tileLayer->width + mgr->collectibles[i].tileX;
        if (idx >= 0 && idx < (int)tileLayer->tilesLength) {
          tileLayer->tiles[idx] = 0;
        }
      }
    }
  }

  // 2. Update Movables (Physics/Pushing)
  for (int i = 0; i < mgr->movablesCount; i++) {
    Movable *m = &mgr->movables[i];
    if (CheckCollisionRecs(player->bounds, m->bounds)) {
      // Push logic: move the block in player's direction if no wall is there
      Vector2 pushDir =
          Vector2Normalize(Vector2Subtract(m->position, player->position));
      Vector2 nextPos = Vector2Add(
          m->position, Vector2Scale(pushDir, player->speed * GetFrameTime()));
      Rectangle nextRect = {nextPos.x, nextPos.y, m->bounds.width,
                            m->bounds.height};

      if (!IsWallCollision(mapMgr, nextRect)) {
        m->position = nextPos;
        m->bounds.x = nextPos.x;
        m->bounds.y = nextPos.y;
      }
    }
  }

  // 3. Update Enemies (AI)
  for (int i = 0; i < mgr->enemiesCount; i++) {
    if (!mgr->enemies[i].active)
      continue;
    // Basic AI: Move toward player if within range
    if (Vector2Distance(mgr->enemies[i].position, player->position) < 100.0f) {
      Vector2 dir = Vector2Normalize(
          Vector2Subtract(player->position, mgr->enemies[i].position));
      mgr->enemies[i].position = Vector2Add(
          mgr->enemies[i].position, Vector2Scale(dir, 50.0f * GetFrameTime()));
      mgr->enemies[i].bounds.x = mgr->enemies[i].position.x;
      mgr->enemies[i].bounds.y = mgr->enemies[i].position.y;
    }
  }
}

void UnloadEntityManager(EntityManager *mgr) {
  if (mgr->enemies) free(mgr->enemies);
  if (mgr->collectibles) free(mgr->collectibles);
  if (mgr->movables) free(mgr->movables);
  if (mgr->statics) free(mgr->statics);
  
  mgr->enemies = NULL;
  mgr->collectibles = NULL;
  mgr->movables = NULL;
  mgr->statics = NULL;
  
  mgr->enemiesCount = 0;
  mgr->collectiblesCount = 0;
  mgr->movablesCount = 0;
  mgr->staticsCount = 0;
}
