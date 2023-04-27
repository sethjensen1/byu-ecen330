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

#define TEXT_SIZE 2
#define CURSOR_X 10
#define CURSOR_Y (DISPLAY_HEIGHT / 2)

// Print out "hello world" on both the console and the LCD screen.
int main() {

  // Initialize display driver, and fill scren with black
  display_init();
  display_fillScreen(DISPLAY_BLACK); // Blank the screen.

  // Configure display text settings
  display_setTextColor(DISPLAY_RED); // Make the text red.
  display_setTextSize(TEXT_SIZE);    // Make the text a little larger.

  // Set the cursor location and print to the LCD
  display_setCursor(CURSOR_X, CURSOR_Y);
  display_println("hello world (on the LCD)!");

  // Also print out 'hello world' on the terminal (stdout).
  printf("hello world!\n");
  return 0;
}
