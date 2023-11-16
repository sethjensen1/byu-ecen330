#include "missile.h"
#include "config.h"
#include "display.h"
#include "touchscreen.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define DISTANCE(x1, y1, x2, y2) sqrt(pow((y2 - y1), 2) + pow((x2 - x1), 2))

#define ENEMY_Y_RANGE (DISPLAY_HEIGHT / 5)
#define ENEMY_IMPACT_THRESHOLD (DISPLAY_HEIGHT)

#define LAUNCH_SITE_0_MAX_X (DISPLAY_WIDTH * 3) / 8
#define LAUNCH_SITE_1_MAX_X (DISPLAY_WIDTH * 5) / 8

#define LAUNCH_SITE_0_X (DISPLAY_WIDTH / 4)
#define LAUNCH_SITE_0_Y (DISPLAY_HEIGHT)
#define LAUNCH_SITE_1_X (DISPLAY_WIDTH / 2)
#define LAUNCH_SITE_1_Y (DISPLAY_HEIGHT)
#define LAUNCH_SITE_2_X ((3 * DISPLAY_WIDTH) / 4)
#define LAUNCH_SITE_2_Y (DISPLAY_HEIGHT)

typedef enum {
  MISSILE_INIT_ST,
  MISSILE_FLY_ST,
  MISSILE_EXPLODE_GROW_ST,
  MISSILE_EXPLODE_SHRINK_ST,
  MISSILE_DEAD_ST
} missile_state_t;

void missile_debugStatePrint(missile_t *missile);

////////// State Machine INIT Functions //////////
// Unlike most state machines that have a single `init` function, our missile
// will have different initializers depending on the missile type.

// Initialize the missile as a dead missile.  This is useful at the start of the
// game to ensure that player and plane missiles aren't moving before they
// should.
void missile_init_dead(missile_t *missile) {
  missile->type = MISSILE_TYPE_PLANE;

  // set origin and destination to arbitrary value
  missile->x_origin = 0;
  missile->y_origin = 0;
  missile->x_dest = 0;
  missile->y_dest = 0;

  // Set current state
  missile->currentState = MISSILE_DEAD_ST;
}

// Initialize the missile as an enemy missile.  This will randomly choose the
// origin and destination of the missile.  The origin should be somewhere near
// the top of the screen, and the destination should be the very bottom of the
// screen.
void missile_init_enemy(missile_t *missile) {
  missile->type = MISSILE_TYPE_ENEMY;

  // Set x,y origin to random place near the top of the screen
  missile->x_origin = rand() % DISPLAY_WIDTH;
  missile->y_origin = rand() % ENEMY_Y_RANGE;

  // Set x,y destination to random location along
  // the bottom of the screen

  missile->x_dest = rand() % DISPLAY_WIDTH;
  missile->y_dest = DISPLAY_HEIGHT;

  // Set current state
  missile->currentState = MISSILE_INIT_ST;
}

// Initialize the missile as a player missile.  This function takes an (x, y)
// destination of the missile (where the user touched on the touchscreen).  The
// origin should be the closest "firing location" to the destination (there are
// three firing locations evenly spaced along the bottom of the screen).
void missile_init_player(missile_t *missile, uint16_t x_dest, uint16_t y_dest) {
  touchscreen_init(CONFIG_TOUCHSCREEN_TIMER_PERIOD);

  missile->type = MISSILE_TYPE_PLAYER;

  display_point_t closest_launch_site;

  // Set the origin to the launch site closest
  // (in the x-plane) to the destination
  if (x_dest < LAUNCH_SITE_0_MAX_X) {
    closest_launch_site.x = LAUNCH_SITE_0_X;
    closest_launch_site.y = LAUNCH_SITE_0_Y;
  } else if (x_dest < LAUNCH_SITE_1_MAX_X) {
    closest_launch_site.x = LAUNCH_SITE_1_X;
    closest_launch_site.y = LAUNCH_SITE_1_Y;
  } else {
    closest_launch_site.x = LAUNCH_SITE_2_X;
    closest_launch_site.y = LAUNCH_SITE_2_Y;
  }

  // Set x,y origin to closest missile launch site
  missile->x_origin = closest_launch_site.x;
  missile->y_origin = closest_launch_site.y;

  // x,y destination is provided (touched location)
  missile->x_dest = x_dest;
  missile->y_dest = y_dest;

  // Set current state
  missile->currentState = MISSILE_INIT_ST;
}

// Initialize the missile as a plane missile.  This function takes an (x, y)
// location of the plane which will be used as the origin.  The destination can
// be randomly chosed along the bottom of the screen.
void missile_init_plane(missile_t *missile, int16_t plane_x, int16_t plane_y) {
  missile->type = MISSILE_TYPE_PLANE;

  // Set x,y origin based on given parameters
  missile->x_origin = plane_x;
  missile->y_origin = plane_y;

  // Set x,y destination to random location along
  // the bottom of the screen

  missile->x_dest = rand() % DISPLAY_WIDTH;
  missile->y_dest = DISPLAY_HEIGHT;

  // Set current state
  missile->currentState = MISSILE_INIT_ST;
}

////////// State Machine TICK Function //////////
void missile_tick(missile_t *missile) {

  // missile_debugStatePrint(missile);

  uint16_t current_color;
  uint16_t current_distance_per_tick;

  // Set the current color and distance per
  // tick to the config values corresponding to
  // the current missile type
  switch (missile->type) {
  case MISSILE_TYPE_PLAYER:
    current_color = CONFIG_COLOR_PLAYER;
    current_distance_per_tick = CONFIG_PLAYER_MISSILE_DISTANCE_PER_TICK;
    break;
  case MISSILE_TYPE_ENEMY:
    current_color = CONFIG_COLOR_ENEMY;
    current_distance_per_tick = CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK;
    break;
  case MISSILE_TYPE_PLANE:
    current_color = CONFIG_COLOR_PLANE;
    current_distance_per_tick = CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK;
    break;
  }

  // Transition
  switch (missile->currentState) {
  case MISSILE_INIT_ST:
    missile->currentState = MISSILE_FLY_ST;

    // Initialize as a Mealy action so it happens even when starting in the INIT
    // state
    missile->length = 0;
    missile->explode_me = false;
    missile->total_length = DISTANCE(missile->x_origin, missile->y_origin,
                                     missile->x_dest, missile->y_dest);
    missile->x_current = missile->x_origin;
    missile->y_current = missile->y_origin;
    missile->impacted = false;
    missile->radius = 0;

    break;
  case MISSILE_FLY_ST:
    // If the missile gets the explode_me signal, or if it reaches its total
    // length, explode it. Otherwise, if it reaches the pixel just above the
    // bottom of the screen and is a non-player missile, then it impacts
    if (missile->explode_me || missile->type == MISSILE_TYPE_PLAYER &&
                                   (missile->length >= missile->total_length)) {
      display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                       missile->y_current, CONFIG_BACKGROUND_COLOR);
      missile->currentState = MISSILE_EXPLODE_GROW_ST;
    } else if (missile->type != MISSILE_TYPE_PLAYER &&
               missile->y_current >= ENEMY_IMPACT_THRESHOLD) {
      display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                       missile->y_current, CONFIG_BACKGROUND_COLOR);
      missile->currentState = MISSILE_DEAD_ST;
      missile->impacted = true;
    }
    break;
  case MISSILE_EXPLODE_GROW_ST:
    // After reaching the max radius, go to the shrink state
    if (missile->radius >= CONFIG_EXPLOSION_MAX_RADIUS) {
      missile->currentState = MISSILE_EXPLODE_SHRINK_ST;
    }
    break;
  case MISSILE_EXPLODE_SHRINK_ST:
    // We always subtract down to a minimum of 0, so it will always hit 0
    if (missile->radius == 0) {
      missile->currentState = MISSILE_DEAD_ST;
    }
    break;
  case MISSILE_DEAD_ST:
    break;
  }

  // Action
  switch (missile->currentState) {
  case MISSILE_INIT_ST:
    break;
  case MISSILE_FLY_ST:
    // erase line
    display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                     missile->y_current, CONFIG_BACKGROUND_COLOR);

    // update length
    missile->length += current_distance_per_tick;

    // update current position
    missile->x_current =
        missile->x_origin + (missile->length / missile->total_length) *
                                (missile->x_dest - missile->x_origin);
    missile->y_current =
        missile->y_origin + (missile->length / missile->total_length) *
                                (missile->y_dest - missile->y_origin);

    // draw new line
    display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                     missile->y_current, current_color);
    break;
  case MISSILE_EXPLODE_GROW_ST:
    missile->radius += CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK;
    display_fillCircle(missile->x_current, missile->y_current, missile->radius,
                       current_color);
    break;
  case MISSILE_EXPLODE_SHRINK_ST:
    // Erase the current circle, then shrink and draw the new one
    display_fillCircle(missile->x_current, missile->y_current, missile->radius,
                       CONFIG_BACKGROUND_COLOR);

    // shrink the radius, and if it would overflow due to subtracting below 0,
    // set it to 0 instead
    if (missile->radius < CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK) {
      missile->radius = 0;
    } else {
      missile->radius -= CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK;
    }

    // draw new circle, as long as the radius is > 0
    if (missile->radius > 0) {
      display_fillCircle(missile->x_current, missile->y_current,
                         missile->radius, current_color);
    }
    break;
  case MISSILE_DEAD_ST:
    break;
  }
}

// Return whether the given missile is dead.
bool missile_is_dead(missile_t *missile) {
  return missile->currentState == MISSILE_DEAD_ST;
}

// Return whether the given missile is exploding.  This is needed when detecting
// whether a missile hits another exploding missile.
bool missile_is_exploding(missile_t *missile) {
  return missile->currentState == MISSILE_EXPLODE_GROW_ST ||
         missile->currentState == MISSILE_EXPLODE_SHRINK_ST;
}

// Return whether the given missile is flying.
bool missile_is_flying(missile_t *missile) {
  return missile->currentState == MISSILE_FLY_ST;
}

// Used to indicate that a flying missile should be detonated.  This occurs when
// an enemy or plane missile is located within an explosion zone.
void missile_trigger_explosion(missile_t *missile) {
  missile->explode_me = true;
}

// print the current state for debugging
void missile_debugStatePrint(missile_t *missile) {
  static missile_state_t previousState;
  static bool firstPass = true;

  // initialize the previousState if it's the first pass
  if (firstPass) {
    previousState = missile->currentState;
    firstPass = false;
  }

  // if the state has changed, print the state and update previous_state for the
  // next iteration
  if (missile->currentState != previousState) {
    // Print the current state
    switch (missile->currentState) {
    case MISSILE_INIT_ST:
      printf("MISSILE_INIT_ST\n");
      break;
    case MISSILE_FLY_ST:
      printf("MISSILE_FLY_ST\n");
      break;
    case MISSILE_EXPLODE_GROW_ST:
      printf("MISSILE_EXPLODE_GROW_ST\n");
      break;
    case MISSILE_EXPLODE_SHRINK_ST:
      printf("MISSILE_EXPLODE_SHRINK_ST\n");
      break;
    case MISSILE_DEAD_ST:
      printf("MISSILE_DEAD_ST\n");
      break;
    }

    // Update previous_state for next iteration
    previousState = missile->currentState;
  }
}