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

void resetVinc(int index) {
    int side = rand() % 4;
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

    vincSpeedX[index] = (rand() % (2 * VINC_SPEED + 1)) - VINC_SPEED;
    vincSpeedY[index] = (rand() % (2 * VINC_SPEED + 1)) - VINC_SPEED;
}

void resetGame() {
    hotboiRect = (SDL_Rect){ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, HOTBOI_WIDTH, HOTBOI_HEIGHT };

    for (int i = 0; i < VINC_COUNT; i++) {
        resetVinc(i);
    }
}

void moveVincs() {
    for (int i = 0; i < VINC_COUNT; i++) {
        vincRect[i].x += vincSpeedX[i];
        vincRect[i].y += vincSpeedY[i];

        if (vincRect[i].x < -vincRect[i].w || vincRect[i].x > SCREEN_WIDTH || vincRect[i].y < -vincRect[i].h || vincRect[i].y > SCREEN_HEIGHT) {
            resetVinc(i);
        }
    }
}

int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Ban Bruffen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    background = loadTexture("Background.bmp");
    hotboi = loadTexture("HotBoi.bmp");
    vincTexture = loadTexture("vinc.bmp");
    endScreen = loadTexture("EndScreen.bmp");

    if (!background || !hotboi || !vincTexture || !endScreen) {
        SDL_Log("Failed to load textures\n");
        return 1;
    }

    srand(time(NULL));
    resetGame();

    int hotboiSpeedX = 0;
    int hotboiSpeedY = 0;

    Uint32 startTime = SDL_GetTicks();
    int score = 0;
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

            for (int i = 0; i < VINC_COUNT; i++) {
                if (checkCollision(hotboiRect, vincRect[i])) {
                    isGameOver = true;
                    break;
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, NULL);

        if (!isGameOver) {
            SDL_RenderCopy(renderer, hotboi, NULL, &hotboiRect);

            for (int i = 0; i < VINC_COUNT; i++) {
                SDL_RenderCopy(renderer, vincTexture, NULL, &vincRect[i]);
            }

            // Uncomment this when renderText function is implemented
            // renderText(score, 10, 10, renderer);
        }
        else {
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
    SDL_Quit();

    return 0;
}