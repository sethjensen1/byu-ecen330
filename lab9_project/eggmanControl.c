#include "display.h"
#include "eggman.h"
#include "intervalTimer.h"
#include "touchscreen.h"
#include <stdio.h>

#define BACKGROUND_COLOR DISPLAY_BLUE

#define MESSAGE_DELAY_S 5

typedef enum { INIT_ST, WAIT_ST, MOVE_ST, EAT_ST, MESSAGE_ST } eggman_state_t;

static eggman_state_t currentState;

static double egg_distance_per_tick;
static double egg_distance = 0;

static uint16_t egg_x_current = 0;

static double message_timer = 0;
static uint32_t message_delay_ticks;

static uint8_t message_number;

// Tick the eggman controller state machine
void eggmanControl_tick() {
  // Transition
  switch (currentState) {
  case INIT_ST:
    currentState = WAIT_ST;
    break;
  case WAIT_ST:
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      touchscreen_ack_touch();
      egg_distance = 0;
      display_drawBitmap(egg_x_current, SINGLE_EGG_Y, single_egg,
                         SINGLE_EGG_WIDTH, SINGLE_EGG_HEIGHT, DISPLAY_BLACK);
      currentState = MOVE_ST;
    }
    break;
  case MOVE_ST:
    if (egg_distance >= MAX_DISTANCE) {
      // Erase egg
      display_drawBitmap(egg_x_current, SINGLE_EGG_Y, single_egg,
                         SINGLE_EGG_WIDTH, SINGLE_EGG_HEIGHT, BACKGROUND_COLOR);
      // Redraw eggman
      display_drawBitmap(EGGMAN_X, EGGMAN_Y, eggman_openmouth, EGGMAN_WIDTH,
                         EGGMAN_HEIGHT, DISPLAY_BLACK);
      currentState = EAT_ST;
    }
    break;
  case EAT_ST:
    if (rand() % 7 == 0) { // 1 in 10 chance
      message_number = rand() % 3;

      display_setCursor(MESSAGE_X, MESSAGE_Y);
      display_setTextColor(DISPLAY_BLACK);

      if (message_number == 0) {
        display_println(MESSAGE_0);
      } else if (message_number == 1) {
        display_println(MESSAGE_1);
      } else {
        display_println(MESSAGE_2);
      }
      currentState = MESSAGE_ST;
    } else {
      currentState = WAIT_ST;
    }
    break;
  case MESSAGE_ST:
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED ||
        message_timer > message_delay_ticks) {
      if (touchscreen_get_status() == TOUCHSCREEN_RELEASED)
        touchscreen_ack_touch();
      currentState = WAIT_ST;
      // erase message text
      display_setCursor(MESSAGE_X, MESSAGE_Y);
      display_setTextColor(BACKGROUND_COLOR);

      if (message_number == 0) {
        display_println(MESSAGE_0);
      } else if (message_number == 1) {
        display_println(MESSAGE_1);
      } else {
        display_println(MESSAGE_2);
      }
    }

    break;
  }

  // Action
  switch (currentState) {
  case INIT_ST:
    break;
  case WAIT_ST:
    break;
  case MOVE_ST:
    // erase egg
    display_drawBitmap(egg_x_current, SINGLE_EGG_Y, single_egg,
                       SINGLE_EGG_WIDTH, SINGLE_EGG_HEIGHT, BACKGROUND_COLOR);

    egg_distance += egg_distance_per_tick;

    // update current position
    egg_x_current = EGG_X_ORIGIN +
                    (egg_distance / MAX_DISTANCE) * (EGG_X_DEST - EGG_X_ORIGIN);

    // draw egg
    display_drawBitmap(egg_x_current, SINGLE_EGG_Y, single_egg,
                       SINGLE_EGG_WIDTH, SINGLE_EGG_HEIGHT, DISPLAY_BLACK);
    break;
  case EAT_ST:
    break;
  case MESSAGE_ST:
    message_timer++;
    break;
  }
}

// Initialize the eggman controller state machine,
// providing the tick period, in seconds.
void eggmanControl_init(double period_s) {
  currentState = INIT_ST;

  display_init();
  display_fillScreen(BACKGROUND_COLOR);
  display_drawBitmap(EGGMAN_X, EGGMAN_Y, eggman_openmouth, EGGMAN_WIDTH,
                     EGGMAN_HEIGHT, DISPLAY_BLACK);
  display_drawBitmap(EGGS_X, EGGS_Y, eggs, EGGS_WIDTH, EGGS_HEIGHT,
                     DISPLAY_BLACK);

  message_delay_ticks = MESSAGE_DELAY_S / period_s;
  egg_distance_per_tick = EGG_DISTANCE_PER_S * period_s;

  display_setTextColor(DISPLAY_BLACK);
  display_setTextSize(TEXT_SIZE);
}