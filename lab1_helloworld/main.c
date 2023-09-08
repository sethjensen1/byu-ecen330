/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include <stdio.h>

#include "display.h"

#define DISPLAY_CENTER_X (DISPLAY_WIDTH / 2)
#define DISPLAY_CENTER_Y (DISPLAY_HEIGHT / 2)

#define CIRCLE_RADIUS 30
#define CIRCLE_DISTANCE (DISPLAY_WIDTH / 4)

#define TRIANGLE_DISTANCE (DISPLAY_HEIGHT / 8)
#define TRIANGLE_HALF_BASE (30)

// Print out "hello world" on both the console and the LCD screen.
int main() {

  // Initialize display driver, and fill scren with black
  display_init();
  display_fillScreen(DISPLAY_BLACK); // Blank the screen.

  // Draw two lines crossing the screen diagonally
  display_drawLine(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_GREEN);
  display_drawLine(0, DISPLAY_HEIGHT, DISPLAY_WIDTH, 0, DISPLAY_GREEN);

  // Draw two symmetrically positioned triangles, fill the top one
  display_drawTriangle(DISPLAY_CENTER_X, (DISPLAY_CENTER_Y + TRIANGLE_DISTANCE), (DISPLAY_CENTER_X - TRIANGLE_HALF_BASE), (DISPLAY_HEIGHT - TRIANGLE_DISTANCE), (DISPLAY_CENTER_X + TRIANGLE_HALF_BASE), (DISPLAY_HEIGHT - TRIANGLE_DISTANCE), DISPLAY_YELLOW);
  display_drawTriangle(DISPLAY_CENTER_X, (DISPLAY_CENTER_Y - TRIANGLE_DISTANCE), (DISPLAY_CENTER_X - TRIANGLE_HALF_BASE), (TRIANGLE_DISTANCE), (DISPLAY_CENTER_X + TRIANGLE_HALF_BASE), (TRIANGLE_DISTANCE), DISPLAY_YELLOW);
  display_fillTriangle(DISPLAY_CENTER_X, (DISPLAY_CENTER_Y - TRIANGLE_DISTANCE), (DISPLAY_CENTER_X - TRIANGLE_HALF_BASE), (TRIANGLE_DISTANCE), (DISPLAY_CENTER_X + TRIANGLE_HALF_BASE), (TRIANGLE_DISTANCE), DISPLAY_YELLOW);

  // Draw two symmetrically positioned circles, fill in the right one
  display_drawCircle((DISPLAY_CENTER_X - CIRCLE_DISTANCE), DISPLAY_CENTER_Y, CIRCLE_RADIUS, DISPLAY_RED);
  display_drawCircle((DISPLAY_CENTER_X + CIRCLE_DISTANCE), DISPLAY_CENTER_Y, CIRCLE_RADIUS, DISPLAY_RED);
  display_fillCircle((DISPLAY_CENTER_X + CIRCLE_DISTANCE), DISPLAY_CENTER_Y, CIRCLE_RADIUS, DISPLAY_RED);

  return 0;
}
