#include "buttons.h"
#include "display.h"
#include "minimax.h"
#include "ticTacToe.h"
#include "ticTacToeDisplay.h"
#include "touchscreen.h"

#include <stdio.h>

#define INSTRUCTIONS_DELAY_S 2
#define FIRST_MOVE_DELAY_S 3

#define INSTRUCTIONS_CURSOR_X 0
#define INSTRUCTIONS_CURSOR_Y DISPLAY_HEIGHT / 3
#define INSTRUCTIONS_TEXT_SIZE 2
#define INSTRUCTIONS                                                           \
  "   Touch board to play X\n          -or-\n   wait for the computer\n   "    \
  "     and play O."

#define BACKGROUND_COLOR DISPLAY_DARK_BLUE

#define TOP 0
#define LEFT 0

// Define state machine states. Initialize current state with unique name for
// this file
enum ticTacToeControl_state {
  init,
  draw_instructions,
  instructions_delay,
  erase_instructions,
  reset,
  draw_board,
  first_move,
  computer_turn,
  wait_for_player,
  calculate_player_location,
  draw_move,
  check_endgame,
  game_over
} ticTacToeControl_current_state = init;

static uint64_t delay_cnt = 0;
static double instructions_delay_num_ticks;
static double first_move_delay_num_ticks;

static bool computer_is_x = true;
static bool is_player_turn = false;

static bool is_first_turn = true;

static tictactoe_board_t my_board;

static tictactoe_location_t pressed_location;
static tictactoe_location_t next_move;

void ticTacToeControl_debugStatePrint();

// Tick the tic-tac-toe controller state machine
void ticTacToeControl_tick() {

  // Transition
  switch (ticTacToeControl_current_state) {
  case init:
    ticTacToeControl_current_state = draw_instructions;
    break;
  case draw_instructions:
    ticTacToeControl_current_state = instructions_delay;
    delay_cnt = 0;
    break;
  case instructions_delay:
    if (delay_cnt > instructions_delay_num_ticks) {
      ticTacToeControl_current_state = erase_instructions;
    }
    break;
  case erase_instructions:
    ticTacToeControl_current_state = draw_board;
    break;
  case draw_board:
    ticTacToeControl_current_state = reset;
    break;
  case reset:
    ticTacToeControl_current_state = first_move;
    delay_cnt = 0;
    break;
  case first_move:
    // wait for 3 seconds; if the doesn't touch the screen in this time to
    // start, then the computer starts
    if (delay_cnt > first_move_delay_num_ticks) {
      ticTacToeControl_current_state = computer_turn;
      computer_is_x = true;
    } else if (touchscreen_get_status() ==
               TOUCHSCREEN_RELEASED) { // If it's the first move, this will
                                       // always be a valid move
      ticTacToeControl_current_state = calculate_player_location;
      computer_is_x = false;
      pressed_location =
          ticTacToeDisplay_getLocationFromPoint(touchscreen_get_location());
      touchscreen_ack_touch();
    }
    break;
  case computer_turn:
    ticTacToeControl_current_state = draw_move;
    break;
  case calculate_player_location: {
    // Check if the player's move is valid. If not, wait for screen to be
    // released in a valid location
    ticTacToeControl_current_state = draw_move;
    next_move = pressed_location;
    break;
  }
  case wait_for_player: {
    // set the global pressed location
    pressed_location =
        ticTacToeDisplay_getLocationFromPoint(touchscreen_get_location());

    // check if the screen has been released and if it is a valid move
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED &&
        my_board.squares[pressed_location.row][pressed_location.column] ==
            MINIMAX_EMPTY_SQUARE) {
      ticTacToeControl_current_state = calculate_player_location;
      touchscreen_ack_touch();
    }
    break;
  }
  case draw_move:
    ticTacToeControl_current_state = check_endgame;
    break;
  case check_endgame:
    // Check for game end, and if it's not game over, then go to the next
    // player's turn
    if (minimax_isGameOver(minimax_computeBoardScore(
            &my_board, is_player_turn ? !computer_is_x : computer_is_x))) {
      ticTacToeControl_current_state = game_over;
    } else if (is_player_turn) {
      ticTacToeControl_current_state = computer_turn;
    } else {
      ticTacToeControl_current_state = wait_for_player;
    }
    break;
  case game_over:
    if (buttons_read() & BUTTONS_BTN0_MASK) {
      ticTacToeControl_current_state = reset;
    }
    break;
  }

  // Action
  switch (ticTacToeControl_current_state) {
  case init:
    break;
  case draw_instructions:
    display_setCursor(INSTRUCTIONS_CURSOR_X, INSTRUCTIONS_CURSOR_Y);
    display_setTextColor(DISPLAY_WHITE);
    display_setTextSize(INSTRUCTIONS_TEXT_SIZE);
    display_println(INSTRUCTIONS);
    break;
  case instructions_delay:
    delay_cnt++;
    break;
  case erase_instructions:
    // print text in background color to erase
    display_setCursor(INSTRUCTIONS_CURSOR_X, INSTRUCTIONS_CURSOR_Y);
    display_setTextColor(BACKGROUND_COLOR);
    display_setTextSize(INSTRUCTIONS_TEXT_SIZE);
    display_println(INSTRUCTIONS);
    break;
  case draw_board:
    ticTacToeDisplay_init();
    break;
  case reset:
    touchscreen_ack_touch();

    // Check each board position, and draw a background X or O to erase it.
    for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
      for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
        tictactoe_location_t current_location = {row, column};

        // Draw a background colored X or O for every X and O on the board
        if (my_board.squares[row][column] == MINIMAX_X_SQUARE) {
          ticTacToeDisplay_drawX(current_location, true);
        } else if (my_board.squares[row][column] == MINIMAX_O_SQUARE) {
          ticTacToeDisplay_drawO(current_location, true);
        }
      }
    }
    minimax_initBoard(&my_board);
    break;
  case first_move:
    delay_cnt++;
    break;
  case computer_turn:
    is_player_turn = false;
    // Play in the top left if the computer starts, to save time on the
    // algorithm
    if (is_first_turn) {
      next_move.row = TOP;
      next_move.column = LEFT;
      is_first_turn = false;
    } else {
      next_move = minimax_computeNextMove(&my_board, computer_is_x);
    }
    break;
  case calculate_player_location:
    is_player_turn = true;
    if (is_first_turn) {
      is_first_turn = false;
    }
    break;
  case wait_for_player:
    break;
  case draw_move:
    my_board.squares[next_move.row][next_move.column] =
        is_player_turn ? (computer_is_x ? MINIMAX_O_SQUARE : MINIMAX_X_SQUARE)
                       : (computer_is_x ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE);

    // Print the current player's move, whether X or O
    if (is_player_turn && !computer_is_x || !is_player_turn && computer_is_x) {
      ticTacToeDisplay_drawX(next_move, false);
    } else {
      ticTacToeDisplay_drawO(next_move, false);
    }
    break;
  case check_endgame:
    break;
  case game_over:
    break;
  }
}

// Initialize the tic-tac-toe controller state machine,
// providing the tick period, in seconds.
void ticTacToeControl_init(double period_s) {
  buttons_init();
  display_init();
  display_fillScreen(BACKGROUND_COLOR);
  ticTacToeControl_current_state = init;
  instructions_delay_num_ticks = INSTRUCTIONS_DELAY_S / period_s;
  first_move_delay_num_ticks = FIRST_MOVE_DELAY_S / period_s;
}

// print the current state for debugging
void ticTacToeControl_debugStatePrint() {
  static enum ticTacToeControl_state previous_state;
  static bool firstPass = true;

  // initialize the previousState if it's the first pass
  if (firstPass) {
    previous_state = ticTacToeControl_current_state;
    firstPass = false;
  }

  // if the state has changed, print the state and update previous_state for the
  // next iteration
  if (ticTacToeControl_current_state != previous_state) {
    // Print the current state
    switch (ticTacToeControl_current_state) {
    case init:
      printf("init\n");
      break;
    case draw_instructions:
      printf("draw_instructions\n");
      break;
    case instructions_delay:
      printf("instructions_delay\n");
      break;
    case erase_instructions:
      printf("erase_instructions\n");
      break;
    case reset:
      printf("reset\n");
      break;
    case draw_board:
      printf("draw_board\n");
      break;
    case first_move:
      printf("first_move\n");
      break;
    case computer_turn:
      printf("computer_turn\n");
      break;
    case calculate_player_location:
      printf("calculate_player_location\n");
      break;
    case wait_for_player:
      printf("wait_for_player\n");
      break;
    case draw_move:
      printf("draw_move\n");
      break;
    case check_endgame:
      printf("check_endgame\n");
      break;
    case game_over:
      printf("game_over\n");
      break;
    }

    // Update previous_state for next iteration
    previous_state = ticTacToeControl_current_state;
  }
}