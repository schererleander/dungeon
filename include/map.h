#pragma once

#include "raylib.h"
#include "raytmx.h"

typedef struct {
  TmxMap *map;
  TmxLayer *wallLayer;
  TmxLayer *groundLayer;
  TmxLayer *decorLayer;
} Map;

typedef struct Entities Entities;

Map *LoadMap(const char *filename, Entities *entities);

bool isCollision(Map *map, Rectangle bounds);

void DrawMap(Map *map);
void DrawPickups(Entities *entities, Map *map);
void DrawKeys(Entities *entities, Map *map);

void UnloadMap(Map *map);
