#include "gpioTest.h"
#include "buttons.h"
#include "display.h"
#include "leds.h"
#include "switches.h"
#include <stdio.h>

// Rectangle size
#define RECTANGLE_HEIGHT (DISPLAY_HEIGHT / 2)
#define RECTANGLE_WIDTH (DISPLAY_WIDTH / 4)

// The Y position of every rectangle, to be displayed when buttons are pressed
#define RECTANGLE_Y 0

// The X position of each button rectangle.
#define BTN_0_RECTANGLE_X ((DISPLAY_WIDTH / 2) + RECTANGLE_WIDTH)
#define BTN_1_RECTANGLE_X (DISPLAY_WIDTH / 2)
#define BTN_2_RECTANGLE_X (DISPLAY_WIDTH / 4)
#define BTN_3_RECTANGLE_X 0

// Padding for the text. 10 to give some left margin, and 1/4 the display height
// minus half the text size to vertically center in the rectangles
#define TEXT_OFFSET_X 20
#define TEXT_OFFSET_Y 50
#define TEXT_SIZE 2

// Runs a test of the buttons. As you push the buttons, graphics and messages
// will be written to the LCD panel. The test will run until all 4 pushbuttons
// are simultaneously pressed.
void gpioTest_buttons() {
  // Initialize buttons and display to show which buttons are pressed.
  buttons_init();
  display_init();

  // Set text color and size
  display_setTextColor(DISPLAY_WHITE);
  display_setTextSize(TEXT_SIZE);

  // Read the buttons to start
  uint8_t buttonsValue = buttons_read();
  uint8_t lastButtonsValue = buttonsValue;

  // Store whether each button is on
  uint8_t button0_on = buttonsValue & BUTTONS_BTN0_MASK;
  uint8_t button1_on = buttonsValue & BUTTONS_BTN1_MASK;
  uint8_t button2_on = buttonsValue & BUTTONS_BTN2_MASK;
  uint8_t button3_on = buttonsValue & BUTTONS_BTN3_MASK;

  // Run the test until all buttons are pressed at once
  while (buttonsValue != 0xF) {
    // Read the buttons
    buttonsValue = buttons_read();

    // Check whether each button is on
    button0_on = buttonsValue & BUTTONS_BTN0_MASK;
    button1_on = buttonsValue & BUTTONS_BTN1_MASK;
    button2_on = buttonsValue & BUTTONS_BTN2_MASK;
    button3_on = buttonsValue & BUTTONS_BTN3_MASK;

    // If button 0's value has changed since the last loop, decide whether to
    // draw a rectangle
    if (button0_on != (lastButtonsValue & BUTTONS_BTN0_MASK)) {
      // If the button's value has changed, then if it is on, draw a colored
      // rectangle with text in it. If it has changed and it is off, then draw a
      // black rectangle to effectively turn that rectangle off
      if (button0_on) {
        // Draw colored rectangle and text at this button's rectangle location
        display_fillRect(BTN_0_RECTANGLE_X, RECTANGLE_Y, RECTANGLE_WIDTH,
                         RECTANGLE_HEIGHT, DISPLAY_GREEN);
        display_setCursor(BTN_0_RECTANGLE_X + TEXT_OFFSET_X,
                          RECTANGLE_Y + TEXT_OFFSET_Y);
        display_println("BTN0");
      } else {
        // Draw blank black rectangle where the colored one was
        display_fillRect(BTN_0_RECTANGLE_X, RECTANGLE_Y, RECTANGLE_WIDTH,
                         RECTANGLE_HEIGHT, DISPLAY_BLACK);
      }
    }

    // If button 1's value has changed since the last loop, decide whether to
    // draw a rectangle
    if (button1_on != (lastButtonsValue & BUTTONS_BTN1_MASK)) {
      // If the button's value has changed, then if it is on, draw a colored
      // rectangle with text in it. If it has changed and it is off, then draw a
      // black rectangle to effectively turn that rectangle off
      if (button1_on) {
        // Draw colored rectangle and text at this button's rectangle location
        display_fillRect(BTN_1_RECTANGLE_X, RECTANGLE_Y, RECTANGLE_WIDTH,
                         RECTANGLE_HEIGHT, DISPLAY_BLUE);
        display_setCursor(BTN_1_RECTANGLE_X + TEXT_OFFSET_X,
                          RECTANGLE_Y + TEXT_OFFSET_Y);
        display_println("BTN1");
      } else {
        // Draw blank black rectangle where the colored one was
        display_fillRect(BTN_1_RECTANGLE_X, RECTANGLE_Y, RECTANGLE_WIDTH,
                         RECTANGLE_HEIGHT, DISPLAY_BLACK);
      }
    }

    // If button 2's value has changed since the last loop, decide whether to
    // draw a rectangle
    if (button2_on != (lastButtonsValue & BUTTONS_BTN2_MASK)) {
      // If the button's value has changed, then if it is on, draw a colored
      // rectangle with text in it. If it has changed and it is off, then draw a
      // black rectangle to effectively turn that rectangle off
      if (button2_on) {
        // Draw colored rectangle and text at this button's rectangle location
        display_fillRect(BTN_2_RECTANGLE_X, RECTANGLE_Y, RECTANGLE_WIDTH,
                         RECTANGLE_HEIGHT, DISPLAY_MAGENTA);
        display_setCursor(BTN_2_RECTANGLE_X + TEXT_OFFSET_X,
                          RECTANGLE_Y + TEXT_OFFSET_Y);
        display_println("BTN2");
      } else {
        // Draw blank black rectangle where the colored one was
        display_fillRect(BTN_2_RECTANGLE_X, RECTANGLE_Y, RECTANGLE_WIDTH,
                         RECTANGLE_HEIGHT, DISPLAY_BLACK);
      }
    }

    // If button 3's value has changed since the last loop, decide whether to
    // draw a rectangle
    if (button3_on != (lastButtonsValue & BUTTONS_BTN3_MASK)) {
      // If the button's value has changed, then if it is on, draw a colored
      // rectangle with text in it. If it has changed and it is off, then draw a
      // black rectangle to effectively turn that rectangle off
      if (button3_on) {
        // Draw colored rectangle and text at this button's rectangle location
        display_fillRect(BTN_3_RECTANGLE_X, RECTANGLE_Y, RECTANGLE_WIDTH,
                         RECTANGLE_HEIGHT, DISPLAY_RED);
        display_setCursor(BTN_3_RECTANGLE_X + TEXT_OFFSET_X,
                          RECTANGLE_Y + TEXT_OFFSET_Y);
        display_println("BTN3");
      } else {
        // Draw blank black rectangle where the colored one was
        display_fillRect(BTN_3_RECTANGLE_X, RECTANGLE_Y, RECTANGLE_WIDTH,
                         RECTANGLE_HEIGHT, DISPLAY_BLACK);
      }
    }

    // Finally, set our last buttons value tracker to the current value, so it's
    // accurate for the next loop
    lastButtonsValue = buttonsValue;
  }

  // Reset display to black, return
  display_fillScreen(DISPLAY_BLACK);
  return;
}

// Runs a test of the switches. As you slide the switches, LEDs directly above
// the switches will illuminate. The test will run until all switches are slid
// upwards. When all 4 slide switches are slid upward, this function will
// return.
void gpioTest_switches() {
  // Initialize switches and the LEDs to show which switches are on
  switches_init();
  leds_init();

  // Read the initial switches value
  uint8_t switchesValue = switches_read();

  printf("%x", switchesValue);

  // Continue to test the switch value and illuminate the corresponding LEDs
  // until all switches are on at the same time.
  while (switchesValue != 0xF) {
    switchesValue = switches_read();
    leds_write(switchesValue);
  }

  leds_write(0x00);
  return;
}