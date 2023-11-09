#include "gameControl.h"
#include <math.h>
#include <stdio.h>

#include "config.h"
#include "missile.h"
#include "touchscreen.h"

#define DISTANCE(x1, y1, x2, y2) sqrt(pow((y2 - y1), 2) + pow((x2 - x1), 2))

missile_t enemy_missiles[CONFIG_MAX_ENEMY_MISSILES];
missile_t player_missiles[CONFIG_MAX_PLAYER_MISSILES];

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

  display_fillScreen(CONFIG_BACKGROUND_COLOR);
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
        missile_init_player(&player_missiles[i], touched_location.x,
                            touched_location.y);
        break; // Only relaunch one player missile per tick
      }
  }

  // Detect collisions

  // Check if missile i should explode, caused by an exploding missile j
  for (uint16_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++) {

    // Check if missile i collides with any enemy missiles
    for (uint16_t j = 0; j < CONFIG_MAX_ENEMY_MISSILES; j++) {

      // Only check distance for valid missiles
      if (!missile_is_flying(&enemy_missiles[i]))
        continue;
      if (!missile_is_exploding(&enemy_missiles[j]))
        continue;

      // Check if missile i is within the blast radius of missile j
      if (DISTANCE(enemy_missiles[i].x_current, enemy_missiles[i].y_current,
                   enemy_missiles[j].x_current,
                   enemy_missiles[j].y_current) <= enemy_missiles[j].radius) {
        enemy_missiles[i].explode_me = true;
      }
    }

    // Check if missile i collides with any player missiles
    for (uint16_t j = 0; j < CONFIG_MAX_PLAYER_MISSILES; j++) {

      // Only check distance for valid missiles
      if (!missile_is_flying(&enemy_missiles[i]))
        continue;
      if (!missile_is_exploding(&player_missiles[j]))
        continue;

      // Check if missile i is within the blast radius of missile j
      if (DISTANCE(enemy_missiles[i].x_current, enemy_missiles[i].y_current,
                   player_missiles[j].x_current,
                   player_missiles[j].y_current) <= player_missiles[j].radius) {
        enemy_missiles[i].explode_me = true;
      }
    }
  }

  // Tick half the missiles
  if (ticking_enemy) {
    // Tick all enemy missiles
    for (uint16_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++) {
      missile_tick(&enemy_missiles[i]);
    }
  } else {
    // Tick all player missiles
    for (uint16_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++) {
      missile_tick(&player_missiles[i]);
    }
  }

  ticking_enemy =
      !ticking_enemy; // Toggle which missiles' turn it is to be ticked
}