#include "bruffengame.h"

// Function to calculate the distance between two points
int calculateDistance(int x1, int y1, int x2, int y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

// Function to check if two characters collide
int checkCollision(Character a, Character b) {
    return !(a.x + a.width <= b.x || a.x >= b.x + b.width || a.y + a.height <= b.y || a.y >= b.y + b.height);
}

int clamp(int val, int min, int max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

SDL_Texture* createTextureFromSurface(SDL_Surface* surface) {
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        SDL_Log("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
    }
    return texture;
}

SDL_Texture* loadTexture(const char* path) {
    SDL_Surface* tempSurface = SDL_LoadBMP(path);
    if (!tempSurface) {
        SDL_Log("Unable to load image %s! SDL Error: %s\n", path, SDL_GetError());
        return NULL;
    }
    return createTextureFromSurface(tempSurface);
}

void toggleStateInGameOver(bool* state) {
    if (currentState == GAMEOVER) {
        *state = !(*state);
    }
}

void stopMovement(int* speed, int direction) {
    if (*speed == direction * HOTBOI_SPEED) {
        *speed = 0;
    }
}

// Function to set the direction based on the side
void setVincDirection(int* directionX, int* directionY, int side, int diagonalChance) {
    *directionX = 0;
    *directionY = 0;

    if (side % 2 == 0) // left or right
        *directionX = (side == 0) ? 1 : -1;
    else // top or bottom
        *directionY = (side == 2) ? 1 : -1;

    if (diagonalChance == 0) {
        if (side % 2 == 0) // left or right
            *directionY = (rand() % 2 == 0 ? -1 : 1);
        else // top or bottom
            *directionX = (rand() % 2 == 0 ? -1 : 1);
    }
}

// Function to set the position of Vinc
void setVincPosition(int index, int side) {
    vincs[index].x = (side % 2 == 0) ? // left or right
        (side == 0 ? -SCREEN_MARGIN : SCREEN_WIDTH) :
        rand() % SCREEN_WIDTH;

    vincs[index].y = (side % 2 == 0) ? // left or right
        rand() % SCREEN_HEIGHT :
        (side == 2 ? -SCREEN_MARGIN : SCREEN_HEIGHT);

    vincs[index].width = VINC_WIDTH;
    vincs[index].height = VINC_HEIGHT;
}


// Function to reset Vinc's position and speed
void resetVinc(int index) {

    float speedIncrement = ((float)score / SCORE_SPEED_FACTOR);

    int speedX = max(rand() % VINC_MIN_SPEED + 1, VINC_MIN_SPEED);
    int speedY = max(rand() % VINC_MIN_SPEED + 1, VINC_MIN_SPEED);

    int directionX = 0, directionY = 0;
    int side = rand() % NUM_SIDES;
    int diagonalChance = rand() % DIAGONAL_CHANCE_FACTOR;

    setVincDirection(&directionX, &directionY, side, diagonalChance);

    int speedIncrementX = (int)(speedX * speedIncrement);
    int speedIncrementY = (int)(speedY * speedIncrement);

    int finalSpeedX = directionX * (speedX + speedIncrementX);
    int finalSpeedY = directionY * (speedY + speedIncrementY);

    vincs[index].speedX = finalSpeedX;
    vincs[index].speedY = finalSpeedY;

    setVincPosition(index, side);
}

// Function to reset the game
void resetGame() {
    hotboi = (Character){ (SCREEN_WIDTH - HOTBOI_WIDTH) / 2, (SCREEN_HEIGHT - HOTBOI_HEIGHT) / 2, HOTBOI_WIDTH, HOTBOI_HEIGHT, 0, 0, hotboi.texture };
    score = 0;
    currentVincCount = STARTING_VINC_COUNT;
    for (int i = 0; i < MAX_VINC_COUNT; i++) {
        vincs[i].texture = vincTexture;
        resetVinc(i);
    }
}

// Function to move Vincs
void moveVincs() {
    for (int i = 0; i < currentVincCount; i++) {
        vincs[i].x += vincs[i].speedX * deltaTime;
        vincs[i].y += vincs[i].speedY * deltaTime;
        if (vincs[i].x < -vincs[i].width || vincs[i].x > SCREEN_WIDTH || vincs[i].y < -vincs[i].height || vincs[i].y > SCREEN_HEIGHT) {
            resetVinc(i);
        }
    }
}

// Function to render text
SDL_Texture* renderText(const char* message, SDL_Color color, TTF_Font* font) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, message, color);
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

void handleKeyboardInput(SDL_Event e, bool* quit, Uint32* startTime) {
    if (e.type == SDL_QUIT) {
        *quit = 1;
    }
    else if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:
            hotboi.speedY = -HOTBOI_SPEED;
            break;
        case SDLK_DOWN:
            hotboi.speedY = HOTBOI_SPEED;
            break;
        case SDLK_LEFT:
            hotboi.speedX = -HOTBOI_SPEED;
            break;
        case SDLK_RIGHT:
            hotboi.speedX = HOTBOI_SPEED;
            break;
        case SDLK_i:
            toggleStateInGameOver(&isInvulnerable);
            break;
        case SDLK_s:
            toggleStateInGameOver(&isScreensaverMode);
            break;
        }
    }
    else if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:
            stopMovement(&hotboi.speedY, -1);
            break;
        case SDLK_DOWN:
            stopMovement(&hotboi.speedY, 1);
            break;
        case SDLK_LEFT:
            stopMovement(&hotboi.speedX, -1);
            break;
        case SDLK_RIGHT:
            stopMovement(&hotboi.speedX, 1);
            break;
        case SDLK_ESCAPE:
            if (currentState == RUNNING) {
                currentState = PAUSED;
            }
            else if (currentState == PAUSED) {
                currentState = RUNNING;
            }
            else if (currentState == GAMEOVER) {
                resetGame();
                *startTime = SDL_GetTicks();
                currentState = RUNNING;
            }
            break;
        }
    }
}

// Function to update the game state
void updateGameState(Uint32* startTime, int* lastScoreIncrease) {
    if (currentState == RUNNING) {
        score = (SDL_GetTicks() - *startTime) / 1000;

        if (score != *lastScoreIncrease) {
            if (score % ADD_VINCS_SECONDS_AMOUNT == 0) {
                *lastScoreIncrease = score;
                if (currentVincCount < MAX_VINC_COUNT) {
                    resetVinc(currentVincCount);
                    currentVincCount++;
                }
            }
        }

        hotboi.x += hotboi.speedX * deltaTime;
        hotboi.y += hotboi.speedY * deltaTime;

        hotboi.x = clamp(hotboi.x, 0, SCREEN_WIDTH - hotboi.width);
        hotboi.y = clamp(hotboi.y, 0, SCREEN_HEIGHT - hotboi.height);

        moveVincs();

        for (int i = 0; i < currentVincCount; i++) {
            if (!isInvulnerable && !isScreensaverMode && checkCollision(hotboi, vincs[i])) {
                currentState = GAMEOVER;
                Mix_PlayChannel(-1, gameOverSound, 0);
                break;
            }
        }
    }
}

// Function to render the game
void renderGame(TTF_Font* font, SDL_Color white, SDL_Rect textRect) {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background, NULL, NULL);

    if (currentState == RUNNING) 
    {
        score = (SDL_GetTicks() - startTime) / 1000;
        char scoreStr[50];
        if (font == NULL) {
            SDL_Log("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
            return 1; // or handle error appropriately
        }

        sprintf_s(scoreStr, 50, "Score: %d", score); // specify buffer size in sprintf_s
        SDL_Texture* scoreTexture = renderText(scoreStr, white, font);
        if (scoreTexture == NULL) {
            SDL_Log("Failed to render text! SDL Error: %s\n", SDL_GetError());
            // handle error appropriately
        }

        if (!isScreensaverMode) {
            // Render the hotboi
            SDL_Rect hotboiRect = { hotboi.x, hotboi.y, hotboi.width, hotboi.height };
            SDL_RenderCopy(renderer, hotboi.texture, NULL, &hotboiRect);
        }

        // Render all the vincs
        for (int i = 0; i < currentVincCount; i++) {
            SDL_Rect vincRect = { (int)vincs[i].x, (int)vincs[i].y, vincs[i].width, vincs[i].height };
            SDL_RenderCopy(renderer, vincs[i].texture, NULL, &vincRect);
        }

        // Render the score text
        SDL_RenderCopy(renderer, scoreTexture, NULL, &textRect);

        // Remember to destroy the score texture after rendering it.
        SDL_DestroyTexture(scoreTexture);

    }
    else if (currentState == PAUSED)
    {
        SDL_Texture* pauseText = renderText("PAUSE", white, font);
        SDL_Rect textRect = { 20, 20, SCORE_TEXT_WIDTH, SCORE_TEXT_HEIGHT };
        SDL_RenderCopy(renderer, pauseText, NULL, &textRect);
        SDL_DestroyTexture(pauseText);
    }
    else if (currentState == GAMEOVER) 
    {
        score = 0;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Rect endScreenRect;
        int endScreenWidth, endScreenHeight;
        SDL_QueryTexture(endScreen, NULL, NULL, &endScreenWidth, &endScreenHeight);

        // Calculate the scale factor
        float scaleW = (float)SCREEN_WIDTH / endScreenWidth;
        float scaleH = (float)SCREEN_HEIGHT / endScreenHeight;
        float scale = (scaleW < scaleH) ? scaleW : scaleH; // Take the smaller scale factor

        // Scale the dimensions
        endScreenRect.w = (int)(endScreenWidth * scale);
        endScreenRect.h = (int)(endScreenHeight * scale);

        // Center the image
        endScreenRect.x = (SCREEN_WIDTH - endScreenRect.w) / 2;
        endScreenRect.y = (SCREEN_HEIGHT - endScreenRect.h) / 2;

        SDL_RenderCopy(renderer, endScreen, NULL, &endScreenRect);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(1);
}

// Main function
int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        SDL_Log("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("SDL_ttf or SDL_mixer could not initialize! SDL_ttf Error: %s, SDL_mixer Error: %s\n", TTF_GetError(), Mix_GetError());
    }

    TTF_Font* font = TTF_OpenFont("arial.ttf", FONT_SIZE);
    if (font == NULL) {
        SDL_Log("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Ban Bruffen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    background = loadTexture("Background.bmp");
    hotboi.texture = loadTexture("HotBoi.bmp");
    vincTexture = loadTexture("vinc.bmp");
    endScreen = loadTexture("EndScreen.bmp");
    gameOverSound = Mix_LoadWAV("wowzer.wav");

    int prevScore = -1;
    SDL_Color white = { 255, 255, 255 };
    SDL_Rect textRect = { 40, 40, SCORE_TEXT_WIDTH, SCORE_TEXT_HEIGHT };

    if (!background || !hotboi.texture || !vincTexture || !endScreen) {
        SDL_Log("Failed to load textures\n");
        return 1;
    }

    srand(time(NULL));
    resetGame();

    startTime = SDL_GetTicks();
    score = 0;
    currentState = RUNNING;

    SDL_Event e;
    bool quit = false;

    // Main game loop
    while (!quit) {

        Uint32 frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&e) != 0) {
            handleKeyboardInput(e, &quit, &startTime);
        }

        updateGameState(&startTime, &lastScoreIncrease);
        renderGame(font, white, textRect);

        Uint32 frameEnd = SDL_GetTicks();
        deltaTime = (frameEnd - frameStart) / 1000.0f; // duration of the last frame in seconds
    }

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(hotboi.texture);
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

