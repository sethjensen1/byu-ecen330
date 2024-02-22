#ifndef EGGMAN
#define EGGMAN

#include <stdint.h>

#define EGGMAN_WIDTH 68
#define EGGMAN_HEIGHT 83

// This defines the top left corner
#define EGGMAN_X 20
#define EGGMAN_Y (DISPLAY_HEIGHT - EGGMAN_HEIGHT - 10)

#define EGGS_WIDTH 81
#define EGGS_HEIGHT 84
#define EGGS_X (DISPLAY_WIDTH - EGGS_WIDTH - 20)
#define EGGS_Y (DISPLAY_HEIGHT - EGGS_HEIGHT - 10)

#define SINGLE_EGG_HEIGHT 41
#define SINGLE_EGG_WIDTH 30

#define SINGLE_EGG_Y (DISPLAY_HEIGHT - SINGLE_EGG_HEIGHT - 30)

#define EGG_X_ORIGIN (EGGS_X - SINGLE_EGG_WIDTH)
#define EGG_X_DEST (EGGMAN_X + 10)

#define MAX_DISTANCE (EGG_X_ORIGIN - EGG_X_DEST)

#define EGG_DISTANCE_PER_S (DISPLAY_WIDTH / 2)

#define MESSAGE_X (DISPLAY_WIDTH / 2) - (3 * TEXT_SIZE)
#define MESSAGE_Y 40

#define TEXT_SIZE 2
#define MESSAGE_0 "-4 eggs"
#define MESSAGE_1 "1 egg"
#define MESSAGE_2 "Dude, you ran out of eggs"

extern uint8_t eggman_openmouth[];
extern uint8_t eggs[];
extern uint8_t single_egg[];

#endif
