#include "raylib.h"
#include "raymath.h"
#include <math.h>

#include "fairy.h"
#include "globals.h"

#define FAIRY_WAND_RADIUS 20
#define FAIRY_STAY_MIN 1
#define FAIRY_STAY_MAX 3
#define FAIRY_SMOOTHNESS 0.25f

#define FAIRY_BOB_SPEED 4
#define FAIRY_BOB_HEIGHT 5

void UpdateFairy(Fairy *fairy, Vector2 playerWorldPosiion) {
  float deltaTime = GetFrameTime();

  fairy->wandTimer -= deltaTime;
  if (fairy->wandTimer <= 0) {
    fairy->targetOffset =
        (Vector2){GetRandomValue(-FAIRY_WAND_RADIUS, FAIRY_WAND_RADIUS),
                  GetRandomValue(-FAIRY_WAND_RADIUS, FAIRY_WAND_RADIUS)};
    fairy->wandTimer = GetRandomValue(FAIRY_STAY_MIN, FAIRY_STAY_MAX);
  }

  fairy->targetWorld = Vector2Add(playerWorldPosiion, fairy->targetOffset);

  fairy->position = Vector2Lerp(fairy->position, fairy->targetWorld,
                                FAIRY_SMOOTHNESS * deltaTime);
}

void DrawFairy(Fairy *fairy) {
  float bob = sin(GetTime() * FAIRY_BOB_SPEED) * FAIRY_BOB_HEIGHT;
  int x = (int)fairy->position.x;
  int y = (int)(fairy->position.y + bob);

  if (debugMode) {
    DrawCircleV(fairy->targetWorld, 1, RED);
    DrawCircleV(fairy->position, 1, GREEN);
  }

  // Outer cross glow
  DrawRectangle(x, y - 3, 1, 7, (Color){140, 180, 255, 40});
  DrawRectangle(x - 3, y, 7, 1, (Color){140, 180, 255, 40});
  // Mid cross
  DrawRectangle(x, y - 1, 1, 3, (Color){190, 215, 255, 120});
  DrawRectangle(x - 1, y, 3, 1, (Color){190, 215, 255, 120});
  // Bright core pixel
  DrawRectangle(x, y, 1, 1, (Color){255, 255, 255, 255});
}
