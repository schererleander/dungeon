#include "raylib.h"
#define RAYTMX_IMPLEMENTATION
#include "raytmx.h"

#include "globals.h"
#include "entities.h"
#include "fairy.h"
#include "map.h"
#include "player.h"


int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_NAME);
  SetTargetFPS(TARGET_FPS);

  Entities entities = {0};
  Map map = LoadMap("assets/maps/debug.tmx", &entities);

  Marker *spawn = FindMarker(&map, "player_spawn");
  Vector2 spawnPos = spawn ? spawn->position : (Vector2){0, 0};


  RaytmxExternalTileset playerTileset = LoadTSX("assets/tilesets/elf.tsx");
  Player player = {.position = spawnPos,
                   .speed = 80.0f,
                   .bounds = {player.position.x, player.position.y, playerTileset.tileset.tileWidth - 4, playerTileset.tileset.tileHeight},
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
    UpdatePlayer(&player, &map, &entities);
		UpdateFairy(&fairy, player.position);
    camera.target = player.position;

    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode2D(camera);
    DrawMap(&map);
    DrawPickups(&entities, &map);
    DrawKeys(&entities, &map);
    DrawPlayer(&player);
    DrawFairy(&fairy);
    EndMode2D();
    EndDrawing();
  }

  UnloadMap(map);

  CloseWindow();
  return 0;
}
