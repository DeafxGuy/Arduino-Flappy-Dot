#include <LedControl.h>

// Pins: DIN,CLK,CS,Number of displays
LedControl lc = LedControl(12, 10, 11, 1);

const int buttonPin = 2;
int buttonState = 0;
int lastButtonState = 0; // to keep track of the last button state
int playerPos = 1; // start at the 2nd row from the top
bool playerVisible = true;
unsigned long lastFallTime = 0; // to keep track of the last time the player fell
unsigned long lastObstacleTime = 0; // to keep track of the last time the obstacle moved
int obstaclePos = 7; // start the obstacle at the rightmost column
int gapPos = random(1, 6); // position of the gap in the obstacle
bool fallingOff = false; // track if the player is about to fall off the matrix
int score = 0; // track the player's score
int gameSpeed = 400; // game speed in milliseconds

// Game over screen
int gameOverScreen[8][8] = {
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {1, 0, 1, 0, 0, 1, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 1, 1, 0, 0, 1},
  {1, 0, 1, 0, 0, 1, 0, 1},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 0, 1, 1, 1, 1, 0, 0}
};

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  pinMode(buttonPin, INPUT_PULLUP); // enable internal pull-up resistor
  randomSeed(analogRead(0)); // initialize random number generator
}

void loop() {
  buttonState = digitalRead(buttonPin);
  unsigned long currentTime = millis(); // get the current time

  lc.clearDisplay(0); // clear the display before drawing the player

  if (((playerPos < gapPos || playerPos > gapPos + 2) && obstaclePos == 1) || fallingOff) { // game over condition
    // display the game over screen
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) {
        lc.setLed(0, y, x, gameOverScreen[y][x]);
      }
    }
    delay(1000); // wait for 1 seconds
    lc.clearDisplay(0); // clear the display before restarting the game

    // display the score
    for (int i = 0; i < score; i++) {
      lc.setLed(0, i / 8, i % 8, 1);
    }
    delay(2000); // wait for 2 seconds
    lc.clearDisplay(0); // clear the display before restarting the game

    playerPos = 1; // reset the player position
    obstaclePos = 7; // reset the obstacle position
    gapPos = random(1, 6); // reset the gap position
    fallingOff = false; // reset the falling off flag
    score = 0; // reset the score
  }

  if (buttonState == LOW && lastButtonState == HIGH) { // button is pressed when the pin reads LOW
    if (playerPos > 0) {
      playerPos--;
      fallingOff = false; // reset the falling off flag when the player moves up
    }
    lastFallTime = currentTime; // reset the fall time when the button is pressed
  } else if (currentTime - lastFallTime >= gameSpeed) { // check if gameSpeed ms have passed since the last fall
    if (playerPos < 7) {
      playerPos++;
    } else if (!fallingOff) {
      fallingOff = true; // set the falling off flag if the player is on the bottom row
    }
    lastFallTime = currentTime; // update the last fall time
  }

  if (currentTime - lastObstacleTime >= gameSpeed) { // check if gameSpeed ms have passed since the last obstacle move
    if (obstaclePos > 0) {
      obstaclePos--;
    } else {
      obstaclePos = 7; // reset the obstacle position
      gapPos = random(1, 6); // reset the gap position
      score++; // increase the score
    }
    lastObstacleTime = currentTime; // update the last obstacle time
  }

  lastButtonState = buttonState; // update the last button state

  playerVisible = !playerVisible;
  lc.setRow(0, playerPos, playerVisible ? B01000000 : B00000000); // player representation

  // draw the obstacle
  for (int y = 0; y < 8; y++) {
    if (y < gapPos || y > gapPos + 2) {
      lc.setLed(0, y, obstaclePos, 1);
    }
  }

  delay(100);
}

