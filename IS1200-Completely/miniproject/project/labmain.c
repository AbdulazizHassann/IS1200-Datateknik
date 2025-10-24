#define VGA_BASE 0x08000000
#define SCREEN_W 320
#define SCREEN_H 240
#define CELL_SIZE 3

#define MAP_W (SCREEN_W / CELL_SIZE)
#define MAP_H (SCREEN_H / CELL_SIZE)

#define LEDS_ADDRESS 0x04000000
#define DISPLAY_ADDRESS 0x04000050
#define TOGGLE_SWITCHES_ADDRESS 0x04000010
#define PUSH_BUTTON_ADDRESS 0x040000d0

extern void delay(int);
extern void enable_interrupt(void);

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
volatile u8 *const vga_base = (volatile u8 *)VGA_BASE;

int apples = 0;
int applesEaten = 0;

//----------USER INPUT-----------------
int get_sw(void)
{
  volatile int *switches = (volatile int *)TOGGLE_SWITCHES_ADDRESS;
  return *switches & 0x3FF; // mask with 0x3FF
}

int get_btn(void)
{
  volatile int *button = (volatile int *)PUSH_BUTTON_ADDRESS;
  return *button & 0x1;
}

//----------VGA OUTPUT-----------------
void drawPixel(int x, int y, u32 color)
{
  if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H)
    return;
  vga_base[y * SCREEN_W + x] = (u8)color;
}

void fillRect(int x0, int y0, int w, int h, u32 color)
{
  for (int y = y0; y < y0 + h; y++)
    for (int x = x0; x < x0 + w; x++)
      drawPixel(x, y, color);
}

void clearScreen()
{
  for (int y = 0; y < SCREEN_H; ++y)
    for (int x = 0; x < SCREEN_W; ++x)
      drawPixel(x, y, 0x00);
}

//----------TEXT-----------------
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
    /* V (sharper, not like U) */ {0x07, 0x38, 0x40, 0x38, 0x07},
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

static inline const unsigned char *font_bitmap(char c)
{
  for (int i = 0; FONT_CHARS[i]; ++i)
    if (FONT_CHARS[i] == c)
      return FONT_DATA[i];
  return 0;
}

static inline void drawChar5x7(int x, int y, char c, u8 color)
{
  const unsigned char *bm = font_bitmap(c);
  if (!bm)
    return;
  for (int col = 0; col < 5; ++col)
  {
    unsigned char bits = bm[col];
    for (int row = 0; row < 7; ++row)
    {
      if (bits & (1 << row))
      {
        if (x + col >= 0 && x + col < SCREEN_W && y + row >= 0 && y + row < SCREEN_H)
          vga_base[(y + row) * SCREEN_W + (x + col)] = color;
      }
    }
  }
}

static inline void drawString5x7(int x, int y, const char *s, u8 color)
{
  while (*s)
  {
    drawChar5x7(x, y, *s, color);
    x += 6; // 5px + 1px spacing
    ++s;
  }
}

static inline void intToStr(unsigned int v, char *buf)
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

void set_displays(int display_number, char value)
{
    volatile int *segm7;
    volatile int* digit_segment[6] = {
        (volatile int*)0x04000050,
        (volatile int*)0x04000060,
        (volatile int*)0x04000070,
        (volatile int*)0x04000080,
        (volatile int*)0x04000090,
        (volatile int*)0x040000A0
    };

    // 7-segment koder (0–9 + några bokstäver)
    const int segment_codes[] = {
        0x40, // 0
        0xF9, // 1
        0x24, // 2
        0x30, // 3
        0x19, // 4
        0x12, // 5
        0x02, // 6
        0xF8, // 7
        0x00, // 8
        0x10, // 9
        0x0E, // F
        0xF9, // I
        0x48, // N
        0x12, // S
        0x09, // H
        0xFF  // blank
    };

    int code = 0xFF; // tom som standard

    if (value >= '0' && value <= '9')
        code = segment_codes[value - '0'];
    else if (value == 'F') code = segment_codes[10];
    else if (value == 'I') code = segment_codes[11];
    else if (value == 'N') code = segment_codes[12];
    else if (value == 'S') code = segment_codes[13];
    else if (value == 'H') code = segment_codes[14];
    else if (value == ' ') code = segment_codes[15];

    segm7 = digit_segment[display_number];
    *segm7 = code;
}


// ----------GAME OVER----------
void gameOver()
{
  // simple black bg for readability
  for (int y = 0; y < SCREEN_H; y++)
    for (int x = 0; x < SCREEN_W; x++)
      vga_base[y * SCREEN_W + x] = 0x00;

  drawString5x7(100, 90, "GAME OVER", 0xFF);

  char num[12];
  intToStr((unsigned)applesEaten, num);
  drawString5x7(100, 110, "APPLES:", 0xFF);
  drawString5x7(160, 110, num, 0xFF);

  const char word[] = "FINISH";
   for (int i = 0; i < 6; i++) {
        set_displays((5 - i), word[i]);
    }
}

//----------WORLD-----------------
unsigned char map[MAP_H][MAP_W];

void initializeMap()
{
  for (int i = 0; i < MAP_H; i++)
    for (int j = 0; j < MAP_W; j++)
      map[i][j] = 0;
}

void drawMap()
{
  for (int i = 0; i < MAP_H; i++)
  {
    for (int j = 0; j < MAP_W; j++)
    {
      if (map[i][j] != 0)
      {
        fillRect(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, 0xFF);
      }
    }
  }
}

//----------SNAKE-----------------
int xPos = 10;
int yPos = 10;
int tailX = 1;
int tailY = 10;
char direction = 'r';
int snakeLength = 10;

char moveSnake()
{
  if (direction == 'r')
  {
    if (xPos + 1 >= MAP_W)
      return 1;
    else if (map[yPos][xPos + 1] == 'a')
    {
      map[yPos][xPos + 1] = direction;
      snakeLength++;
      apples--;
      applesEaten++;
      xPos++;
    }
    else if (map[yPos][xPos + 1] != 0)
      return 1;
    xPos++;
  }
  else if (direction == 'l')
  {
    if (xPos - 1 < 0)
      return 1;
    else if (map[yPos][xPos - 1] == 'a')
    {
      map[yPos][xPos - 1] = direction;
      snakeLength++;
      apples--;
      applesEaten++;
      xPos--;
    }
    else if (map[yPos][xPos - 1] != 0)
      return 1;
    xPos--;
  }
  else if (direction == 'u')
  {
    if (yPos - 1 < 0)
      return 1;
    else if (map[yPos - 1][xPos] == 'a')
    {
      map[yPos - 1][xPos] = direction;
      snakeLength++;
      apples--;
      applesEaten++;
      yPos--;
    }
    else if (map[yPos - 1][xPos] != 0)
      return 1;
    yPos--;
  }
  else if (direction == 'd')
  {
    if (yPos + 1 >= MAP_H)
      return 1;

    else if (map[yPos + 1][xPos] == 'a')
    {
      map[yPos + 1][xPos] = direction;
      snakeLength++;
      apples--;
      applesEaten++;
      yPos++;
    }
    else if (map[yPos + 1][xPos] != 0)
      return 1;
    yPos++;
  }

  char tailDir = map[tailY][tailX];
  map[tailY][tailX] = 0;
  if (tailDir == 'r')
    tailX++;
  else if (tailDir == 'l')
    tailX--;
  else if (tailDir == 'u')
    tailY--;
  else if (tailDir == 'd')
    tailY++;

  return 0;
}
// ritate snake function//
void rotateSnake(char turn)
{
  if (direction == 'r')
  {
    if (turn == 'l')
    {
      direction = 'u';
    }
    else if (turn == 'r')
    {
      direction = 'd';
    }
  }
  else if (direction == 'l')
  {
    if (turn == 'l')
    {
      direction = 'd';
    }
    else if (turn == 'r')
    {
      direction = 'u';
    }
  }
  else if (direction == 'u')
  {
    if (turn == 'l')
    {
      direction = 'l';
    }
    else if (turn == 'r')
    {
      direction = 'r';
    }
  }
  else if (direction == 'd')
  {
    if (turn == 'l')
    {
      direction = 'r';
    }
    else if (turn == 'r')
    {
      direction = 'l';
    }
  }
}

//----------APPLE-----------------
static unsigned seed = 1234567;
int rand()
{
  seed = seed * 1103515245 + 12345;
  return (seed >> 16) & 0x7FFF;
}

void spawnApple()
{
  if (snakeLength < apples + 10)
  {
    return;
  }
  while (1)
  {
    int appleX = rand() % MAP_W;
    int appleY = rand() % MAP_H;
    if (map[appleY][appleX] == 0)
    { // empty cell
      map[appleY][appleX] = 'a';
      apples++;
      break;
    }
  }
}



void playGame(void)
{
  char gameOverFlag = 0;

  // Initiera allt för nytt spel
  apples = 0;
  applesEaten = 0;
  xPos = 10;
  yPos = 10;
  tailX = 1;
  tailY = 10;
  direction = 'r';
  snakeLength = 10;

  initializeMap();
  for (int i = 0; i < 10; i++)
    map[yPos][xPos - i] = 'r';
  tailX = xPos - 9;

  // Spelloop
  while (1)
  {
    clearScreen();
    drawMap();

    int switches = get_sw();
    if (switches & 0x1)
    {
      while (switches & 0x1)
        switches = get_sw();
      rotateSnake('r');
    }
    else if (switches & 0x2)
    {
      while (switches & 0x2)
        switches = get_sw();
      rotateSnake('l');
    }

    map[yPos][xPos] = direction;
    gameOverFlag = moveSnake();
    spawnApple();
    delay(15);
    // Visa antalet äpplen (max 999999)
    int score = applesEaten;
    for (int i = 0; i <= 5; i++) {
        int digit = score % 10;
        set_displays(i, '0' + digit);
        score /= 10;
    }

    if (gameOverFlag)
    {
      delay(100);
      break;
    }
  }

  gameOver();
}

//----------MAIN-----------------

int main(void)
{
  while (1)
  {
    playGame(); // Kör spelet

    // Vänta på knapptryck för att starta om
    while (1)
    {
      int restart = get_btn();
      if (restart)
      {
        while (get_btn()) ; // vänta tills knappen släpps
        break; // starta om spelet
      }
    }
  }
}


//----------INTERRUPT HANDLER-----------------
/* Minimal interrupt handler */
void handle_interrupt(unsigned cause)
{
}
// if (cause == 17) // switches interrupt
// {
//   /* Switch interrupt */
//   volatile int *sw_base = (volatile int *)0x04000010;
//   volatile int *edgecapture = (volatile int *)(0x04000010 + 0xC); // offset 3*4
//   int edges = *edgecapture;

//   if (edges & 0x2)
//   {                     // SW1 is bit1
//     *edgecapture = 0x2; // acknowledge SW1 edge

//     for (volatile int i = 0; i < 8000; i++)
//     {
//       int dummy = *sw_base;
//       (void)dummy;
//     }

//     rotateSnake('l');
//   }
//   else if (edges & 0x1)
//   {                     // SW1 is bit1
//     *edgecapture = 0x2; // acknowledge SW1 edge

//     for (volatile int i = 0; i < 8000; i++)
//     {
//       int dummy = *sw_base;
//       (void)dummy;
//     }

//     moveSnake('r');
//   }
//   return;
// }
// if (cause == 16) // time interrupt
// {
// }