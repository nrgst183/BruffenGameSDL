#include <snes.h>

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 224
#define HOTBOI_WIDTH 16
#define HOTBOI_HEIGHT 16
#define VINC_WIDTH 16
#define VINC_HEIGHT 16

#define MAX_VINCS 10 // The number of Vincs

extern unsigned char hotboiGfx[], hotboiPal[], vincGfx[], vincPal[];

//Game Objects
t_objs *playerObj;
s16 *playerPosX, *playerPosY;
s16 vincsPosX[MAX_VINCS], vincsPosY[MAX_VINCS]; // Vinc positions
u8 gameOver; // Game over flag

s32 score;

u8 checkCollision(s16 x1, s16 y1, s16 x2, s16 y2, s16 width, s16 height) {
    return (x1 < x2 + width) && (x1 + width > x2) && (y1 < y2 + height) && (y1 + height > y2);
}

void updateGameOverScreen() {
    // Code to update the game over screen
    // This will depend on how you want to display the game over screen
}

void moveVincsRandomly() {
    int i;
    for (i = 0; i < MAX_VINCS; i++) {
        s16 dx = (rand() % 3) - 1; // random value between -1 and 1
        s16 dy = (rand() % 3) - 1;
        vincsPosX[i] += dx;
        vincsPosY[i] += dy;

        // If Vinc goes out of the screen, reposition it to a new random position
        if (vincsPosX[i] < 0 || vincsPosX[i] > SCREEN_WIDTH || vincsPosY[i] < 0 || vincsPosY[i] > SCREEN_HEIGHT) {
            vincsPosX[i] = (rand() % (SCREEN_WIDTH - VINC_WIDTH));
            vincsPosY[i] = (rand() % (SCREEN_HEIGHT - VINC_HEIGHT));
        }
    }
}

void resetGame() {
    gameOver = 0;
    score = 0;
    *playerPosX = SCREEN_WIDTH / 2;
    *playerPosY = SCREEN_HEIGHT / 2;
    int i;
    for (i = 0; i < MAX_VINCS; i++) {
        vincsPosX[i] = (rand() % (SCREEN_WIDTH - VINC_WIDTH));
        vincsPosY[i] = (rand() % (SCREEN_HEIGHT - VINC_HEIGHT));
    }
}

// Function to initialize Bruffen game objects
void initGameObjects(u16 playerStartX, u16 playerStartY) {
    // Initialize player
    if (objNew(0, playerStartX, playerStartY) == 0) return; // no more space, get out
    
    objGetPointer(objgetid);
    playerObj = &objbuffers[objptr - 1];
    playerObj->width = HOTBOI_WIDTH;
    playerObj->height = HOTBOI_HEIGHT;
    
    playerPosX = (u16 *)&(playerObj->xpos + 1);
    playerPosY = (u16 *)&(playerObj->ypos + 1);
    
    oambuffer[0].oamattribute = 0x60; // settings for sprite
    oambuffer[0].oamgraphics = hotboiGfx; // your player graphics here

    // Initialize enemy
    int i;
    for (i = 0; i < MAX_VINCS; i++) {
        if (objNew(1, vincsPosX[i], vincsPosY[i]) == 0) return; // no more space, get out

        objGetPointer(objgetid);
        t_objs *vincObj = &objbuffers[objptr - 1];
        vincObj->width = VINC_WIDTH;
        vincObj->height = VINC_HEIGHT;
        
        oambuffer[i + 1].oamattribute = 0x60; // settings for sprite
        oambuffer[i + 1].oamgraphics = vincGfx; // your enemy graphics here
    }
    
    // Init Sprites palette (assuming you have palette data)
    setPalette(hotboiPal, 128 + 0 * 16, 16 * 2);
    setPalette(vincPal, 128 + 1 * 16, 16 * 2);
}

// main function
int main(void) {
    // Initialize SNES console
    consoleInit();

    // Initialize sprite engine
    oamInitDynamicSprite(0x0000, 0x1000, 0, 0, OBJ_SIZE8_L16);

    // Object engine activation
    objInitEngine();

    // Initialize game objects
    initGameObjects(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2); // Position player at center of screen

    // Now set mode and disable BG3
    setMode(BG_MODE1, 0);
    bgSetDisable(2);

    // Turn screen on
    setScreenOn();

    while (1) {
        // Check Game Over
        if (gameOver) {
            updateGameOverScreen();

            u16 pad0 = padsCurrent(0);
            if (pad0 & KEY_START) { // Press Start button to reset the game
                resetGame();
                continue;
            }
        }

        // Handle D-pad inputs for HotBoi movement
        u16 pad0 = padsCurrent(0);
        if (pad0) {
            if ((pad0 & KEY_RIGHT) && *playerPosX < SCREEN_WIDTH - HOTBOI_WIDTH) *playerPosX += 1;
            if ((pad0 & KEY_LEFT) && *playerPosX > 0) *playerPosX -= 1;
            if ((pad0 & KEY_UP) && *playerPosY > 0) *playerPosY -= 1;
            if ((pad0 & KEY_DOWN) && *playerPosY < SCREEN_HEIGHT - HOTBOI_HEIGHT) *playerPosY += 1;
        }

        // Move Vincs randomly
        moveVincsRandomly();

        // Check for collision between HotBoi and Vincs
        int i;
        for (i = 0; i < MAX_VINCS; i++) {
            if (checkCollision(*playerPosX, *playerPosY, vincsPosX[i], vincsPosY[i], HOTBOI_WIDTH, HOTBOI_HEIGHT)) {
                // Collision detected, set game over flag
                gameOver = 1;
                break;
            }
        }

        // Increase the score
        score++;

        // Prepare next frame and wait for VBlank
        WaitForVBlank();
    }

    return 0;
}

