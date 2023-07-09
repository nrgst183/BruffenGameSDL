#pragma once

#include <SDL.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define VINC_COUNT 5
#define VINC_SPEED 5
#define HOTBOI_SPEED 10
#define HOTBOI_WIDTH 112
#define HOTBOI_HEIGHT 112
#define VINC_WIDTH 157
#define VINC_HEIGHT 149
#define SCREEN_MARGIN 150

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* background = NULL;
SDL_Texture* hotboi = NULL;
SDL_Texture* vincTexture = NULL;
SDL_Texture* endScreen = NULL;
SDL_Rect hotboiRect;
SDL_Rect vincRect[VINC_COUNT];
int vincSpeedX[VINC_COUNT];
int vincSpeedY[VINC_COUNT];

SDL_Texture* loadTexture(const char* path);
int checkCollision(SDL_Rect a, SDL_Rect b);
void moveVincs();
int calculateDistance(int x1, int y1, int x2, int y2);
