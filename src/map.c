#include "map.h"
#include "entities.h"
#include "raylib.h"

#define RAYTMX_IMPLEMENTATION
#include "raytmx.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static TmxLayer *FindLayerByName(TmxMap *map, const char *name) {
  for (uint32_t i = 0; i < map->layersLength; i++) {
    if (strcmp(map->layers[i].name, name) == 0)
      return &map->layers[i];
  }
  return NULL;
}

static TmxTileset *FindTileSetByGid(TmxMap *map, int gid) {
  for (uint32_t i = 0; i < map->tilesetsLength; i++) {
    if (map->tilesets[i].firstGid <= (uint32_t)gid &&
        (uint32_t)gid <= map->tilesets[i].lastGid)
      return &map->tilesets[i];
  }
  return NULL;
}

static PickupType GetPickupTypeByTileId(int tileId) {
  int count = sizeof(PICKUP_TILE_ID) / sizeof(PICKUP_TILE_ID[0]);
  for (int i = 0; i < count; i++) {
    if (PICKUP_TILE_ID[i] == tileId)
      return (PickupType)i;
  }
  return PICKUP_COIN;
}

static Rectangle GetTileBounds(TmxTileset *tileset, uint32_t tileId, float worldX,
                               float worldY) {
  for (uint32_t i = 0; i < tileset->tilesLength; i++) {
    if (tileset->tiles[i].id == tileId) {
      TmxObjectGroup *og = &tileset->tiles[i].objectGroup;
      if (og->objectsLength > 0) {
        TmxObject *obj = &og->objects[0];
        return (Rectangle){
            worldX + (float)obj->x,
            worldY + (float)obj->y,
            (float)obj->width,
            (float)obj->height,
        };
      }
    }
  }
  return (Rectangle){worldX, worldY, (float)tileset->tileWidth,
                     (float)tileset->tileHeight};
}

static void LoadPickups(TmxMap *tmxMap, TmxLayer *layer, Entities *entities) {
  TmxObjectGroup *group = &layer->exact.objectGroup;
  for (uint32_t i = 0; i < group->objectsLength; i++) {
    TmxObject *object = &group->objects[i];
    if (object->type != OBJECT_TYPE_TILE ||
        entities->pickupsCount >= MAX_ENTITIES)
      continue;

    TmxTileset *tileset = FindTileSetByGid(tmxMap, object->gid);
    assert(tileset);

    uint32_t tileId = object->gid - tileset->firstGid;
    float x = (float)object->x;
    float y = (float)object->y - (float)tileset->tileHeight;

    Pickup *p = &entities->pickups[entities->pickupsCount++];
    p->type = GetPickupTypeByTileId((int)tileId);
    p->position = (Vector2){x, y};
    p->bounds = GetTileBounds(tileset, tileId, x, y);
    p->value = 1;
    p->active = true;
    p->gid = (uint32_t)object->gid;
  }
}

static void LoadKeys(TmxMap *tmxMap, TmxLayer *layer, Entities *entities) {
  TmxObjectGroup *group = &layer->exact.objectGroup;
  for (uint32_t i = 0; i < group->objectsLength; i++) {
    TmxObject *object = &group->objects[i];
    if (object->type != OBJECT_TYPE_TILE || entities->keysCount >= MAX_ENTITIES)
      continue;

    TmxTileset *tileset = FindTileSetByGid(tmxMap, object->gid);
    assert(tileset);

    uint32_t tileId = object->gid - tileset->firstGid;
    float x = (float)object->x;
    float y = (float)object->y - (float)tileset->tileHeight;

    Key *k = &entities->keys[entities->keysCount++];
    k->position = (Vector2){x, y};
    k->bounds = GetTileBounds(tileset, tileId, x, y);
    k->uniqueId = (int)object->id;
    k->active = true;
    k->gid = (uint32_t)object->gid;
  }
}

Map *LoadMap(const char *filename, Entities *entities) {
  Map *map = (Map *)malloc(sizeof(Map));
  if (!map)
    return NULL;

  map->map = LoadTMX(filename);
  if (!map->map) {
    free(map);
    return NULL;
  }

  map->wallLayer = FindLayerByName(map->map, "Walls");
  map->groundLayer = FindLayerByName(map->map, "Ground");
  map->decorLayer = FindLayerByName(map->map, "Decor");

  if (entities) {
    TmxLayer *pickupsLayer = FindLayerByName(map->map, "Pickups");
    if (pickupsLayer)
      LoadPickups(map->map, pickupsLayer, entities);

    TmxLayer *itemsLayer = FindLayerByName(map->map, "Items");
    if (itemsLayer)
      LoadKeys(map->map, itemsLayer, entities);
  }

  return map;
}

bool isCollision(Map *map, Rectangle bounds) {
  if (!map || !map->wallLayer || !map->decorLayer)
    return false;
  return CheckCollisionTMXTileLayersRec(map->map, map->wallLayer, 1, bounds,
                                        NULL) ||
         CheckCollisionTMXTileLayersRec(map->map, map->decorLayer, 1, bounds,
                                        NULL);
}

void DrawMap(Map *map) {
  if (!map)
    return;
  AnimateTMX(map->map);

  TmxLayer layers[3];
  uint32_t count = 0;
  if (map->groundLayer) layers[count++] = *map->groundLayer;
  if (map->wallLayer)   layers[count++] = *map->wallLayer;
  if (map->decorLayer)  layers[count++] = *map->decorLayer;
  DrawTMXLayers(map->map, NULL, NULL, layers, count, 0, 0, WHITE);
}

static void DrawTileByGid(TmxMap *tmxMap, uint32_t gid, float x, float y) {
  if (gid == 0 || gid >= tmxMap->gidsToTilesLength)
    return;
  TmxTile tile = tmxMap->gidsToTiles[gid];
  if (tile.gid == 0)
    return;
  if (tile.hasAnimation && tile.frameIndex < tile.animation.framesLength) {
    uint32_t frameGid = tile.animation.frames[tile.frameIndex].gid;
    if (frameGid < tmxMap->gidsToTilesLength)
      tile = tmxMap->gidsToTiles[frameGid];
  }
  DrawTextureRec(tile.texture, tile.sourceRect, (Vector2){x, y}, WHITE);
}

void DrawPickups(Entities *entities, Map *map) {
  if (!entities || !map)
    return;
  for (int i = 0; i < entities->pickupsCount; i++) {
    Pickup *p = &entities->pickups[i];
    if (!p->active)
      continue;
    DrawTileByGid(map->map, p->gid, p->position.x, p->position.y);
  }
}

void DrawKeys(Entities *entities, Map *map) {
  if (!entities || !map)
    return;
  for (int i = 0; i < entities->keysCount; i++) {
    Key *k = &entities->keys[i];
    if (!k->active)
      continue;
    DrawTileByGid(map->map, k->gid, k->position.x, k->position.y);
  }
}

void UnloadMap(Map *map) {
  if (!map)
    return;
  UnloadTMX(map->map);
  free(map);
}
