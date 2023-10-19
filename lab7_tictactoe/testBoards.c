/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "minimax.h"
#include <stdio.h>

#define TOP 0
#define MID 1
#define BOT 2
#define LFT 0
#define RGT 2

// Test the next move code, given several boards.
// You need to also create 10 boards of your own to test.
void testBoards() {
  tictactoe_board_t board1; // Board 1 is the main example in the web-tutorial
                            // that I use on the web-site.
  board1.squares[TOP][LFT] = MINIMAX_O_SQUARE;
  board1.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board1.squares[TOP][RGT] = MINIMAX_X_SQUARE;
  board1.squares[MID][LFT] = MINIMAX_X_SQUARE;
  board1.squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board1.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board1.squares[BOT][LFT] = MINIMAX_X_SQUARE;
  board1.squares[BOT][MID] = MINIMAX_O_SQUARE;
  board1.squares[BOT][RGT] = MINIMAX_O_SQUARE;

  tictactoe_board_t board2;
  board2.squares[TOP][LFT] = MINIMAX_O_SQUARE;
  board2.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board2.squares[TOP][RGT] = MINIMAX_X_SQUARE;
  board2.squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board2.squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board2.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board2.squares[BOT][LFT] = MINIMAX_X_SQUARE;
  board2.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board2.squares[BOT][RGT] = MINIMAX_O_SQUARE;

  tictactoe_board_t board3;
  board3.squares[TOP][LFT] = MINIMAX_O_SQUARE;
  board3.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board3.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board3.squares[MID][LFT] = MINIMAX_O_SQUARE;
  board3.squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board3.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board3.squares[BOT][LFT] = MINIMAX_X_SQUARE;
  board3.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board3.squares[BOT][RGT] = MINIMAX_X_SQUARE;

  tictactoe_board_t board4;
  board4.squares[TOP][LFT] = MINIMAX_O_SQUARE;
  board4.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board4.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board4.squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board4.squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board4.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board4.squares[BOT][LFT] = MINIMAX_X_SQUARE;
  board4.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board4.squares[BOT][RGT] = MINIMAX_X_SQUARE;

  tictactoe_board_t board5;
  board5.squares[TOP][LFT] = MINIMAX_X_SQUARE;
  board5.squares[TOP][MID] = MINIMAX_X_SQUARE;
  board5.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board5.squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board5.squares[MID][MID] = MINIMAX_O_SQUARE;
  board5.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board5.squares[BOT][LFT] = MINIMAX_EMPTY_SQUARE;
  board5.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board5.squares[BOT][RGT] = MINIMAX_EMPTY_SQUARE;

  tictactoe_board_t board6;
  board6.squares[TOP][LFT] = MINIMAX_X_SQUARE;
  board6.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board6.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board6.squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board6.squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board6.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board6.squares[BOT][LFT] = MINIMAX_EMPTY_SQUARE;
  board6.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board6.squares[BOT][RGT] = MINIMAX_EMPTY_SQUARE;

  tictactoe_board_t board7;
  board7.squares[TOP][LFT] = MINIMAX_O_SQUARE;
  board7.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board7.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board7.squares[MID][LFT] = MINIMAX_X_SQUARE;
  board7.squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board7.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board7.squares[BOT][LFT] = MINIMAX_O_SQUARE;
  board7.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board7.squares[BOT][RGT] = MINIMAX_O_SQUARE;

  tictactoe_board_t board8;
  board8.squares[TOP][LFT] = MINIMAX_EMPTY_SQUARE;
  board8.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board8.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board8.squares[MID][LFT] = MINIMAX_X_SQUARE;
  board8.squares[MID][MID] = MINIMAX_EMPTY_SQUARE;
  board8.squares[MID][RGT] = MINIMAX_X_SQUARE;
  board8.squares[BOT][LFT] = MINIMAX_O_SQUARE;
  board8.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board8.squares[BOT][RGT] = MINIMAX_O_SQUARE;

  tictactoe_board_t board9;
  board9.squares[TOP][LFT] = MINIMAX_EMPTY_SQUARE;
  board9.squares[TOP][MID] = MINIMAX_O_SQUARE;
  board9.squares[TOP][RGT] = MINIMAX_X_SQUARE;
  board9.squares[MID][LFT] = MINIMAX_X_SQUARE;
  board9.squares[MID][MID] = MINIMAX_X_SQUARE;
  board9.squares[MID][RGT] = MINIMAX_O_SQUARE;
  board9.squares[BOT][LFT] = MINIMAX_EMPTY_SQUARE;
  board9.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board9.squares[BOT][RGT] = MINIMAX_EMPTY_SQUARE;

  tictactoe_board_t board10;
  board10.squares[TOP][LFT] = MINIMAX_X_SQUARE;
  board10.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board10.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board10.squares[MID][LFT] = MINIMAX_O_SQUARE;
  board10.squares[MID][MID] = MINIMAX_O_SQUARE;
  board10.squares[MID][RGT] = MINIMAX_X_SQUARE;
  board10.squares[BOT][LFT] = MINIMAX_EMPTY_SQUARE;
  board10.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board10.squares[BOT][RGT] = MINIMAX_X_SQUARE;

  tictactoe_board_t board11;
  board11.squares[TOP][LFT] = MINIMAX_O_SQUARE;
  board11.squares[TOP][MID] = MINIMAX_X_SQUARE;
  board11.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board11.squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board11.squares[MID][MID] = MINIMAX_X_SQUARE;
  board11.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board11.squares[BOT][LFT] = MINIMAX_O_SQUARE;
  board11.squares[BOT][MID] = MINIMAX_O_SQUARE;
  board11.squares[BOT][RGT] = MINIMAX_EMPTY_SQUARE;

  tictactoe_board_t board12;
  board12.squares[TOP][LFT] = MINIMAX_O_SQUARE;
  board12.squares[TOP][MID] = MINIMAX_O_SQUARE;
  board12.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board12.squares[MID][LFT] = MINIMAX_X_SQUARE;
  board12.squares[MID][MID] = MINIMAX_X_SQUARE;
  board12.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board12.squares[BOT][LFT] = MINIMAX_EMPTY_SQUARE;
  board12.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board12.squares[BOT][RGT] = MINIMAX_EMPTY_SQUARE;

  tictactoe_board_t board13;
  board13.squares[TOP][LFT] = MINIMAX_O_SQUARE;
  board13.squares[TOP][MID] = MINIMAX_EMPTY_SQUARE;
  board13.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board13.squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board13.squares[MID][MID] = MINIMAX_X_SQUARE;
  board13.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board13.squares[BOT][LFT] = MINIMAX_X_SQUARE;
  board13.squares[BOT][MID] = MINIMAX_EMPTY_SQUARE;
  board13.squares[BOT][RGT] = MINIMAX_O_SQUARE;

  tictactoe_board_t board14;
  board14.squares[TOP][LFT] = MINIMAX_EMPTY_SQUARE;
  board14.squares[TOP][MID] = MINIMAX_O_SQUARE;
  board14.squares[TOP][RGT] = MINIMAX_EMPTY_SQUARE;
  board14.squares[MID][LFT] = MINIMAX_X_SQUARE;
  board14.squares[MID][MID] = MINIMAX_X_SQUARE;
  board14.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board14.squares[BOT][LFT] = MINIMAX_EMPTY_SQUARE;
  board14.squares[BOT][MID] = MINIMAX_X_SQUARE;
  board14.squares[BOT][RGT] = MINIMAX_O_SQUARE;

  tictactoe_board_t board15;
  board15.squares[TOP][LFT] = MINIMAX_X_SQUARE;
  board15.squares[TOP][MID] = MINIMAX_O_SQUARE;
  board15.squares[TOP][RGT] = MINIMAX_X_SQUARE;
  board15.squares[MID][LFT] = MINIMAX_EMPTY_SQUARE;
  board15.squares[MID][MID] = MINIMAX_X_SQUARE;
  board15.squares[MID][RGT] = MINIMAX_EMPTY_SQUARE;
  board15.squares[BOT][LFT] = MINIMAX_O_SQUARE;
  board15.squares[BOT][MID] = MINIMAX_X_SQUARE;
  board15.squares[BOT][RGT] = MINIMAX_O_SQUARE;

  tictactoe_location_t move;

  bool is_Xs_turn = true;

  move = minimax_computeNextMove(&board1, is_Xs_turn);
  printf("next move for board1: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board2, is_Xs_turn);
  printf("next move for board2: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board3, is_Xs_turn);
  printf("next move for board3: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board4, !is_Xs_turn);
  printf("next move for board4: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board5, !is_Xs_turn);
  printf("next move for board5: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board6, !is_Xs_turn);
  printf("next move for board6: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board7, is_Xs_turn);
  printf("next move for board7: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board8, !is_Xs_turn);
  printf("next move for board8: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board9, !is_Xs_turn);
  printf("next move for board9: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board10, !is_Xs_turn);
  printf("next move for board10: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board11, is_Xs_turn);
  printf("next move for board11: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board12, is_Xs_turn);
  printf("next move for board12: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board13, is_Xs_turn);
  printf("next move for board13: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board14, !is_Xs_turn);
  printf("next move for board14: (%d, %d)\n", move.row, move.column);

  move = minimax_computeNextMove(&board15, !is_Xs_turn);
  printf("next move for board15: (%d, %d)\n", move.row, move.column);
}
