#include "snake-lib.h"

extern void delay(int);
extern void enable_interrupt(void);

int apples = 0;
int applesEaten = 0;

//----------USER INPUT-----------------
int get_sw(void)
{
  volatile int *switches = (volatile int *)TOGGLE_SWITCHES_ADDRESS;
  return *switches & 0x3; // mask only SW0 and SW1
}

int get_btn(void)
{
  volatile int *button = (volatile int *)PUSH_BUTTON_ADDRESS;
  return *button & 0x1;
}

//----------WORLD-----------------
// The world map (screen grid) that holds snake parts and apples
unsigned char map[MAP_HEIGHT][MAP_WIDTH];

// Initializes the world map to empty
void initializeMap()
{
  for (int i = 0; i < MAP_HEIGHT; i++)
  {
    for (int j = 0; j < MAP_WIDTH; j++)
    {
      map[i][j] = 0;
    }
  }
}
// Draws the current game map (snake and apples)
void drawMap()
{
  for (int i = 0; i < MAP_HEIGHT; i++)
  {
    for (int j = 0; j < MAP_WIDTH; j++)
    {
      if (map[i][j] != 0)
      {
        drawBox(j * MAP_PIXEL_SIZE, i * MAP_PIXEL_SIZE, MAP_PIXEL_SIZE, MAP_PIXEL_SIZE);
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

// Move snake one step right , left , up and down
char moveRight()
{
  if (xPos + 1 >= MAP_WIDTH)
  {
    return 1;
  }
  else if (map[yPos][xPos + 1] == 'a')
  {
    map[yPos][xPos + 1] = direction;
    snakeLength++;
    apples--;
    applesEaten++;
    xPos++;
  }
  else if (map[yPos][xPos + 1] != 0)
  {
    return 1;
  }
  xPos++;
  map[yPos][xPos] = direction;
  return 0;
}
char moveLeft()
{
  if (xPos - 1 < 0)
  {
    return 1;
  }
  else if (map[yPos][xPos - 1] == 'a')
  {
    map[yPos][xPos - 1] = direction;
    snakeLength++;
    apples--;
    applesEaten++;
    xPos--;
  }
  else if (map[yPos][xPos - 1] != 0)
  {
    return 1;
  }
  xPos--;
  map[yPos][xPos] = direction;
  return 0;
}
char moveUp()
{

  if (yPos - 1 < 0)
  {
    return 1;
  }
  else if (map[yPos - 1][xPos] == 'a')
  {
    map[yPos - 1][xPos] = direction;
    snakeLength++;
    apples--;
    applesEaten++;
    yPos--;
  }
  else if (map[yPos - 1][xPos] != 0)
  {
    return 1;
  }
  yPos--;
  map[yPos][xPos] = direction;
  return 0;
}

char moveDown()
{
  if (yPos + 1 >= MAP_HEIGHT)
  {
    return 1;
  }
  else if (map[yPos + 1][xPos] == 'a')
  {
    map[yPos + 1][xPos] = direction;
    snakeLength++;
    apples--;
    applesEaten++;
    yPos++;
  }
  else if (map[yPos + 1][xPos] != 0)
  {
    return 1;
  }
  yPos++;
  map[yPos][xPos] = direction;
  return 0;
}
// Moves the snake one step based on the current direction
char moveSnake()
{
  char result = 0;
  if (direction == 'r')
  {
    result = moveRight();
  }
  else if (direction == 'l')
  {
    result = moveLeft();
  }
  else if (direction == 'u')
  {
    result = moveUp();
  }
  else if (direction == 'd')
  {
    result = moveDown();
  }
  // If the snake hits a wall or itself then  Game Over
  if (result == 1)
  {
    return 1;
  }
  // Move the tail forward
  char tailDir = map[tailY][tailX];
  map[tailY][tailX] = 0;
  if (tailDir == 'r')
  {
    tailX++;
  }
  else if (tailDir == 'l')
  {
    tailX--;
  }
  else if (tailDir == 'u')
  {
    tailY--;
  }
  else if (tailDir == 'd')
  {
    tailY++;
  }

  return 0;
}

// Helper functions to rotate direction based on turn input
void turnFromRight(char turn)
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

void turnFromLeft(char turn)
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

void turnFromUp(char turn)
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

void turnFromDown(char turn)
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
// Rotate snake left ('l') or right ('r') depending on current direction
void rotateSnake(char turn)
{
  if (direction == 'r')
  {
    turnFromRight(turn);
  }
  else if (direction == 'l')
  {
    turnFromLeft(turn);
  }
  else if (direction == 'u')
  {
    turnFromUp(turn);
  }
  else if (direction == 'd')
  {
    turnFromDown(turn);
  }
}

//----------APPLE-----------------
static unsigned seed = 1234567;
int rand()
{
  seed = seed * 1103515245 + 12345;
  return (seed >> 16) & 0x7FFF;
}
// Spawns apples randomly on the map
void spawnApple()
{
  if (snakeLength < apples + 10) // limit apples on screen
  {
    return;
  }
  while (1)
  {
    int appleX = rand() % MAP_WIDTH;
    int appleY = rand() % MAP_HEIGHT;
    if (map[appleY][appleX] == 0)
    { // empty cell
      map[appleY][appleX] = 'a';
      apples++;
      break;
    }
  }
}
//----------Play game --------
// Reset all game state variables and recreate initial snake
void resetGame()
{
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
  {
    map[yPos][xPos - i] = 'r';
  }

  tailX = xPos - 9;
}
// Handle switch inputs (SW0 → right, SW1 → left)
void handleInput()
{
  int switches = get_sw();

  if (switches & 0x1)
  {
    while (get_sw() & 0x1)
    {
    }
    rotateSnake('r');
  }
  else if (switches & 0x2)
  {
    while (get_sw() & 0x2)
    {
    }
    rotateSnake('l');
  }
}
// Main game loop
void playGame(void)
{
  char gameOverFlag = 0;
  resetGame();

  while (1)
  {
    clearScreen();
    drawMap();
    //handleInput();

    map[yPos][xPos] = direction;
    gameOverFlag = moveSnake();
    spawnApple();
    delay(15);
    updateScoreDisplay();

    if (gameOverFlag)
    {
      delay(100);
      break;
    }
  }

  gameOver();
}

//----------INTERRUPT HANDLER-----------------
/* Minimal interrupt handler */


/*
 * handle_interrupt()
 * Called automatically when an interrupt is triggered by the switches.
 * SW0 → turn right ('r')
 * SW1 → turn left  ('l')
 */
void handle_interrupt(unsigned cause)
{
    // cause 17 = switch interrupt (on DTEK-V)
    if (cause == 17)
    {
        volatile int *edgecapture = (volatile int *)(TOGGLE_SWITCHES_ADDRESS + 0xC);
        int edges = *edgecapture;  // read which switch triggered

        if (edges & 0x1)  // SW0 pressed
        {
            rotateSnake('r');
        }
        if (edges & 0x2)  // SW1 pressed
        {
            rotateSnake('l');
        }

        *edgecapture = edges;  // clear interrupt flags
    }
}

/*
 * init_switch_interrupt()
 * Enables interrupts for SW0 and SW1.
 */
void init_switch_interrupt(void)
{
    volatile int *edgecapture   = (volatile int *)(TOGGLE_SWITCHES_ADDRESS + 0xC);
    volatile int *interruptmask = (volatile int *)(TOGGLE_SWITCHES_ADDRESS + 0x8);

    *edgecapture = 0xFFFFFFFF;  // clear old flags
    *interruptmask = 0x3;       // enable SW0 and SW1 (bit0 + bit1)

    enable_interrupt();         // enable global CPU interrupts
}
