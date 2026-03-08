#ifndef GAME_H
#define GAME_H

typedef enum GameScreen {
		SCREEN_TITLE,
		SCREEN_PLAYING,
		SCREEN_PAUSED,
		SCREEN_GAME_OVER
} GameScreen;

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define BACKGROUND_COLOR (Color){ 88, 68, 34, 255 }

#endif // GAME_H
