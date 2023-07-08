#include <snes.h>


extern char sprites_pic, sprites_pic_end, sprites_pal, sprites_pal_end;
extern char bg_tiles, bg_tiles_end, bg_map, bg_map_end, bg_palette, bg_palette_end;

//Game Objects
t_objs *playerObj;
s16 *playerPosX, *playerPosY;
s16 vincsPosX[MAX_VINCS], vincsPosY[MAX_VINCS]; // Vinc positions
u8 gameOver; // Game over flag

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 224
#define HOTBOI_WIDTH 16
#define HOTBOI_HEIGHT 16
#define VINC_WIDTH 16
#define VINC_HEIGHT 16

#define MAX_VINCS 5 // The number of Vincs

s32 score;

u8 checkCollision(s16 x1, s16 y1, s16 x2, s16 y2, s16 width, s16 height) {
    return (x1 < x2 + width) && (x1 + width > x2) && (y1 < y2 + height) && (y1 + height > y2);
}

u8 checkCollision_asm(s16 x1, s16 y1, s16 x2, s16 y2, s16 width, s16 height) {
    u8 result;

    asm volatile(
        "rep #0x10          \n"   // Use 16-bit A and X/Y registers
        "lda %1             \n"   // A = x1
        "clc                \n"   // Clear carry flag for addition
        "adc %4             \n"   // A = x1 + width
        "cmp %2             \n"   // Compare A with x2
        "bcc no_collision%= \n"   // If (x1 + width) < x2, branch to no_collision
        "lda %2             \n"   // A = x2
        "adc %4             \n"   // A = x2 + width
        "cmp %1             \n"   // Compare A with x1
        "bcc no_collision%= \n"   // If (x2 + width) < x1, branch to no_collision
        "lda %3             \n"   // A = y1
        "adc %5             \n"   // A = y1 + height
        "cmp %3             \n"   // Compare A with y2
        "bcc no_collision%= \n"   // If (y1 + height) < y2, branch to no_collision
        "lda %3             \n"   // A = y2
        "adc %5             \n"   // A = y2 + height
        "cmp %3             \n"   // Compare A with y1
        "bcc no_collision%= \n"   // If (y2 + height) < y1, branch to no_collision
        "lda #1             \n"   // Set result to 1 (collision)
        "sta %0             \n"   // Store in result variable
        "bra end%=          \n"   // Branch to end
        "no_collision%=:    \n"   // Label for no collision
        "lda #0             \n"   // Set result to 0 (no collision)
        "sta %0             \n"   // Store in result variable
        "end%=:             \n"   // End label
        : "=m" (result)
        : "m" (x1), "m" (x2), "m" (y1), "m" (y2), "m" (width), "m" (height)
        : "cc", "memory"
    );

    return result;
}


void moveVincsRandomly() {
    for (int i = 0; i < MAX_VINCS; i++) {
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
    for (int i = 0; i < MAX_VINCS; i++) {
        vincsPosX[i] = (rand() % (SCREEN_WIDTH - VINC_WIDTH));
        vincsPosY[i] = (rand() % (SCREEN_HEIGHT - VINC_HEIGHT));
    }
}

// Function to initialize Bruffen game objects
void initGameObjects(u16 playerStartX, u16 playerStartY, u16 enemyStartX, u16 enemyStartY) {
    // Initialize player
    if (objNew(0, playerStartX, playerStartY) == 0) return; // no more space, get out
    
    objGetPointer(objgetid);
    playerObj = &objbuffers[objptr - 1];
    playerObj->width = 16; // assuming sprite is 16x16 pixels
    playerObj->height = 16;
    
    playerPosX = (u16 *)&(playerObj->xpos + 1);
    playerPosY = (u16 *)&(playerObj->ypos + 1);
    
    oambuffer[0].oamattribute = 0x60; // settings for sprite
    oambuffer[0].oamgraphics = &playerGfx; // your player graphics here

    // Initialize enemy
    if (objNew(1, enemyStartX, enemyStartY) == 0) return; // no more space, get out

    objGetPointer(objgetid);
    enemyObj = &objbuffers[objptr - 1];
    enemyObj->width = 16; // assuming sprite is 16x16 pixels
    enemyObj->height = 16;
    
    enemyPosX = (u16 *)&(enemyObj->xpos + 1);
    enemyPosY = (u16 *)&(enemyObj->ypos + 1);
    
    oambuffer[1].oamattribute = 0x60; // settings for sprite
    oambuffer[1].oamgraphics = &enemyGfx; // your enemy graphics here
    
    // Init Sprites palette (assuming you have palette data)
    setPalette(&playerPal, 128 + 0 * 16, 16 * 2);
    setPalette(&enemyPal, 128 + 1 * 16, 16 * 2);
}

// main function
int main(void) {
    // Initialize SNES console
    consoleInit();
    
    // Initialize text console with font
    // ... (same as in the sample code) ...

    // Initialize layer with tiles
    // ... (same as in the sample code) ...

    // Initialize sprite engine
    oamInitDynamicSprite(0x0000, 0x1000, 0, 0, OBJ_SIZE8_L16);

    // Object engine activation
    objInitEngine();

    // Initialize game objects
    initGameObjects(50, 50, 100, 100); // Position player at (50,50) and enemy at (100,100)

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
		for (int i = 0; i < MAX_VINCS; i++) {
			if (checkCollision_asm(*playerPosX, *playerPosY, vincsPosX[i], vincsPosY[i], 16, 16)) {
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
