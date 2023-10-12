#include "touchscreen.h"
#include "display.h"
#include "xparameters.h"
#include <stdbool.h>
#include <stdio.h>

#define ADC_SETTLE_TIME .05

enum touchscreen_state {
  waiting_st,
  adc_settling_st,
  pressed_st
} touchscreen_current_state = waiting_st;

static touchscreen_status_t status;
static uint64_t adc_settle_ticks;
static uint64_t adc_timer;
static int16_t x, y;
static uint8_t z;

// print the current state for debugging
void touchscreen_debugStatePrint() {
  static enum touchscreen_state previousState;
  static bool firstPass = true;

  // initialize the previousState if it's the first pass
  if (firstPass) {
    previousState = touchscreen_current_state;
    firstPass = false;
  }

  // if the state has changed, print the state and update previousState for the
  // next iteration
  if (touchscreen_current_state != previousState) {
    switch (touchscreen_current_state) {
    case waiting_st:
      printf("waiting_st\n");
      break;
    case adc_settling_st:
      printf("adc_settling_st\n");
      break;
    case pressed_st:
      printf("pressed_st\n");
      break;
    }
    previousState = touchscreen_current_state;
  }
}

// Initialize the touchscreen driver state machine, with a given tick period (in
// seconds).
void touchscreen_init(double period_seconds) {
  touchscreen_current_state = waiting_st;
  adc_timer = 0;
  adc_settle_ticks = ADC_SETTLE_TIME / period_seconds;
}

// Tick the touchscreen driver state machine
void touchscreen_tick() {
  // touchscreen_debugStatePrint();

  // Transition
  switch (touchscreen_current_state) {
  case waiting_st:
    if (display_isTouched()) {
      display_clearOldTouchData();
      touchscreen_current_state = adc_settling_st;
    }
    break;
  case adc_settling_st:
    if (!display_isTouched()) {
      touchscreen_current_state = waiting_st;
    } else if (display_isTouched() && adc_timer == adc_settle_ticks) {
      display_getTouchedPoint(&x, &y, &z);
      touchscreen_current_state = pressed_st;
    }
    break;
  case pressed_st:
    if (!display_isTouched()) {
      status = TOUCHSCREEN_RELEASED;
      touchscreen_current_state = waiting_st;
    }
    break;
  }

  // Action
  switch (touchscreen_current_state) {
  case waiting_st:
    adc_timer = 0;
    break;
  case adc_settling_st:
    adc_timer++;
    break;
  case pressed_st:
    status = TOUCHSCREEN_PRESSED;
    break;
  }
}

// Return the current status of the touchscreen
touchscreen_status_t touchscreen_get_status() { return status; }

// Acknowledge the touchscreen touch.  This function will only have effect when
// the touchscreen is in the TOUCHSCREEN_RELEASED status, and will cause it to
// switch to the TOUCHSCREEN_IDLE status.
void touchscreen_ack_touch() {
  if (status == TOUCHSCREEN_RELEASED) {
    status = TOUCHSCREEN_IDLE;
  }
}

// Get the (x,y) location of the last touchscreen touch
display_point_t touchscreen_get_location() {
  display_point_t location = {x, y};
  return location;
}