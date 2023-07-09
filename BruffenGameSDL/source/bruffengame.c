#include "bruffengame.h"

SDL_Texture* loadTexture(const char* path) {
    SDL_Surface* tempSurface = SDL_LoadBMP(path);
    if (!tempSurface) {
        SDL_Log("Unable to load image %s! SDL Error: %s\n", path, SDL_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    if (!texture) {
        SDL_Log("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
    }
    return texture;
}

int calculateDistance(int x1, int y1, int x2, int y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

int checkCollision(SDL_Rect a, SDL_Rect b) {
    return !(a.x + a.w <= b.x || a.x >= b.x + b.w || a.y + a.h <= b.y || a.y >= b.y + b.h);
}

// This function sets the direction based on the side
void setDirection(int* directionX, int* directionY, int side, int diagonalChance) {
    switch (side) {
    case 0: // left side
        *directionX = 1;
        *directionY = diagonalChance == 0 ? (rand() % 2 == 0 ? -1 : 1) : 0;
        break;
    case 1: // right side
        *directionX = -1;
        *directionY = diagonalChance == 0 ? (rand() % 2 == 0 ? -1 : 1) : 0;
        break;
    case 2: // top side
        *directionY = 1;
        *directionX = diagonalChance == 0 ? (rand() % 2 == 0 ? -1 : 1) : 0;
        break;
    case 3: // bottom side
        *directionY = -1;
        *directionX = diagonalChance == 0 ? (rand() % 2 == 0 ? -1 : 1) : 0;
        break;
    }
}

void setVincPosition(int index, int side) {
    switch (side) {
    case 0: // left side
        vincRect[index].x = -SCREEN_MARGIN;
        vincRect[index].y = rand() % SCREEN_HEIGHT;
        break;
    case 1: // right side
        vincRect[index].x = SCREEN_WIDTH;
        vincRect[index].y = rand() % SCREEN_HEIGHT;
        break;
    case 2: // top side
        vincRect[index].x = rand() % SCREEN_WIDTH;
        vincRect[index].y = -SCREEN_MARGIN;
        break;
    case 3: // bottom side
        vincRect[index].x = rand() % SCREEN_WIDTH;
        vincRect[index].y = SCREEN_HEIGHT;
        break;
    }
    vincRect[index].w = VINC_WIDTH;
    vincRect[index].h = VINC_HEIGHT;
}

void resetVinc(int index) {
    float speedIncrement = ((float)score / SCORE_SPEED_FACTOR);
    speedIncrement = speedIncrement > 1.0 ? 1.0 : speedIncrement;

    int speedX = (rand() % VINC_SPEED + 1);
    int speedY = (rand() % VINC_SPEED + 1);

    int directionX = 0, directionY = 0;
    int side = rand() % 4;
    int diagonalChance = rand() % 5;

    setDirection(&directionX, &directionY, side, diagonalChance);

    int speedIncrementX = (int)(speedX * speedIncrement);
    int speedIncrementY = (int)(speedY * speedIncrement);

    int finalSpeedX = directionX * (speedX + (speedIncrementX < VINC_MIN_SPEED ? VINC_MIN_SPEED : speedIncrementX));
    int finalSpeedY = directionY * (speedY + (speedIncrementY < VINC_MIN_SPEED ? VINC_MIN_SPEED : speedIncrementY));

    vincSpeedX[index] = finalSpeedX;
    vincSpeedY[index] = finalSpeedY;

    setVincPosition(index, side);
}

void resetGame() {
    hotboiRect = (SDL_Rect){ (SCREEN_WIDTH - HOTBOI_WIDTH) / 2, (SCREEN_HEIGHT - HOTBOI_HEIGHT) / 2, HOTBOI_WIDTH, HOTBOI_HEIGHT };
    score = 0;
    currentVincCount = STARTING_VINC_COUNT;
    for (int i = 0; i < currentVincCount; i++) {
        resetVinc(i);
    }
}

void moveVincs() {
    for (int i = 0; i < currentVincCount; i++) {
        vincRect[i].x += vincSpeedX[i];
        vincRect[i].y += vincSpeedY[i];
        if (vincRect[i].x < -vincRect[i].w || vincRect[i].x > SCREEN_WIDTH || vincRect[i].y < -vincRect[i].h || vincRect[i].y > SCREEN_HEIGHT) {
            resetVinc(i);
        }
    }
}

SDL_Texture* renderText(const char* message, SDL_Color color, TTF_Font* font) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, message, color);
    if (!surface) {
        SDL_Log("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
    }
    SDL_FreeSurface(surface);
    return texture;
}

int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) { // Added SDL_INIT_AUDIO
        SDL_Log("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    // if no sound, we don't care, just ignore
    if (TTF_Init() == -1 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("SDL_ttf or SDL_mixer could not initialize! SDL_ttf Error: %s, SDL_mixer Error: %s\n", TTF_GetError(), Mix_GetError());
    }

    TTF_Font* font = TTF_OpenFont("arial.ttf", FONT_SIZE);
    if (font == NULL) {
        SDL_Log("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Ban Bruffen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    background = loadTexture("Background.bmp");
    hotboi = loadTexture("HotBoi.bmp");
    vincTexture = loadTexture("vinc.bmp");
    endScreen = loadTexture("EndScreen.bmp");
    Mix_Chunk* gameOverSound = Mix_LoadWAV("wowzer.wav");

    int prevScore = -1;
    SDL_Color yellow = { 255, 255, 102 };
    SDL_Rect textRect = { 10, 10, SCORE_TEXT_WIDTH, SCORE_TEXT_HEIGHT };
    int lastScoreIncrease = 0;

    if (!background || !hotboi || !vincTexture || !endScreen) {
        SDL_Log("Failed to load textures\n");
        return 1;
    }

    srand(time(NULL));
    resetGame();

    int hotboiSpeedX = 0;
    int hotboiSpeedY = 0;

    Uint32 startTime = SDL_GetTicks();
    score = 0;
    bool isGameOver = false;

    SDL_Event e;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    if (hotboiRect.y > 0) {
                        hotboiSpeedY = -10;
                    }
                    break;
                case SDLK_DOWN:
                    if (hotboiRect.y < SCREEN_HEIGHT - hotboiRect.h) {
                        hotboiSpeedY = 10;
                    }
                    break;
                case SDLK_LEFT:
                    if (hotboiRect.x > 0) {
                        hotboiSpeedX = -10;
                    }
                    break;
                case SDLK_RIGHT:
                    if (hotboiRect.x < SCREEN_WIDTH - hotboiRect.w) {
                        hotboiSpeedX = 10;
                    }
                    break;
                }
            }
            else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                case SDLK_DOWN:
                    hotboiSpeedY = 0;
                    break;
                case SDLK_LEFT:
                case SDLK_RIGHT:
                    hotboiSpeedX = 0;
                    break;
                case SDLK_r:
                    if (isGameOver) {
                        resetGame();
                        startTime = SDL_GetTicks();
                        isGameOver = false;
                    }
                    break;
                }
            }
        }

        if (!isGameOver) {
            score = (SDL_GetTicks() - startTime) / 1000;

            if (score != lastScoreIncrease) {
                if (score % ADD_VINCS_SECONDS_AMOUNT == 0) {
                    lastScoreIncrease = score;
                    if (currentVincCount < MAX_VINC_COUNT) {
                        resetVinc(currentVincCount);
                        currentVincCount++;
                    }
                }
            }

            hotboiRect.x += hotboiSpeedX;
            hotboiRect.y += hotboiSpeedY;

            if (hotboiRect.x < 0) {
                hotboiRect.x = 0;
            }
            else if (hotboiRect.x > SCREEN_WIDTH - hotboiRect.w) {
                hotboiRect.x = SCREEN_WIDTH - hotboiRect.w;
            }

            if (hotboiRect.y < 0) {
                hotboiRect.y = 0;
            }
            else if (hotboiRect.y > SCREEN_HEIGHT - hotboiRect.h) {
                hotboiRect.y = SCREEN_HEIGHT - hotboiRect.h;
            }

            moveVincs();

            for (int i = 0; i < currentVincCount; i++) {
                if (checkCollision(hotboiRect, vincRect[i])) {
                    isGameOver = true;
                    Mix_PlayChannel(-1, gameOverSound, 0);
                    break;
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, NULL);

        if (!isGameOver) {
            score = (SDL_GetTicks() - startTime) / 1000;
            char scoreStr[50];
            if (font == NULL) {
                SDL_Log("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
                return 1; // or handle error appropriately
            }

            sprintf_s(scoreStr, 50, "Score: %d", score); // specify buffer size in sprintf_s
            SDL_Texture* scoreTexture = renderText(scoreStr, yellow, font);
            if (scoreTexture == NULL) {
                SDL_Log("Failed to render text! SDL Error: %s\n", SDL_GetError());
                // handle error appropriately
            }

            // Render the hotboi
            SDL_RenderCopy(renderer, hotboi, NULL, &hotboiRect);

            // Render all the vincs
            for (int i = 0; i < currentVincCount; i++) {
                SDL_RenderCopy(renderer, vincTexture, NULL, &vincRect[i]);
            }

            // Render the score text
            SDL_RenderCopy(renderer, scoreTexture, NULL, &textRect);

            // Remember to destroy the score texture after rendering it.
            SDL_DestroyTexture(scoreTexture);

        }
        else
        {
            SDL_RenderCopy(renderer, endScreen, NULL, NULL);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(20);
    }

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(hotboi);
    SDL_DestroyTexture(vincTexture);
    SDL_DestroyTexture(endScreen);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    Mix_FreeChunk(gameOverSound);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();

    return 0;
}