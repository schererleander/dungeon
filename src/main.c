#include "raylib.h"
#define RAYTMX_IMPLEMENTATION
#include "raytmx.h"

#include "entities.h"
#include "fairy.h"
#include "map.h"
#include "player.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define TARGET_FPS 60

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dungeon");
  SetTargetFPS(TARGET_FPS);

  Entities entities = {0};
  Map *map = LoadMap("assets/maps/debug.tmx", &entities);

  TraceLog(LOG_INFO, "Entities loaded:");
  TraceLog(LOG_INFO, "pickups: %d", entities.pickupsCount);
  TraceLog(LOG_INFO, "keys: %d", entities.keysCount);
  TraceLog(LOG_INFO, "bats: %d", entities.batsCount);
  TraceLog(LOG_INFO, "slimes: %d", entities.slimesCount);
  TraceLog(LOG_INFO, "flying skulls: %d", entities.flyingSkullsCount);
  TraceLog(LOG_INFO, "vases: %d", entities.vasesCount);
  TraceLog(LOG_INFO, "crates: %d", entities.cratesCount);

  RaytmxExternalTileset playerTileset = LoadTSX("assets/tilesets/elf.tsx");

  Player player = {.position = {192.0f, 192.0f},
                   .speed = 80.0f,
                   .bounds = {185.0f, 184.0f, 13.0f, 16.0f},
                   .state = PLAYER_IDLE,
                   .facingRight = true,
                   .tileset = playerTileset.tileset,
                   .health = 6,
                   .maxHealth = 6};

  Fairy fairy = {.position = {player.position.x, player.position.y}};

  Camera2D camera = {
      .offset = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f},
      .target = player.position,
      .rotation = 0.0f,
      .zoom = 3.0f,
  };

  while (!WindowShouldClose()) {
    UpdatePlayer(&player, map, &entities);
		UpdateFairy(&fairy, player.position);
    camera.target = player.position;

    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode2D(camera);
    DrawMap(map);
    DrawPickups(&entities, map);
    DrawKeys(&entities, map);
    DrawPlayer(&player);
    DrawFairy(&fairy);
    EndMode2D();
    EndDrawing();
  }

  UnloadMap(map);
  CloseWindow();
  return 0;
}
