#ifndef SNAKE_LIB_H
#define SNAKE_LIB_H

#include <stdint.h>  // for uint8_t, uint32_t

// ---------------------------------------------
//  CONSTANTS AND HARDWARE ADDRESSES
// ---------------------------------------------
#define VGA_BASE 0x08000000      // VGA base address
#define SCREEN_W 320
#define SCREEN_H 240
#define CELL_SIZE 3
#define MAP_W (SCREEN_W / CELL_SIZE)
#define MAP_H (SCREEN_H / CELL_SIZE)

#define DISPLAY_ADDRESS 0x04000050
#define TOGGLE_SWITCHES_ADDRESS 0x04000010
#define PUSH_BUTTON_ADDRESS 0x040000D0

// ---------------------------------------------
//  GLOBAL VARIABLES
// ---------------------------------------------
extern int apples;                    // number of apples on map
extern int applesEaten;               // score counter
extern unsigned char map[MAP_H][MAP_W]; // the 2D world grid

// ---------------------------------------------
//  GAME CORE FUNCTIONS
// ---------------------------------------------
void playGame(void);
void resetGame(void);
void drawMap(void);
void initializeMap(void);
void spawnApple(void);
void gameOver(void);

// ---------------------------------------------
//  INPUT / OUTPUT
// ---------------------------------------------
int get_sw(void);
int get_btn(void);
void handleInput(void);               // handle switch inputs
void set_displays(int display_number, char value);
void updateScoreDisplay(void);

// ---------------------------------------------
//  VGA GRAPHICS
// ---------------------------------------------
void drawPixel(int x, int y, uint32_t color);
void fillRect(int x0, int y0, int w, int h, uint32_t color);
void clearScreen(void);

// ---------------------------------------------
//  TEXT RENDERING
// ---------------------------------------------
void drawString5x7(int x, int y, const char *s, uint8_t color);
void intToStr(unsigned int v, char *buf);

// ---------------------------------------------
//  UTILITIES (from dtekv-lib)
// ---------------------------------------------
void delay(int);              // delay function (assembly)
void enable_interrupt(void);  // enable interrupts (assembly)

#endif
