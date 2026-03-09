#include "map_manager.h"
#include "entity.h"
#include "raytmx.h"
#include "globals.h"

// Forward declaration of internal raytmx function
void DrawTMXLayerTile(const TmxMap* map, Rectangle viewport, uint32_t rawGid, Rectangle destRect, Color tint);

MapManager LoadGameMap(const char *fileName) {
  MapManager mgr = {0};
  mgr.map = LoadTMX(fileName);

  // Cache layers for faster access
  if (mgr.map) {
    for (uint32_t i = 0; i < mgr.map->layersLength; i++) {
      if (strcmp(mgr.map->layers[i].name, "Walls") == 0 &&
          mgr.map->layers[i].type == LAYER_TYPE_TILE_LAYER) {
        mgr.wallLayer = &mgr.map->layers[i];
      }
      if (strcmp(mgr.map->layers[i].name, "Solid") == 0 &&
          mgr.map->layers[i].type == LAYER_TYPE_TILE_LAYER) {
        mgr.solidLayer = &mgr.map->layers[i];
      }
      if (strcmp(mgr.map->layers[i].name, "Collectible") == 0 || 
          strcmp(mgr.map->layers[i].name, "Pickups") == 0)
        mgr.pickupLayer = &mgr.map->layers[i];
    }
  }
  return mgr;
}

void UnloadGameMap(MapManager *mgr) {
  if (mgr->map) {
    UnloadTMX(mgr->map);
    mgr->map = NULL;
  }
}

bool IsWallCollision(MapManager *mgr, Rectangle rect) {
  // Check main walls
  if (mgr->wallLayer && CheckCollisionTMXTileLayersRec(mgr->map, mgr->wallLayer, 1, rect, NULL))
    return true;

  // Check additional solid layer (props, etc)
  if (mgr->solidLayer && CheckCollisionTMXTileLayersRec(mgr->map, mgr->solidLayer, 1, rect, NULL))
    return true;

  return false;
}

// Internal helper for debug rendering
static void DrawDebugTileCollision(TmxMap *map, TmxLayer *layer, Color color) {
  if (!layer || layer->type != LAYER_TYPE_TILE_LAYER) return;

  TmxTileLayer *tileLayer = &layer->exact.tileLayer;
  for (uint32_t y = 0; y < tileLayer->height; y++) {
    for (uint32_t x = 0; x < tileLayer->width; x++) {
      uint32_t gid = tileLayer->tiles[y * tileLayer->width + x];
      if (gid != 0 && gid < map->gidsToTilesLength) {
        TmxTile tileData = map->gidsToTiles[gid];
        for (uint32_t objIdx = 0; objIdx < tileData.objectGroup.objectsLength; objIdx++) {
          TmxObject obj = tileData.objectGroup.objects[objIdx];
          DrawRectangleLinesEx((Rectangle){ 
            (float)x * TILE_SIZE + (float)obj.x, 
            (float)y * TILE_SIZE + (float)obj.y, 
            (float)obj.width, 
            (float)obj.height 
          }, 0.5f, color);
        }
      }
    }
  }
}

void DrawMap(MapManager *mgr) {
  AnimateTMX(mgr->map);
  DrawTMX(mgr->map, NULL, NULL, 0, 0, WHITE);

  if (debugMode) {
    DrawDebugTileCollision(mgr->map, mgr->wallLayer, RED);
    DrawDebugTileCollision(mgr->map, mgr->solidLayer, RED);
  }
}

void DrawEntities(EntityManager *mgr, TmxMap *map) {
  Rectangle drawViewport = {-1000, -1000, 5000, 5000};

  for (int i = 0; i < mgr->collectiblesCount; i++) {
    if (mgr->collectibles[i].active) {
      // Pickups are drawn by the map renderer (DrawTMX).
      // We only draw the debug hitbox here.
      if (debugMode) {
        DrawRectangleLinesEx(mgr->collectibles[i].bounds, 0.5f, GREEN);
      }
    }
  }

  for (int i = 0; i < mgr->movablesCount; i++) {
    DrawTMXLayerTile(map, drawViewport, mgr->movables[i].gid,
                     mgr->movables[i].bounds, WHITE);
    if (debugMode) {
      DrawRectangleLinesEx(mgr->movables[i].bounds, 0.5f, GREEN);
    }
  }

  for (int i = 0; i < mgr->enemiesCount; i++) {
    if (mgr->enemies[i].active) {
      DrawTMXLayerTile(map, drawViewport, mgr->enemies[i].gid,
                       mgr->enemies[i].bounds, WHITE);
      if (debugMode) {
        DrawRectangleLinesEx(mgr->enemies[i].bounds, 0.5f, RED);
      }
    }
  }
}
