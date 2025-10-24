#include "snake-lib.h"
#include <stdint.h>  // For uint8_t, uint32_t

// VGA memory base address
volatile uint8_t *const vga_base = (volatile uint8_t *)VGA_BASE;

// ------------------------------------------------------------
//                     BASIC VGA FUNCTIONS
// ------------------------------------------------------------

// Draw one pixel at (x, y) with a color (0x00–0xFF)
void drawPixel(int x, int y, uint32_t color)
{
    if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H)
        return;
    vga_base[y * SCREEN_W + x] = (uint8_t)color;
}

// Fill a rectangle area with color
void fillRect(int x0, int y0, int w, int h, uint32_t color)
{
    for (int y = y0; y < y0 + h; y++)
        for (int x = x0; x < x0 + w; x++)
            drawPixel(x, y, color);
}

// Clear entire screen (fill with black)
void clearScreen(void)
{
    for (int y = 0; y < SCREEN_H; ++y)
        for (int x = 0; x < SCREEN_W; ++x)
            drawPixel(x, y, 0x00);
}

// ------------------------------------------------------------
//                         TEXT DRAWING
// ------------------------------------------------------------
// Small 5x7 pixel font for simple text on VGA

static const char FONT_CHARS[] = "AEGMOPLRSV0123456789:";
static const unsigned char FONT_DATA[][5] = {
    /* A */ {0x7C, 0x12, 0x11, 0x12, 0x7C},
    /* E */ {0x7F, 0x49, 0x49, 0x49, 0x41},
    /* G */ {0x3E, 0x41, 0x49, 0x49, 0x3A},
    /* M */ {0x7F, 0x02, 0x0C, 0x02, 0x7F},
    /* O */ {0x3E, 0x41, 0x41, 0x41, 0x3E},
    /* P */ {0x7F, 0x09, 0x09, 0x09, 0x06},
    /* L */ {0x7F, 0x40, 0x40, 0x40, 0x40},
    /* R */ {0x7F, 0x09, 0x19, 0x29, 0x46},
    /* S */ {0x26, 0x49, 0x49, 0x49, 0x32},
    /* V */ {0x07, 0x38, 0x40, 0x38, 0x07},
    /* 0 */ {0x3E, 0x51, 0x49, 0x45, 0x3E},
    /* 1 */ {0x00, 0x42, 0x7F, 0x40, 0x00}, 
    /* 2 */ {0x42, 0x61, 0x51, 0x49, 0x46},
    /* 3 */ {0x21, 0x41, 0x45, 0x4B, 0x31},
    /* 4 */ {0x18, 0x14, 0x12, 0x7F, 0x10},
    /* 5 */ {0x27, 0x45, 0x45, 0x45, 0x39},
    /* 6 */ {0x3C, 0x4A, 0x49, 0x49, 0x30},
    /* 7 */ {0x01, 0x71, 0x09, 0x05, 0x03},
    /* 8 */ {0x36, 0x49, 0x49, 0x49, 0x36},
    /* 9 */ {0x06, 0x49, 0x49, 0x29, 0x1E},
    /* : */ {0x00, 0x36, 0x36, 0x00, 0x00},
};

// Return bitmap data for a given character
static inline const unsigned char *font_bitmap(char c)
{
    for (int i = 0; FONT_CHARS[i]; ++i)
        if (FONT_CHARS[i] == c)
            return FONT_DATA[i];
    return 0;
}

// Draw one 5x7 character at (x, y)
void drawChar5x7(int x, int y, char c, uint8_t color)
{
    const unsigned char *bm = font_bitmap(c);
    if (!bm) return;

    for (int col = 0; col < 5; ++col)
    {
        unsigned char bits = bm[col];
        for (int row = 0; row < 7; ++row)
        {
            if (bits & (1 << row))
            {
                if (x + col >= 0 && x + col < SCREEN_W &&
                    y + row >= 0 && y + row < SCREEN_H)
                {
                    vga_base[(y + row) * SCREEN_W + (x + col)] = color;
                }
            }
        }
    }
}

// Draw text string using 5x7 characters
void drawString5x7(int x, int y, const char *s, uint8_t color)
{
    while (*s)
    {
        drawChar5x7(x, y, *s, color);
        x += 6; // 5 pixels + 1 pixel space
        ++s;
    }
}

// Convert integer to ASCII string (for score)
void intToStr(unsigned int v, char *buf)
{
    char tmp[12];
    int i = 0, j = 0;

    if (v == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    while (v)
    {
        tmp[i++] = '0' + (v % 10);
        v /= 10;
    }
    while (i)
    {
        buf[j++] = tmp[--i];
    }
    buf[j] = '\0';
}

// ------------------------------------------------------------
//                    7-SEGMENT DISPLAY
// ------------------------------------------------------------

// Show one character on specific 7-segment display
void set_displays(int display_number, char value)
{
    volatile int* digit_segment[6] = {
        (volatile int*)0x04000050, (volatile int*)0x04000060,
        (volatile int*)0x04000070, (volatile int*)0x04000080,
        (volatile int*)0x04000090, (volatile int*)0x040000A0
    };

    // segment codes for 0–9 and letters F, I, N, S, H
    const int segment_codes[] = {
        0x40, 0xF9, 0x24, 0x30, 0x19, 0x12, 0x02, 0xF8,
        0x00, 0x10, 0x0E, 0xF9, 0x48, 0x12, 0x09, 0xFF
    };

    int code = 0xFF;
    if (value >= '0' && value <= '9') code = segment_codes[value - '0'];
    else if (value == 'F') code = segment_codes[10];
    else if (value == 'I') code = segment_codes[11];
    else if (value == 'N') code = segment_codes[12];
    else if (value == 'S') code = segment_codes[13];
    else if (value == 'H') code = segment_codes[14];
    else if (value == ' ') code = segment_codes[15];

    *digit_segment[display_number] = code;
}

// Update 7-segment displays to show current score
void updateScoreDisplay(void)
{
    int score = applesEaten;
    for (int i = 0; i <= 5; i++)
    {
        int digit = score % 10;
        set_displays(i, '0' + digit);
        score /= 10;
    }
}

// ------------------------------------------------------------
//                     GAME OVER SCREEN
// ------------------------------------------------------------

void gameOver(void)
{
    clearScreen(); // black background
    drawString5x7(100, 90, "GAME OVER", 0xFF);

    // show score
    char num[12];
    intToStr((unsigned)applesEaten, num);
    drawString5x7(100, 110, "APPLES:", 0xFF);
    drawString5x7(160, 110, num, 0xFF);

    // show "FINISH" on 7-segment displays
    const char word[] = "FINISH";
    for (int i = 0; i < 6; i++)
        set_displays((5 - i), word[i]);
}
