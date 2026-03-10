#ifndef FAIRY_H
#define FAIRY_H

#include "raylib.h"


typedef struct Fairy {
	Vector2 position;
	Vector2 targetOffset;
	Vector2 targetWorld;

	float wandTimer;
} Fairy;

void UpdateFairy(Fairy *Fairy, Vector2 playerWorldPosition);
void DrawFairy(Fairy *Fairy);

#endif // FAIRY_H
