#include <stdio.h>

#include "armInterrupts.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "touchscreen.h"
#include "utils.h"

#define PERIOD_S 0.01
#define CIRCLE_RADIUS_MAX 50
#define MS_PER_S 1000

volatile static display_point_t point1;
volatile static uint8_t radius;
static enum {
  red = DISPLAY_RED,
  black = DISPLAY_BLACK,
  blue = DISPLAY_BLUE,
  dark_blue = DISPLAY_DARK_BLUE,
  dark_red = DISPLAY_DARK_RED,
  green = DISPLAY_GREEN,
  dark_green = DISPLAY_DARK_GREEN,
  cyan = DISPLAY_CYAN,
  dark_cyan = DISPLAY_DARK_CYAN,
  magenta = DISPLAY_MAGENTA,
  dark_magenta = DISPLAY_DARK_MAGENTA,
  yellow = DISPLAY_YELLOW,
  dark_yellow = DISPLAY_DARK_YELLOW,
  white = DISPLAY_WHITE,
  gray = DISPLAY_GRAY,
  dark_gray = DISPLAY_DARK_GRAY,
  light_gray = DISPLAY_LIGHT_GRAY
} currentColor = red;

volatile static enum {
  TEST_IDLE_ST,
  TEST_TOUCH_PRESSED_ST,
  TEST_TOUCH_RELEASED_ST
} currentState = TEST_IDLE_ST;

// Tick the test code application
void test_tick() {
  display_point_t point2 = {0, 0};
  // Get the current status of the touchscreen
  touchscreen_status_t status = touchscreen_get_status();

  // Transition
  switch (currentState) {
  case TEST_IDLE_ST:
    if (status != TOUCHSCREEN_IDLE) {
      currentState = TEST_TOUCH_PRESSED_ST;

      // New button press detected, draw filled circle
      point1 = touchscreen_get_location();
      radius = 1;
      printf("x: %hu y: %hu\n", point1.x, point1.y);
    }
    break;
  case TEST_TOUCH_PRESSED_ST:
    if (status == TOUCHSCREEN_RELEASED) {
      currentState = TEST_TOUCH_RELEASED_ST;
      point2 = touchscreen_get_location();
    }
    break;
  case TEST_TOUCH_RELEASED_ST:
    currentState = TEST_IDLE_ST;
    break;
  }

  // Action
  switch (currentState) {
  case TEST_TOUCH_PRESSED_ST:
    if (radius < CIRCLE_RADIUS_MAX) {
      display_fillCircle(point1.x, point1.y, radius, currentColor);
      radius++;
    }
    break;
  case TEST_TOUCH_RELEASED_ST:
    // Press released, draw empty circle and acknowledge press
    display_fillCircle(point1.x, point1.y, radius, DISPLAY_BLACK);
    display_drawCircle(point1.x, point1.y, radius, currentColor);
    // Shows blue circle if release location is different from press
    if (point1.x != point2.x || point1.y != point2.y)
      display_drawCircle(point2.x, point2.y, radius, DISPLAY_BLUE);
    touchscreen_ack_touch();
    break;
  }

  // Cycle between colors for fun
  switch (currentColor) {
  case black:
    currentColor = blue;
    break;
  case blue:
    currentColor = dark_blue;
    break;
  case dark_blue:
    currentColor = red;
    break;
  case red:
    currentColor = dark_red;
    break;
  case dark_red:
    currentColor = green;
    break;
  case green:
    currentColor = dark_green;
    break;
  case dark_green:
    currentColor = cyan;
    break;
  case cyan:
    currentColor = dark_cyan;
    break;
  case dark_cyan:
    currentColor = magenta;
    break;
  case magenta:
    currentColor = dark_magenta;
    break;
  case dark_magenta:
    currentColor = yellow;
    break;
  case yellow:
    currentColor = dark_yellow;
    break;
  case dark_yellow:
    currentColor = white;
    break;
  case white:
    currentColor = gray;
    break;
  case gray:
    currentColor = dark_gray;
    break;
  case dark_gray:
    currentColor = light_gray;
    break;
  case light_gray:
    currentColor = black;
    break;
  }
}

// Interrupt service routine to run FSM ticks
void isr() {
  // Acknowledge timer interrupt
  intervalTimer_ackInterrupt(INTERVAL_TIMER_0);

  // Repeatedly tick the touch screen state machine
  touchscreen_tick();
  test_tick();
}

// Touchscreen test program
int main() {
  printf("========== Starting touchscreen test program ==========\n");

  // Initialize drivers
  display_init();
  interrupts_init();
  touchscreen_init(PERIOD_S);

  // Fill screen black
  display_fillScreen(DISPLAY_BLACK);

  // Set up interrupts
  interrupts_register(INTERVAL_TIMER_0_INTERRUPT_IRQ, isr);

  intervalTimer_initCountDown(INTERVAL_TIMER_0, PERIOD_S);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_0);
  intervalTimer_start(INTERVAL_TIMER_0);
  interrupts_irq_enable(INTERVAL_TIMER_0_INTERRUPT_IRQ);

  while (1) {
  }
  return 0;
}