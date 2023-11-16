#include "gameControl.h"
#include <math.h>
#include <stdio.h>

#include "config.h"
#include "missile.h"
#include "plane.h"
#include "touchscreen.h"

#define DISTANCE(x1, y1, x2, y2) sqrt(pow((y2 - y1), 2) + pow((x2 - x1), 2))

static missile_t enemy_missiles[CONFIG_MAX_ENEMY_MISSILES];
static missile_t player_missiles[CONFIG_MAX_PLAYER_MISSILES];
static missile_t plane_missile;

static uint16_t num_shot = 0;
static uint16_t num_shot_display = 0;

static uint16_t num_impacted = 0;
static uint16_t num_impacted_display = 0;

// Initialize the game control logic
// This function will initialize all missiles, stats, plane, etc.
void gameControl_init() {
  // Initialize enemy missiles
  for (uint8_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++) {
    missile_init_enemy(&enemy_missiles[i]);
  }

  // Initialize player missiles
  for (uint8_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++) {
    missile_init_dead(&player_missiles[i]);
  }

  // Initialize plane (which initializes the plane missile)
  plane_init(&plane_missile);

  display_fillScreen(CONFIG_BACKGROUND_COLOR);

  // Draw stats text
  display_setTextSize(CONFIG_STATS_TEXT_SIZE);
  display_setTextColor(CONFIG_STATS_TEXT_COLOR);

  display_setCursor(CONFIG_STATS_SHOT_CURSOR_X, CONFIG_STATS_CURSOR_Y);
  display_println("Shot: ");

  display_setCursor(CONFIG_STATS_IMPACTED_CURSOR_X, CONFIG_STATS_CURSOR_Y);
  display_println("Impacted: ");
}

// Given a pointer to a missile, check if that missile collides with any others.
// If it does, explode it.
void checkCollisions(missile_t *missile) {

  // Track impact and skip this missile, if it's impacted
  if (missile->impacted) {
    missile->impacted = false;
    num_impacted++;
    return;
  }

  // If this missile isn't impacted, make sure it's flying
  if (!missile_is_flying(missile))
    return;

  // Check if missile collides with any enemy missiles
  for (uint16_t j = 0; j < CONFIG_MAX_ENEMY_MISSILES; j++) {

    // Only check distance for valid missiles
    if (!missile_is_exploding(&enemy_missiles[j]))
      continue;

    // Check if missile is within the blast radius of missile j
    if (DISTANCE(missile->x_current, missile->y_current,
                 enemy_missiles[j].x_current,
                 enemy_missiles[j].y_current) <= enemy_missiles[j].radius) {
      missile->explode_me = true;
    }
  }

  // Check if missile collides with any player missiles
  for (uint16_t j = 0; j < CONFIG_MAX_PLAYER_MISSILES; j++) {

    // Only check distance for valid missiles
    if (!missile_is_exploding(&player_missiles[j]))
      continue;

    // Check if missile is within the blast radius of missile j
    if (DISTANCE(missile->x_current, missile->y_current,
                 player_missiles[j].x_current,
                 player_missiles[j].y_current) <= player_missiles[j].radius) {
      missile->explode_me = true;
    }
  }

  // Check if missile collides with plane missile
  // Only check distance for valid missiles
  if (!missile_is_exploding(&plane_missile))
    return;

  // Check if missile is within the blast radius of plane missile
  if (DISTANCE(missile->x_current, missile->y_current, plane_missile.x_current,
               plane_missile.y_current) <= plane_missile.radius) {
    missile->explode_me = true;
  }
}

// Tick the game control logic
//
// This function should tick the missiles, handle screen touches, collisions,
// and updating statistics.
void gameControl_tick() {
  // track whether we're ticking just the enemy type missiles or just the player
  // & plane missiles
  static bool ticking_enemy = true;

  // Check for dead enemy missiles and re-initialize
  for (uint16_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++)
    if (missile_is_dead(&enemy_missiles[i])) {
      missile_init_enemy(&enemy_missiles[i]);
    }

  // If the screen has been touched, check for a player missile.
  if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
    touchscreen_ack_touch();

    // Check all player missiles
    for (uint16_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++)
      // If there is a dead missile, reinitialize it with the new screen touch
      // location
      if (missile_is_dead(&player_missiles[i])) {
        display_point_t touched_location = touchscreen_get_location();
        num_shot++;
        missile_init_player(&player_missiles[i], touched_location.x,
                            touched_location.y);
        break; // Only relaunch one player missile per tick
      }
  }

  // Detect plane collisions

  display_point_t currentPlanePosition = plane_getXY();

  // Check if plane collides with any enemy missiles
  for (uint16_t j = 0; j < CONFIG_MAX_ENEMY_MISSILES; j++) {

    // Only check distance for valid missiles
    if (!missile_is_exploding(&enemy_missiles[j]))
      continue;

    // Check if plane is within the blast radius of missile j
    if (DISTANCE(currentPlanePosition.x, currentPlanePosition.y,
                 enemy_missiles[j].x_current,
                 enemy_missiles[j].y_current) <= enemy_missiles[j].radius) {
      plane_explode();
    }
  }

  // Check if plane collides with any player missiles
  for (uint16_t j = 0; j < CONFIG_MAX_PLAYER_MISSILES; j++) {

    // Only check distance for valid missiles
    if (!missile_is_exploding(&player_missiles[j]))
      continue;

    // Check if plane is within the blast radius of missile j
    if (DISTANCE(currentPlanePosition.x, currentPlanePosition.y,
                 player_missiles[j].x_current,
                 player_missiles[j].y_current) <= player_missiles[j].radius) {
      plane_explode();
    }
  }

  // Check if plane collides with exploding plane missile
  if (missile_is_exploding(&plane_missile)) {

    // Check if plane is within the blast radius of plane missile
    if (DISTANCE(currentPlanePosition.x, currentPlanePosition.y,
                 plane_missile.x_current,
                 plane_missile.y_current) <= plane_missile.radius) {
      plane_explode();
    }
  }

  // Check every enemy missile for collisions
  for (uint16_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++) {
    checkCollisions(&enemy_missiles[i]);
  }

  // Check plane missile for collisions
  checkCollisions(&plane_missile);

  // Tick half the missiles
  if (ticking_enemy) {
    // Tick all enemy missiles
    for (uint16_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++) {
      missile_tick(&enemy_missiles[i]);
    }
  } else {
    // Tick all player missiles & the plane missile
    for (uint16_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++) {
      missile_tick(&player_missiles[i]);
    }
    missile_tick(&plane_missile);
  }

  // Tick the plane
  plane_tick();

  // Stats

  display_setTextSize(CONFIG_STATS_TEXT_SIZE);

  // Erase old stats

  display_setTextColor(CONFIG_BACKGROUND_COLOR);

  display_setCursor(CONFIG_STATS_SHOT_NUMBER_CURSOR_X, CONFIG_STATS_CURSOR_Y);
  display_printDecimalInt(num_shot_display);

  display_setCursor(CONFIG_STATS_IMPACTED_NUMBER_CURSOR_X,
                    CONFIG_STATS_CURSOR_Y);
  display_printDecimalInt(num_impacted_display);

  // Draw new stats

  display_setTextColor(CONFIG_STATS_TEXT_COLOR);

  display_setCursor(CONFIG_STATS_SHOT_NUMBER_CURSOR_X, CONFIG_STATS_CURSOR_Y);
  display_printDecimalInt(num_shot);

  display_setCursor(CONFIG_STATS_IMPACTED_NUMBER_CURSOR_X,
                    CONFIG_STATS_CURSOR_Y);
  display_printDecimalInt(num_impacted);

  num_shot_display = num_shot;
  num_impacted_display = num_impacted;

  ticking_enemy =
      !ticking_enemy; // Toggle which missiles' turn it is to be ticked
}