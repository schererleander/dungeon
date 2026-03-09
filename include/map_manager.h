#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H

#include "raytmx.h"

typedef struct MapManager {
  TmxMap *map;
  TmxLayer *wallLayer;
  TmxLayer *solidLayer;
  TmxLayer *pickupLayer;
} MapManager;

MapManager LoadGameMap(const char *fileName);
void UnloadGameMap(MapManager *mgr);
bool IsWallCollision(MapManager *mgr, Rectangle rect);
void DrawMap(MapManager *mgr);

#endif
