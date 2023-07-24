#pragma once

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define HOTBOI_SPEED 500
#define HOTBOI_WIDTH 112
#define HOTBOI_HEIGHT 112
#define VINC_WIDTH 157
#define VINC_HEIGHT 149
#define VINC_MAX_SPEED 1000
#define VINC_MIN_SPEED 200
#define MAX_VINC_COUNT 100
#define STARTING_VINC_COUNT 3
#define ADD_VINCS_SECONDS_AMOUNT 10
#define MAX_VINC_SECONDS_AMOUNT 380
#define SCORE_SPEED_FACTOR 15
#define SCREEN_MARGIN 150
#define SCORE_X 10
#define SCORE_Y 10
#define SCORE_TEXT_WIDTH 150
#define SCORE_TEXT_HEIGHT 60
#define FONT_SIZE 128
#define NUM_SIDES 4
#define DIAGONAL_CHANCE_FACTOR 5

typedef struct {
    float x;
    float y;
    int width;
    int height;
    float speedX;
    float speedY;
    SDL_Texture* texture;
} Character;

enum GameState { RUNNING, PAUSED, SETTINGS, GAMEOVER };
enum GameState currentState;
enum Difficulty { EASY, MEDIUM, HARD };
enum Difficulty currentDifficulty;
bool vsyncOn;


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
Uint32 startTime;
Uint32 lastScoreIncrease;
Uint32 lastVincAdditionTime;
float deltaTime;
bool isInvulnerable = false;
bool isScreensaverMode = false;

SDL_Texture* loadTexture(const char* path);
int calculateDistance(int x1, int y1, int x2, int y2);
int checkCollision(SDL_Rect a, SDL_Rect b);
void setVincDirection(int* directionX, int* directionY, int side, int diagonalChance);
void setVincPosition(int index, int side);
void resetVinc(int index);
void resetGame();
void moveVincs();
SDL_Texture* renderText(const char* message, SDL_Color color, TTF_Font* font);
