/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include <stdint.h>
#include <stdio.h>

#include "intervalTimer.h"
#include "utils.h"
#include "xil_io.h"
#include "xparameters.h"

#define TCSR0_OFFSET 0x00
#define TCR0_OFFSET 0x08 // register offset for TCR0
#define TCR1_OFFSET 0x18 // register offset for TCR1

#define TCSR_INT_BIT 0x0100
#define TCSR_ENIT_BIT 0x0040

#define MS_PER_S 1000

#define M2_ROLLOVER_DELAY_S 40
#define M2_DOWN_PERIOD_S 5
#define M2_FIRST_DELAY_SECONDS 4
#define M2_SECOND_DELAY_SECONDS 2

static uint32_t readTimer0Reg(uint32_t registerOffset);
static uint32_t readTimer1Reg(uint32_t registerOffset);
static uint32_t readTimer2Reg(uint32_t registerOffset);
static void testUpCounterAll();
static void testDownCounterAll();

// Run milestone 2
int main() {
  printf("=============== Starting milestone 2 ===============\n");

  printf("******************************************\n");
  printf("***** Testing UP Counter for All Timers **\n");
  printf("******************************************\n");
  testUpCounterAll();

  printf("******************************************\n");
  printf("***** Testing Down Counter & Interrupts for All Timers \n");
  printf("******************************************\n");
  testDownCounterAll();

  printf("=============== Done milestone 2 ===============\n");
}

// Reads the timer0 registers based upon the offset.
static uint32_t readTimer0Reg(uint32_t registerOffset) {
  uint32_t address = XPAR_AXI_TIMER_0_BASEADDR + registerOffset;
  return Xil_In32(address);
}

// Reads the timer1 registers based upon the offset.
static uint32_t readTimer1Reg(uint32_t registerOffset) {
  uint32_t address = XPAR_AXI_TIMER_1_BASEADDR + registerOffset;
  return Xil_In32(address);
}

// Reads the timer2 registers based upon the offset.
static uint32_t readTimer2Reg(uint32_t registerOffset) {
  uint32_t address = XPAR_AXI_TIMER_2_BASEADDR + registerOffset;
  return Xil_In32(address);
}

// Test UP Counter for all timers
static void testUpCounterAll() {
  printf("Calling `initCountUp` to initialize all the timers.\n");
  intervalTimer_initCountUp(INTERVAL_TIMER_0);
  intervalTimer_initCountUp(INTERVAL_TIMER_1);
  intervalTimer_initCountUp(INTERVAL_TIMER_2);

  printf("Calling `start` to start all the timers.\n");
  intervalTimer_start(INTERVAL_TIMER_0);
  intervalTimer_start(INTERVAL_TIMER_1);
  intervalTimer_start(INTERVAL_TIMER_2);

  printf("Waiting for %d seconds.\n", M2_FIRST_DELAY_SECONDS);
  utils_msDelay(M2_FIRST_DELAY_SECONDS * MS_PER_S);

  printf("`getTotalDurationInSeconds` for timer 0 (should be ~%d): %f\n",
         M2_FIRST_DELAY_SECONDS,
         intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_0));
  printf("`getTotalDurationInSeconds` for timer 1 (should be ~%d): %f\n",
         M2_FIRST_DELAY_SECONDS,
         intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_1));
  printf("`getTotalDurationInSeconds` for timer 2 (should be ~%d): %f\n",
         M2_FIRST_DELAY_SECONDS,
         intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_2));

  printf("Calling `reload` and waiting another %d seconds\n",
         M2_SECOND_DELAY_SECONDS);
  intervalTimer_reload(INTERVAL_TIMER_0);
  intervalTimer_reload(INTERVAL_TIMER_1);
  intervalTimer_reload(INTERVAL_TIMER_2);
  utils_msDelay(M2_SECOND_DELAY_SECONDS * MS_PER_S);
  printf("`getTotalDurationInSeconds` for timer 0 (should be ~%d): %f\n",
         M2_SECOND_DELAY_SECONDS,
         intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_0));
  printf("`getTotalDurationInSeconds` for timer 1 (should be ~%d): %f\n",
         M2_SECOND_DELAY_SECONDS,
         intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_1));
  printf("`getTotalDurationInSeconds` for timer 2 (should be ~%d): %f\n",
         M2_SECOND_DELAY_SECONDS,
         intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_2));

  printf("Calling `stop` and waiting another %d seconds.  Make sure the timer "
         "hasn't changed much.\n",
         M2_SECOND_DELAY_SECONDS);
  intervalTimer_stop(INTERVAL_TIMER_0);
  intervalTimer_stop(INTERVAL_TIMER_1);
  intervalTimer_stop(INTERVAL_TIMER_2);

  printf("`getTotalDurationInSeconds` for timer 0 (should be ~%d): %f\n",
         M2_SECOND_DELAY_SECONDS,
         intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_0));
  printf("`getTotalDurationInSeconds` for timer 1 (should be ~%d): %f\n",
         M2_SECOND_DELAY_SECONDS,
         intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_1));
  printf("`getTotalDurationInSeconds` for timer 2 (should be ~%d): %f\n",
         M2_SECOND_DELAY_SECONDS,
         intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_2));

  printf("Testing cascade mode; calling `start` and then waiting another ~%d "
         "seconds.\n",
         M2_ROLLOVER_DELAY_S);
  intervalTimer_start(INTERVAL_TIMER_0);
  intervalTimer_start(INTERVAL_TIMER_1);
  intervalTimer_start(INTERVAL_TIMER_2);

  utils_msDelay(M2_ROLLOVER_DELAY_S * MS_PER_S);

  printf("`getTotalDurationInSeconds` (should be ~%d): "
         "%f\n",
         M2_SECOND_DELAY_SECONDS + M2_ROLLOVER_DELAY_S,
         intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_0));
  printf("`getTotalDurationInSeconds` (should be ~%d): "
         "%f\n",
         M2_SECOND_DELAY_SECONDS + M2_ROLLOVER_DELAY_S,
         intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_1));
  printf("`getTotalDurationInSeconds` (should be ~%d): "
         "%f\n",
         M2_SECOND_DELAY_SECONDS + M2_ROLLOVER_DELAY_S,
         intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_2));
}

// Test down counter and interrupts for all timers
static void testDownCounterAll() {
  printf("Calling `initCountDown` to initialize all the timers with a %ds "
         "period.\n",
         M2_DOWN_PERIOD_S);
  intervalTimer_initCountDown(INTERVAL_TIMER_0, M2_DOWN_PERIOD_S);
  intervalTimer_initCountDown(INTERVAL_TIMER_1, M2_DOWN_PERIOD_S);
  intervalTimer_initCountDown(INTERVAL_TIMER_2, M2_DOWN_PERIOD_S);

  printf("Verifying interrupt enable (ENIT) bit is 0 after init on all "
         "timers:\n");
  printf("Timer 0 TCSR0 ENIT (should be 0): %d\n",
         (readTimer0Reg(TCSR0_OFFSET) & TCSR_ENIT_BIT) == TCSR_ENIT_BIT);
  printf("Timer 1 TCSR0 ENIT (should be 0): %d\n",
         (readTimer1Reg(TCSR0_OFFSET) & TCSR_ENIT_BIT) == TCSR_ENIT_BIT);
  printf("Timer 2 TCSR0 ENIT (should be 0): %d\n",
         (readTimer2Reg(TCSR0_OFFSET) & TCSR_ENIT_BIT) == TCSR_ENIT_BIT);

  printf("Calling `enableInterrupt` on all timers.\n");
  intervalTimer_enableInterrupt(INTERVAL_TIMER_0);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_1);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_2);

  printf("Verifying ENIT bit is set on all timers:\n");
  printf("Timer 0 TCSR0 ENIT (should be 1): %d\n",
         (readTimer0Reg(TCSR0_OFFSET) & TCSR_ENIT_BIT) == TCSR_ENIT_BIT);
  printf("Timer 1 TCSR0 ENIT (should be 1): %d\n",
         (readTimer1Reg(TCSR0_OFFSET) & TCSR_ENIT_BIT) == TCSR_ENIT_BIT);
  printf("Timer 2 TCSR0 ENIT (should be 1): %d\n",
         (readTimer2Reg(TCSR0_OFFSET) & TCSR_ENIT_BIT) == TCSR_ENIT_BIT);

  printf("Calling `disableInterrupt` on all timers:\n");
  intervalTimer_disableInterrupt(INTERVAL_TIMER_0);
  intervalTimer_disableInterrupt(INTERVAL_TIMER_1);
  intervalTimer_disableInterrupt(INTERVAL_TIMER_2);

  printf("Verifying interrupt enable (ENIT) bit has been cleared on all "
         "timers:\n");
  printf("Timer 0 TCSR0 ENIT (should be 0): %d\n",
         (readTimer0Reg(TCSR0_OFFSET) & TCSR_ENIT_BIT) == TCSR_ENIT_BIT);
  printf("Timer 1 TCSR0 ENIT (should be 0): %d\n",
         (readTimer1Reg(TCSR0_OFFSET) & TCSR_ENIT_BIT) == TCSR_ENIT_BIT);
  printf("Timer 2 TCSR0 ENIT (should be 0): %d\n",
         (readTimer2Reg(TCSR0_OFFSET) & TCSR_ENIT_BIT) == TCSR_ENIT_BIT);

  printf("Calling `start` to start all the timers.\n");
  intervalTimer_start(INTERVAL_TIMER_0);
  intervalTimer_start(INTERVAL_TIMER_1);
  intervalTimer_start(INTERVAL_TIMER_2);

  printf("Waiting for %d seconds.\n", M2_DOWN_PERIOD_S + 1);
  utils_msDelay((M2_DOWN_PERIOD_S + 1) * MS_PER_S);

  printf("Verifying that timers restarted:\n");
  printf("`getTotalDurationInSeconds` (should be ~%d): "
         "%f\n",
         M2_DOWN_PERIOD_S - 1,
         intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_0));
  printf("`getTotalDurationInSeconds` (should be ~%d): "
         "%f\n",
         M2_DOWN_PERIOD_S - 1,
         intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_1));
  printf("`getTotalDurationInSeconds` (should be ~%d): "
         "%f\n",
         M2_DOWN_PERIOD_S - 1,
         intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_2));

  printf("Verifying that rollover occurred:\n");
  printf("Timer 0 TCSR0 INT_BIT (should be 1): %d\n",
         (readTimer0Reg(TCSR0_OFFSET) & TCSR_INT_BIT) == TCSR_INT_BIT);
  printf("Timer 1 TCSR0 INT_BIT (should be 1): %d\n",
         (readTimer1Reg(TCSR0_OFFSET) & TCSR_INT_BIT) == TCSR_INT_BIT);
  printf("Timer 2 TCSR0 INT_BIT (should be 1): %d\n",
         (readTimer2Reg(TCSR0_OFFSET) & TCSR_INT_BIT) == TCSR_INT_BIT);

  printf("Calling `ackInterrupt` on all timers.\n");
  intervalTimer_ackInterrupt(INTERVAL_TIMER_0);
  intervalTimer_ackInterrupt(INTERVAL_TIMER_1);
  intervalTimer_ackInterrupt(INTERVAL_TIMER_2);

  printf(
      "Verifying that rollover/interrupt output was successfully cleared:\n");
  printf("Timer 0 TCSR0 INT_BIT (should be 0): %d\n",
         (readTimer0Reg(TCSR0_OFFSET) & TCSR_INT_BIT) == TCSR_INT_BIT);
  printf("Timer 1 TCSR0 INT_BIT (should be 0): %d\n",
         (readTimer1Reg(TCSR0_OFFSET) & TCSR_INT_BIT) == TCSR_INT_BIT);
  printf("Timer 2 TCSR0 INT_BIT (should be 0): %d\n",
         (readTimer2Reg(TCSR0_OFFSET) & TCSR_INT_BIT) == TCSR_INT_BIT);
}
