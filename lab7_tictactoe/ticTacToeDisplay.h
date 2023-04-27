/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#ifndef TICTACTOEDISPLAY
#define TICTACTOEDISPLAY

#include <stdbool.h>
#include <stdint.h>

#include "display.h"
#include "ticTacToe.h"

// Inits the tic-tac-toe display, drawing the lines that form the board.
void ticTacToeDisplay_init();

// Draws an X at the specified location
// erase == true means to erase the X by redrawing it as background.
void ticTacToeDisplay_drawX(tictactoe_location_t location, bool erase);

// Draws an O at the specified row and column.
// erase == true means to erase the X by redrawing it as background.
void ticTacToeDisplay_drawO(tictactoe_location_t location, bool erase);

// For a given touch location on the touchscreen, this function returns the
// corresponding tictactoe board location.
tictactoe_location_t
ticTacToeDisplay_getLocationFromPoint(display_point_t point);

#endif /* TICTACTOEDISPLAY */
