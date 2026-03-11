#pragma once

#include "raylib.h"

typedef struct {
  Vector2 position;
  Vector2 targetOffset;
  Vector2 targetWorld;

  float wandTimer;
} Fairy;

void UpdateFairy(Fairy *fairy, Vector2 playerWorldPosition);
void DrawFairy(Fairy *fairy);
