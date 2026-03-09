#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"
#include "raytmx.h"

#include "player.h"
#include "map_manager.h"

typedef struct Enemy {
  Vector2 position;
  Rectangle bounds;
  uint32_t gid;
  int health;
  bool active;
} Enemy;

typedef struct Static {
  Vector2 position;
  Rectangle bounds;
  uint32_t gid;
  bool active;
} Static;

typedef struct Collectible {
  Vector2 position;
  Rectangle bounds;
  uint32_t gid;
  bool active;
  int tileX;
  int tileY;
} Collectible;

typedef struct Movable {
  Vector2 position;
  Rectangle bounds;
  uint32_t gid;
  bool active;
} Movable;

typedef struct EntityManager {
  Enemy *enemies;
  int enemiesCount;

  Collectible *collectibles;
  int collectiblesCount;

  Static *statics;
  int staticsCount;

  Movable *movables;
  int movablesCount;
} EntityManager;

EntityManager InitEntityManager(uint32_t capacity);
void SpawnEntitiesFromMap(EntityManager *mgr, TmxMap *map);
void UpdateEntities(EntityManager *mgr, Player *player, MapManager *mapMgr);
void DrawEntities(EntityManager *mgr, TmxMap *map);
void UnloadEntityManager(EntityManager *mgr);

#endif // ENTITY_H
