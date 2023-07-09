#pragma once

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define HOTBOI_SPEED 10
#define HOTBOI_WIDTH 112
#define HOTBOI_HEIGHT 112
#define VINC_WIDTH 157
#define VINC_HEIGHT 149
#define MAX_VINC_COUNT 15
#define STARTING_VINC_COUNT 3
#define ADD_VINCS_SECONDS_AMOUNT 5
#define MAX_VINC_SECONDS_AMOUNT 60
#define VINC_SPEED 3
#define VINC_SPEED_INCREMENT 0.5
#define VINC_MAX_SPEED 20
#define SCREEN_MARGIN 150
#define SCORE_X 10
#define SCORE_Y 10
#define SCORE_TEXT_WIDTH 100
#define SCORE_TEXT_HEIGHT 50
#define FONT_SIZE 64
#define SCORE_INCREMENT 10

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* background = NULL;
SDL_Texture* hotboi = NULL;
SDL_Texture* vincTexture = NULL;
SDL_Texture* endScreen = NULL;
SDL_Rect hotboiRect;
SDL_Rect vincRect[MAX_VINC_COUNT];
int vincSpeedX[MAX_VINC_COUNT];
int vincSpeedY[MAX_VINC_COUNT];
int currentVincCount;
int score;

SDL_Texture* loadTexture(const char* path);
SDL_Texture* renderText(const char* message, SDL_Color color, TTF_Font* font);
int calculateDistance(int x1, int y1, int x2, int y2);
int checkCollision(SDL_Rect a, SDL_Rect b);
void resetVinc(int index);
void resetGame();
void moveVincs();
int calculateDistance(int x1, int y1, int x2, int y2);
