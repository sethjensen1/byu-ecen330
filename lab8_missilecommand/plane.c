#include "plane.h"
#include "config.h"
#include "display.h"
#include "missile.h"
#include <stdbool.h>
#include <stdio.h>

typedef enum { PLANE_INIT_ST, PLANE_FLY_ST, PLANE_DEAD_ST } plane_state_t;

#define PLANE_Y DISPLAY_HEIGHT / 4
#define PLANE_ORIGIN_X DISPLAY_WIDTH

#define PLANE_MAX_DISTANCE_TRAVELED DISPLAY_WIDTH
#define PLANE_LAUNCH_DISTANCE DISPLAY_WIDTH / 2

static plane_state_t plane_currentState = PLANE_INIT_ST;

static uint16_t traveled = 0;

// Distance is the top right corner of the plane
static int16_t x_current = PLANE_ORIGIN_X;
static int16_t y_current = PLANE_Y;
static bool explode = false;
static bool launched = false;
static missile_t *missile;
static uint16_t death_cnt = 0;

void plane_debugStatePrint(void);

// Initialize the plane state machine
// Pass in a pointer to the missile struct (the plane will only have one
// missile)
void plane_init(missile_t *plane_missile) {
  plane_currentState = PLANE_INIT_ST;
  missile_init_dead(plane_missile);
  missile = plane_missile;
}

// State machine tick function
void plane_tick() {
  // plane_debugStatePrint();
  // Transition
  switch (plane_currentState) {
  case PLANE_INIT_ST:
    // Initialize the plane state machine. Move to the FLY state immediately
    plane_currentState = PLANE_FLY_ST;
    traveled = 0;
    explode = false;
    launched = false;
    death_cnt = 0;
    x_current = PLANE_ORIGIN_X; // Start at right edge
    break;
  case PLANE_FLY_ST:
    // if plane needs to explode or if it reached the left side, the plane is
    // dead. Otherwise, if the plane has crossed the launch threshold, launch
    // the missile
    if (explode || traveled >= PLANE_MAX_DISTANCE_TRAVELED) {
      plane_currentState = PLANE_DEAD_ST;
      // Erase plane
      display_fillTriangle(
          x_current, y_current, x_current, y_current + CONFIG_PLANE_HEIGHT,
          x_current - CONFIG_PLANE_WIDTH, y_current + (CONFIG_PLANE_HEIGHT / 2),
          CONFIG_BACKGROUND_COLOR);
    } else if (!launched && traveled >= PLANE_LAUNCH_DISTANCE) {
      missile_init_plane(missile, x_current, y_current);
      launched = true;
    }
    break;
  case PLANE_DEAD_ST:
    if (death_cnt >= CONFIG_PLANE_RESPAWN_DELAY_TICKS) {
      plane_currentState = PLANE_INIT_ST;
    }
    break;
  }

  // Action
  switch (plane_currentState) {
  case PLANE_INIT_ST:
    break;

  case PLANE_FLY_ST:
    // Erase plane
    display_fillTriangle(
        x_current, y_current, x_current, y_current + CONFIG_PLANE_HEIGHT,
        x_current - CONFIG_PLANE_WIDTH, y_current + (CONFIG_PLANE_HEIGHT / 2),
        CONFIG_BACKGROUND_COLOR);

    traveled += CONFIG_PLANE_DISTANCE_PER_TICK;
    // simplified expression since we're only traveling from right to left
    x_current = PLANE_ORIGIN_X - traveled;

    // Draw plane
    display_fillTriangle(x_current, y_current, x_current,
                         y_current + CONFIG_PLANE_HEIGHT,
                         x_current - CONFIG_PLANE_WIDTH,
                         y_current + (CONFIG_PLANE_HEIGHT / 2), DISPLAY_WHITE);
    break;
  case PLANE_DEAD_ST:
    death_cnt++;
    break;
  }
}

// Trigger the plane to expode
void plane_explode() { explode = true; }

// Get the XY location of the plane
display_point_t plane_getXY() {
  display_point_t current;
  current.x = x_current;
  current.y = y_current;

  return current;
}

// print the current state for debugging
void plane_debugStatePrint(void) {
  static plane_state_t previousState;
  static bool firstPass = true;

  // if the state has changed, print the state and update previous_state for the
  // next iteration
  if (firstPass || plane_currentState != previousState) {
    // Print the current state
    switch (plane_currentState) {
    case PLANE_INIT_ST:
      printf("PLANE_INIT_ST\n");
      break;
    case PLANE_FLY_ST:
      printf("PLANE_FLY_ST\n");
      break;
    case PLANE_DEAD_ST:
      printf("PLANE_DEAD_ST\n");
      break;
    }

    // initialize the previousState if it's the first pass
    if (firstPass) {
      previousState = plane_currentState;
      firstPass = false;
    }

    // Update previous_state for next iteration
    previousState = plane_currentState;
  }
}