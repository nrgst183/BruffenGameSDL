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
#define MAX_VINC_COUNT 25
#define STARTING_VINC_COUNT 3
#define ADD_VINCS_SECONDS_AMOUNT 10
#define MAX_VINC_SECONDS_AMOUNT 120
#define VINC_SPEED 3
#define SCORE_SPEED_FACTOR 20
#define VINC_MAX_SPEED 100
#define VINC_MIN_SPEED 3
#define SCREEN_MARGIN 150
#define SCORE_X 10
#define SCORE_Y 10
#define SCORE_TEXT_WIDTH 125
#define SCORE_TEXT_HEIGHT 50
#define FONT_SIZE 128
#define SCORE_INCREMENT 10

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int speedX;
    int speedY;
    SDL_Texture* texture;
} Character;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* background;
SDL_Texture* endScreen;
SDL_Texture* vincTexture;
Mix_Chunk* gameOverSound;
TTF_Font* font;
Character hotboi;
Character vincs[MAX_VINC_COUNT];
int currentVincCount;
int score;
bool isGameOver;
Uint32 startTime;
Uint32 lastScoreIncrease;
Uint32 lastVincAdditionTime;

SDL_Texture* loadTexture(const char* path);
int calculateDistance(int x1, int y1, int x2, int y2);
int checkCollision(SDL_Rect a, SDL_Rect b);
void setVincDirection(int* directionX, int* directionY, int side, int diagonalChance);
void setVincPosition(int index, int side);
void resetVinc(int index);
void resetGame();
void moveVincs();
SDL_Texture* renderText(const char* message, SDL_Color color, TTF_Font* font);
