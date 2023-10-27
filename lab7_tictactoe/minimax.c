#include "minimax.h"
#include "ticTacToe.h"
#include <stdio.h>

#define BOARD_TOP 0
#define BOARD_BOTTOM TICTACTOE_BOARD_ROWS - 1

#define BOARD_LEFT 0
#define BOARD_RIGHT TICTACTOE_BOARD_COLUMNS - 1

#define BOARD_CENTER 1

#define NO_SCORE 999 // Arbitrary high value to denote no score
#define STARTING_DEPTH 0

// global choice of next move
tictactoe_location_t choice;

// Print out the current board with X, O, and space representing their
// counterparts
void minimax_printBoard(tictactoe_board_t board) {
  // Loop through the board's rows
  printf("-----\n");
  for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
    printf("|");
    // Loop through the board's columns
    for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
      char square_display;

      // Print based on the state of the current square
      switch (board.squares[row][column]) {
      case MINIMAX_EMPTY_SQUARE:
        square_display = ' ';
        break;
      case MINIMAX_O_SQUARE:
        square_display = 'O';
        break;
      case MINIMAX_X_SQUARE:
        square_display = 'X';
        break;
      }

      printf("%c", square_display);
    }
    printf("|\n");
  }
  printf("-----\n");
}

minimax_score_t minimax(tictactoe_board_t *board, bool is_Xs_turn,
                        uint8_t depth) {
  // printf("Depth: %d\n", depth);

  // Evaluate board based upon prev player's turn.
  minimax_score_t current_score = minimax_computeBoardScore(board, !is_Xs_turn);

  if (minimax_isGameOver(current_score)) {
    // Recursion base case, there has been a win or a draw.
    return current_score;
  }

  // move/score table in a 2d array. Matches board size, but instead of storing
  // an X or O, stores the score. There's an arbitrary value (NO_SCORE) chosen
  // to represent no score, but it's high enough that no possible path could
  // have that high of a score.
  minimax_score_t scores[TICTACTOE_BOARD_ROWS][TICTACTOE_BOARD_COLUMNS] = {
      NO_SCORE};

  // Otherwise, you need to recurse.
  // This loop will generate all possible boards and call
  // minimax recursively for every empty square
  for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
    // loop columns second
    for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
      // Check if space is playable
      if (board->squares[row][column] == MINIMAX_EMPTY_SQUARE) {

        // Simulate playing at this location. This changes the value in the
        // actual board, but we'll reset it to empty after testing.
        board->squares[row][column] =
            is_Xs_turn ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;

        // Recursively call minimax to get the best score, assuming player
        // choses to play at this location.
        minimax_score_t score = minimax(board, !is_Xs_turn, depth + 1);

        scores[row][column] = score;

        // Undo the change to the board
        board->squares[row][column] = MINIMAX_EMPTY_SQUARE;
      } else {
        scores[row][column] = NO_SCORE;
      }
    }
  }

  // Once you get here, you have iterated over empty squares at this level.
  // All of the scores have been computed in the move-score table for boards
  // at this level. Now you need to return the score depending upon whether
  // you are computing min or max.
  minimax_score_t score = 0;

  // So that we can choose the first empty square as a default choice
  bool finding_first_square = true;

  if (is_Xs_turn) {
    minimax_score_t highest = 0;
    // loop through the scores table to find the highest
    for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
      // loop columns second
      for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
        // make sure there is a real score stored here
        if ((scores[row][column] != NO_SCORE)) {
          // If we haven't made a choice & this is our first empty square,
          // choose it to start
          if (finding_first_square) {
            highest = scores[row][column];
            choice.row = row;
            choice.column = column;

            finding_first_square = false;
          }

          // compare with the highest score and replace if necessary
          if (scores[row][column] > highest) {
            highest = scores[row][column];
            choice.row = row;
            choice.column = column;
          }
        }
      }
    }
    score = highest;
  } else {
    minimax_score_t lowest = 0;
    // loop through the scores table to find the highest
    for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
      // loop columns second
      for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
        // make sure there is a real score stored here
        if ((scores[row][column] != NO_SCORE)) {
          // If we haven't made a choice & this is our first empty square,
          // choose it to start
          if (finding_first_square) {
            lowest = scores[row][column];
            choice.row = row;
            choice.column = column;

            finding_first_square = false;
          }

          // compare with the highest score and replace if necessary
          if (scores[row][column] < lowest) {
            lowest = scores[row][column];
            choice.row = row;
            choice.column = column;
          }
        }
      }
    }
    score = lowest;
  }

  // minimax_printBoard(*board);
  // printf("Current board score: %d\n", score);
  return score;
}

// Returns the score of the board.
// This returns one of 4 values: MINIMAX_X_WINNING_SCORE,
// MINIMAX_O_WINNING_SCORE, MINIMAX_DRAW_SCORE, MINIMAX_NOT_ENDGAME
// Note: the is_Xs_turn argument indicates which player just took their
// turn and makes it possible to speed up this function.
// Assumptions:
// (1) if is_Xs_turn == true, the last thing played was an 'X'.
// (2) if is_Xs_turn == false, the last thing played was an 'O'.
// Hint: If you know the game was not over when an 'X' was played,
// you don't need to look for 'O's, and vice-versa.
minimax_score_t minimax_computeBoardScore(tictactoe_board_t *board,
                                          bool is_Xs_turn) {
  if (is_Xs_turn) {
    // check for row wins
    for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
      // check columns second
      for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
        // If, on a given row, we run into something that's not an X, then we
        // don't have a win on that row. Stop iterating and go to next row.
        if (board->squares[row][column] != MINIMAX_X_SQUARE) {
          break;
        } else if (column == (TICTACTOE_BOARD_COLUMNS - 1)) {
          return MINIMAX_X_WINNING_SCORE;
        }
      }
    }

    // check for column wins
    for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
      // Check rows second
      for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
        // If, on a given column, we run into something that's not an X, then we
        // don't have a win on that column. Stop iterating and go to next
        // column.
        if (board->squares[row][column] != MINIMAX_X_SQUARE) {
          break;
        } else if (row == (TICTACTOE_BOARD_ROWS - 1)) {
          return MINIMAX_X_WINNING_SCORE;
        }
      }
    }

    // check for down/right diagonal win.
    if (board->squares[BOARD_TOP][BOARD_LEFT] == MINIMAX_X_SQUARE &&
        board->squares[BOARD_CENTER][BOARD_CENTER] == MINIMAX_X_SQUARE &&
        board->squares[BOARD_BOTTOM][BOARD_RIGHT] == MINIMAX_X_SQUARE) {
      return MINIMAX_X_WINNING_SCORE;
    }

    // check for down/left diagonal win
    if (board->squares[BOARD_TOP][BOARD_RIGHT] == MINIMAX_X_SQUARE &&
        board->squares[BOARD_CENTER][BOARD_CENTER] == MINIMAX_X_SQUARE &&
        board->squares[BOARD_BOTTOM][BOARD_LEFT] == MINIMAX_X_SQUARE) {
      return MINIMAX_X_WINNING_SCORE;
    }

    // check for game not being finished. Since X played last, if X didn't win
    // AND if there are some empty squares the game isn't over
    for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
      // check all columns second
      for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
        // If we find an empty square and the current player hasn't won, then
        // it's not endgame.
        if (board->squares[row][column] == MINIMAX_EMPTY_SQUARE) {
          return MINIMAX_NOT_ENDGAME;
        }
      }
    }
  } else { // it's O's turn
    // check for row wins
    for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
      // check columns second
      for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
        // If, on a given row, we run into something that's not an O, then we
        // don't have a win on that row. Stop iterating and go to next row.
        if (board->squares[row][column] != MINIMAX_O_SQUARE) {
          break;
        } else if (column == (TICTACTOE_BOARD_COLUMNS - 1)) {
          return MINIMAX_O_WINNING_SCORE;
        }
      }
    }

    // check for column wins
    for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
      // Check rows second
      for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
        // If, on a given column, we run into something that's not an O, then we
        // don't have a win on that column. Stop iterating and go to next
        // column.
        if (board->squares[row][column] != MINIMAX_O_SQUARE) {
          break;
        } else if (row == (TICTACTOE_BOARD_ROWS - 1)) {
          return MINIMAX_O_WINNING_SCORE;
        }
      }
    }

    // check for down/right diagonal win.
    if (board->squares[BOARD_TOP][BOARD_LEFT] == MINIMAX_O_SQUARE &&
        board->squares[BOARD_CENTER][BOARD_CENTER] == MINIMAX_O_SQUARE &&
        board->squares[BOARD_BOTTOM][BOARD_RIGHT] == MINIMAX_O_SQUARE) {
      return MINIMAX_O_WINNING_SCORE;
    }

    // check for down/left diagonal win
    if (board->squares[BOARD_TOP][BOARD_RIGHT] == MINIMAX_O_SQUARE &&
        board->squares[BOARD_CENTER][BOARD_CENTER] == MINIMAX_O_SQUARE &&
        board->squares[BOARD_BOTTOM][BOARD_LEFT] == MINIMAX_O_SQUARE) {
      return MINIMAX_O_WINNING_SCORE;
    }

    // check for game not being finished. Since O played last, if O didn't win
    // AND if there are some empty squares the game isn't over
    for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
      // check all columns second
      for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
        // If we find an empty square and the current player hasn't won, then
        // it's not endgame.
        if (board->squares[row][column] == MINIMAX_EMPTY_SQUARE) {
          return MINIMAX_NOT_ENDGAME;
        }
      }
    }
  }

  // If neither X or O won, then it's a draw
  return MINIMAX_DRAW_SCORE;
}

// This routine is not recursive but will invoke the recursive minimax function.
// You will call this function from the controlling state machine that you will
// implement in a later milestone. It computes the row and column of the next
// move based upon: the current board and player.
//
// When called from the controlling state machine, you will call this function
// as follows:
// 1. If the computer is playing as X, you will call this function with
// is_Xs_turn = true.
// 2. If the computer is playing as O, you will call this function with
// is_Xs_turn = false.
// This function directly passes the  is_Xs_turn argument into the minimax()
// (helper) function.
tictactoe_location_t minimax_computeNextMove(tictactoe_board_t *board,
                                             bool is_Xs_turn) {
  // minimax_printBoard(*board);
  minimax(board, is_Xs_turn,
          STARTING_DEPTH); // This will modify the global choice variable
  return choice;
}

// Init the board to all empty squares.
void minimax_initBoard(tictactoe_board_t *board) {
  // For every row and column, set the board at that location to be empty
  for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
    for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
      board->squares[row][column] = MINIMAX_EMPTY_SQUARE;
    }
  }
}

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score) {
  return score != MINIMAX_NOT_ENDGAME;
}