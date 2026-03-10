// src/main.c
#include "raylib.h"
#define RAYTMX_IMPLEMENTATION
#include "entity.h"
#include "fairy.h"
#include "game.h"
#include "globals.h"
#include "map_manager.h"
#include "player.h"

bool debugMode = false;

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dungeon Crawler");
  SetTargetFPS(280);

  GameScreen currentScreen = SCREEN_TITLE;

  MapManager mapMgr = LoadGameMap("assets/maps/debug.tmx");

  EntityManager entityMgr = InitEntityManager(100);
  if (mapMgr.map) {
    SpawnEntitiesFromMap(&entityMgr, mapMgr.map);
  }

  RaytmxExternalTileset elfTS = LoadTSX("assets/tilesets/elf.tsx");
  Player player = {.position = {200, 200},
                   .speed = 120.0f,
                   .bounds = {204, 204, 12, 12},
                   .tileset = elfTS.tileset,
                   .currentFrame = 0,
                   .frameTime = 0.0f,
                   .state = 0, // idle
                   .facingRight = true};

  Fairy fairy = {.position = player.position, .targetOffset = {0}};

  Camera2D camera = {0};
  camera.offset.x = (float)SCREEN_WIDTH / 2.0f;
  camera.offset.y = (float)SCREEN_HEIGHT / 2.0f;
  camera.target = player.position;
  camera.zoom = 4.0f;

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_B))
      debugMode = !debugMode;
    switch (currentScreen) {
    case SCREEN_TITLE:
      if (IsKeyPressed(KEY_ENTER))
        currentScreen = SCREEN_PLAYING;
      break;
    case SCREEN_PLAYING:
      if (IsKeyDown(KEY_ESCAPE))
        currentScreen = SCREEN_PAUSED;
      UpdatePlayer(&player, &mapMgr);
			UpdateFairy(&fairy, player.position);
      UpdateEntities(&entityMgr, &player, &mapMgr);
      camera.target = player.position;
      break;
    case SCREEN_PAUSED:
      if (IsKeyDown(KEY_ESCAPE))
        currentScreen = SCREEN_PLAYING;
      break;
    case SCREEN_GAME_OVER:
      if (IsKeyDown(KEY_ENTER))
        currentScreen = SCREEN_PLAYING;
      break;
    };

    // --- DRAW ---
    BeginDrawing();

    switch (currentScreen) {
    case SCREEN_TITLE:
      ClearBackground(BLACK);
      DrawText("DUNGEON CRAWLER", 20, 20, 40, DARKGRAY);
      break;
    case SCREEN_PAUSED:
      ClearBackground(BLACK);
      DrawText("PAUSED", 20, 20, 40, DARKGRAY);
      break;
    case SCREEN_PLAYING:
      ClearBackground(BACKGROUND_COLOR);
      BeginMode2D(camera);
      {
        DrawMap(&mapMgr);
        DrawEntities(&entityMgr, mapMgr.map);
        DrawPlayer(&player);
				DrawFairy(&fairy);
      }
      EndMode2D();
      if (debugMode)
        DrawFPS(10, 10);
      break;
    case SCREEN_GAME_OVER:
      ClearBackground(BLACK);
      DrawText("GAME OVER", 20, 20, 40, RED);
      break;
    }

    EndDrawing();
  }

  UnloadGameMap(&mapMgr);
  UnloadEntityManager(&entityMgr);
  UnloadTexture(player.tileset.image.texture);
  CloseWindow();
  return 0;
}
