#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>

// =====================================================
// MAX7219 SETTINGS
// =====================================================
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

#define MAX_DEVICES 4
#define DATA_PIN 7
#define CS_PIN 10
#define CLK_PIN 6
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// =====================================================
// JOYSTICK
// =====================================================
#define JOY_X 1   // VRx - avoid GPIO0 (boot strapping pin)
#define JOY_Y 2   // VRy
#define JOY_SW 3  // button (optional, unused for now)

const int CENTER = 2048;   // ESP32 ADC is 12-bit (0-4095), center ~2048
const int DEADZONE = 800;  // how far from center counts as a "push"

// =====================================================
// DISPLAY SIZE
// =====================================================
#define WIDTH 32
#define HEIGHT 8

// =====================================================
// GAME SETTINGS
// =====================================================
#define MAX_SNAKE_LENGTH 100
int snakeX[MAX_SNAKE_LENGTH];
int snakeY[MAX_SNAKE_LENGTH];
int snakeLength = 3;

// Snake starts moving RIGHT
int dirX = 1;
int dirY = 0;

// Food
int foodX;
int foodY;

// Timing
unsigned long lastMove = 0;
const int moveDelay = 200;

// =====================================================
// SETUP
// =====================================================
void setup()
{
  Serial.begin(115200);

  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 3);
  mx.clear();

  // IMPORTANT FOR YOUR PANEL
  mx.transform(MD_MAX72XX::TRC);

  pinMode(JOY_SW, INPUT_PULLUP);

  randomSeed(millis());
  startGame();
}

// =====================================================
// MAIN LOOP
// =====================================================
void loop()
{
  readJoystick();

  if (millis() - lastMove > moveDelay)
  {
    lastMove = millis();
    moveSnake();
    checkFood();
    checkCollision();
    drawGame();
  }
}

// =====================================================
// START / RESET GAME
// =====================================================
void startGame()
{
  snakeLength = 3;

  // Initial snake position
  snakeX[0] = 2;
  snakeY[0] = 3;
  snakeX[1] = 1;
  snakeY[1] = 3;
  snakeX[2] = 0;
  snakeY[2] = 3;

  // Initial movement RIGHT
  dirX = 1;
  dirY = 0;

  generateFood();
}

// =====================================================
// JOYSTICK CONTROLS
// =====================================================
void readJoystick()
{
  int xVal = analogRead(JOY_X);
  int yVal = analogRead(JOY_Y);

  int xOffset = xVal - CENTER;
  int yOffset = yVal - CENTER;

  // Only register the dominant direction (prevents diagonal confusion)
  if (abs(xOffset) > abs(yOffset)) {
    if (xOffset > DEADZONE && dirX != -1)      { dirX = 1; dirY = 0; }   // right
    else if (xOffset < -DEADZONE && dirX != 1) { dirX = -1; dirY = 0; }  // left
  } else {
    if (yOffset > DEADZONE && dirY != -1)      { dirX = 0; dirY = 1; }   // down
    else if (yOffset < -DEADZONE && dirY != 1) { dirX = 0; dirY = -1; }  // up
  }
}

// =====================================================
// MOVE SNAKE
// =====================================================
void moveSnake()
{
  // Move body
  for (int i = snakeLength; i > 0; i--)
  {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  // Move head
  snakeX[0] += dirX;
  snakeY[0] += dirY;

  // Wrap screen
  if (snakeX[0] >= WIDTH) snakeX[0] = 0;
  if (snakeX[0] < 0) snakeX[0] = WIDTH - 1;
  if (snakeY[0] >= HEIGHT) snakeY[0] = 0;
  if (snakeY[0] < 0) snakeY[0] = HEIGHT - 1;
}

// =====================================================
// GENERATE FOOD
// =====================================================
void generateFood()
{
  bool valid = false;
  while (!valid)
  {
    valid = true;
    foodX = random(0, WIDTH);
    foodY = random(0, HEIGHT);

    // Avoid snake body
    for (int i = 0; i < snakeLength; i++)
    {
      if (foodX == snakeX[i] && foodY == snakeY[i])
      {
        valid = false;
      }
    }
  }
}

// =====================================================
// CHECK FOOD
// =====================================================
void checkFood()
{
  if (snakeX[0] == foodX && snakeY[0] == foodY)
  {
    // Increase snake length
    if (snakeLength < MAX_SNAKE_LENGTH - 1)
    {
      snakeLength++;
    }
    generateFood();
  }
}

// =====================================================
// CHECK COLLISION
// =====================================================
void checkCollision()
{
  for (int i = 1; i < snakeLength; i++)
  {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i])
    {
      gameOverAnimation();
      startGame();
    }
  }
}

// =====================================================
// GAME OVER ANIMATION
// =====================================================
void gameOverAnimation()
{
  for (int k = 0; k < 3; k++)
  {
    mx.clear();
    drawText("REVO EMAG");
    delay(600);
  }
  mx.clear();
}

// =====================================================
// DRAW TEXT
// =====================================================
void drawText(String text)
{
  // Simple scrolling text effect
  for (int pos = WIDTH; pos > -((int)text.length() * 6); pos--)
  {
    mx.clear();
    for (int i = 0; i < text.length(); i++)
    {
      mx.setChar(pos + (i * 6), text[i]);
    }
    mx.update();
    delay(40);
  }
}

// =====================================================
// DRAW GAME
// =====================================================
void drawGame()
{
  mx.clear();

  // Draw snake
  for (int i = 0; i < snakeLength; i++)
  {
    mx.setPoint(snakeY[i], snakeX[i], true);
  }

  // Draw food
  mx.setPoint(foodY, foodX, true);
  mx.update();
}