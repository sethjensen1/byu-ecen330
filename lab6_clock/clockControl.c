#include "clockControl.h"
#include "clockDisplay.h"
#include "touchscreen.h"
#include <stdio.h>

#define LONG_PRESS_DELAY_S 0.5
#define FAST_UPDATE_PERIOD_S 0.1

// Define state machine states. Initialize current state with unique name for
// this file
enum clockControl_state {
  waiting,
  inc_dec,
  long_press_delay,
  fast_update
} clockControl_current_state = waiting;

static uint64_t delay_cnt = 0;
static double delay_num_ticks;

static uint64_t update_cnt = 0;
static double update_num_ticks;

void clockControl_debugStatePrint();

// Initialize the clock control state machine, with a given period in seconds.
void clockControl_init(double period_s) {
  clockControl_current_state = waiting;
  delay_num_ticks = LONG_PRESS_DELAY_S / period_s;
  update_num_ticks = FAST_UPDATE_PERIOD_S / period_s;
}

// Tick the clock control state machine
void clockControl_tick() {
  clockControl_debugStatePrint();
  // Get the status once per tick
  touchscreen_status_t touchscreen_status = touchscreen_get_status();

  // Transition
  switch (clockControl_current_state) {
  case waiting:
    // If touchscreen has been released, move to increment/decrement state
    //
    // If touchscreen has been pressed (and held for longer than our tick
    // length), start counting in long press delay state
    if (touchscreen_status == TOUCHSCREEN_RELEASED) {
      clockControl_current_state = inc_dec;
    } else if (touchscreen_status == TOUCHSCREEN_PRESSED) {
      delay_cnt = 0;
      clockControl_current_state = long_press_delay;
    }
    break;
  case inc_dec:
    // Touchscreen should still be in released status
    // leading to this state, unless you're erroneously calling
    // touchscreen_ack_touch() elsewhere
    // released / touchscreen_ack_touch() -> waiting
    //
    // else -> waiting
    // Error state, should never happen
    if (touchscreen_status == TOUCHSCREEN_RELEASED) {
      touchscreen_ack_touch();
      clockControl_current_state = waiting;
    } else if (touchscreen_status == TOUCHSCREEN_PRESSED) {
      // pressed -> long_press_delay
      clockControl_current_state = long_press_delay;
    } else {
      clockControl_current_state = waiting;
    }
    break;
  case long_press_delay:
    // released -> inc_dec
    //
    // delay_cnt >= delay_num_ticks / update_cnt = 0 -> fast_update
    if (touchscreen_status == TOUCHSCREEN_RELEASED) {
      clockControl_current_state = inc_dec;
    } else if (delay_cnt >= delay_num_ticks) {
      update_cnt = 0;
      clockControl_current_state = fast_update;
    }
    break;
  case fast_update:
    // released / touchscreen_ack_touch() -> waiting
    //
    // update_cnt >= update_num_ticks / update_cnt = 0;
    // clockDisplay_performIncDec(); -> fast_update
    if (touchscreen_status == TOUCHSCREEN_RELEASED) {
      touchscreen_ack_touch();
      clockControl_current_state = waiting;
    } else if (update_cnt >= update_num_ticks) {
      update_cnt = 0;
      clockDisplay_performIncDec(touchscreen_get_location());
    }
    break;
  }

  // Action
  switch (clockControl_current_state) {
  case waiting:
    break;
  case inc_dec:
    clockDisplay_performIncDec(touchscreen_get_location());
    break;
  case long_press_delay:
    delay_cnt++;
    break;
  case fast_update:
    update_cnt++;
    break;
  }
}

// print the current state for debugging
void clockControl_debugStatePrint() {
  static enum clockControl_state previous_state;
  static bool firstPass = true;

  // initialize the previousState if it's the first pass
  if (firstPass) {
    previous_state = clockControl_current_state;
    firstPass = false;
  }

  // if the state has changed, print the state and update previous_state for the
  // next iteration
  if (clockControl_current_state != previous_state) {
    // Print the current state
    switch (clockControl_current_state) {
    case waiting:
      printf("waiting\n");
      break;
    case inc_dec:
      printf("inc_dec\n");
      break;
    case long_press_delay:
      printf("long_press_delay\n");
      break;
    case fast_update:
      printf("fast_update\n");
      break;
    }

    // Update previous_state for next iteration
    previous_state = clockControl_current_state;
  }
}