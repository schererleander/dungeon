// src/main.c
#include "raylib.h"
#define RAYTMX_IMPLEMENTATION
#include "raytmx.h"

#include "game.h"

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dungeon Crawler");
  SetTargetFPS(60);

  GameScreen currentScreen = SCREEN_TITLE;

  TmxMap *map = LoadTMX("assets/maps/debug.tmx");
  if (map == NULL) {
    CloseWindow();
  }

  Camera2D camera;
  camera.target.x = (float)(map->width * map->tileWidth) / 2.0f;
  camera.target.y = (float)(map->height * map->tileHeight) / 2.0f;
  camera.offset.x = (float)SCREEN_WIDTH / 2.0f;
  camera.offset.y = (float)SCREEN_HEIGHT / 2.0f;
  camera.rotation = 0.0f;
  camera.zoom = 4.0f;

  const float panSpeed = 20.0f; /* Pixel per second */

  while (!WindowShouldClose()) {
    switch (currentScreen) {
    case SCREEN_TITLE:
      if (IsKeyPressed(KEY_ENTER))
        currentScreen = SCREEN_PLAYING;
      break;
    case SCREEN_PLAYING:
      if (IsKeyDown(KEY_UP))
        camera.target.y += panSpeed * GetFrameTime();
      if (IsKeyDown(KEY_DOWN))
        camera.target.y -= panSpeed * GetFrameTime();
      if (IsKeyDown(KEY_RIGHT))
        camera.target.x += panSpeed * GetFrameTime();
      if (IsKeyDown(KEY_LEFT))
        camera.target.x -= panSpeed * GetFrameTime();
      if (IsKeyDown(KEY_ESCAPE))
        currentScreen = SCREEN_PAUSED;
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
    ClearBackground(BLACK);
		

    switch (currentScreen) {
    case SCREEN_TITLE:
      DrawText("DUNGEON CRAWLER", 20, 20, 40, DARKGRAY);
      break;
		case SCREEN_PAUSED:
			DrawText("PAUSED", 20, 20, 40, DARKGRAY);
      break;
    case SCREEN_PLAYING:
      BeginMode2D(camera);
      {
				ClearBackground(BACKGROUND_COLOR);
        AnimateTMX(map);
        DrawTMX(map, &camera, NULL, 0, 0, WHITE);
      }
      EndMode2D();
			break;
    case SCREEN_GAME_OVER:
      DrawText("GAME OVER", 20, 20, 40, RED);
      break;
    }

    EndDrawing();
  }

	UnloadTMX(map);
  CloseWindow();
  return 0;
}
