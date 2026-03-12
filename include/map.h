#pragma once

#include "raylib.h"
#include "raytmx.h"

#include "marker.h"

#define MAX_MARKERS 25

typedef struct {
  TmxMap *map;
  TmxLayer *wallLayer;
  TmxLayer *groundLayer;
  TmxLayer *decorLayer;

	Marker markers[MAX_MARKERS];
	int markersCount;
} Map;

typedef struct Entities Entities;

Map LoadMap(const char *filename, Entities *entities);

bool isCollision(Map *map, Rectangle bounds);

void DrawMap(Map *map);
void DrawPickups(Entities *entities, Map *map);
void DrawKeys(Entities *entities, Map *map);

Marker *FindMarker(Map *map, const char *name);

void UnloadMap(Map map);
