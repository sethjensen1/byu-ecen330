#include "intervalTimer.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdint.h>
#include <stdio.h>

#define TCSR0_OFFSET 0
#define TCSR1_OFFSET 0x10
#define TLR0_OFFSET 0x04
#define TLR1_OFFSET 0x14
#define TCR0_OFFSET 0x08
#define TCR1_OFFSET 0x18

#define TCSR0_CASC_MASK 0x800
#define TCSR0_UDT0_MASK 0x2
#define TCSR0_LOAD0_MASK 0x20
#define TCSR0_ENT0_MASK 0x80
#define TCSR0_ARHT0_MASK 0x10
#define TCSR0_ENIT0_MASK 0x40
#define TCSR0_INT0_MASK 0x100

#define TCSR1_ENIT1_MASK 0x40

#define TCSR1_LOAD1_MASK 0x20
#define TCSR1_ENT1_MASK 0x80

// Assuming XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ = 100000000
#define CYCLE_DURATION 0.000000010

// returns: the correct base address for the given timer number
static uint32_t getTimerBaseAddress(uint8_t timerNumber) {
  uint32_t baseAddress;

  // Choose the correct parameter for the given timer number
  switch (timerNumber) {
  case INTERVAL_TIMER_0:
    baseAddress = XPAR_AXI_TIMER_0_BASEADDR;
    break;
  case INTERVAL_TIMER_1:
    baseAddress = XPAR_AXI_TIMER_1_BASEADDR;
    break;
  case INTERVAL_TIMER_2:
    baseAddress = XPAR_AXI_TIMER_2_BASEADDR;
    break;
  }

  return baseAddress;
}

// Helper function to read from the timer register, given a timer number and the
// register's offset
// returns: value at the register defined above
static uint32_t readRegister(uint8_t timerNumber, uint32_t offset) {
  return Xil_In32(getTimerBaseAddress(timerNumber) + offset);
}

// Helper function to write to the timer register, given a timer number and the
// register's offset
static void writeRegister(uint8_t timerNumber, uint32_t offset,
                          uint32_t value) {
  Xil_Out32(getTimerBaseAddress(timerNumber) + offset, value);
}

// Initialize the timer given by timerNumber by setting it to 64-bit cascade
// mode, counting up, and resetting the load registers
void intervalTimer_initCountUp(uint32_t timerNumber) {
  // 1. Set the Timer Control/Status Registers such that:
  //  - The timer is in 64-bit cascade mode
  //  - The timer counts up
  writeRegister(timerNumber, TCSR0_OFFSET, 0);
  writeRegister(timerNumber, TCSR1_OFFSET, 0);

  // Set the CASC bit to 1, and leave the UDT0 bit at 0, making the timer count
  // up
  writeRegister(timerNumber, TCSR0_OFFSET, TCSR0_CASC_MASK);

  // 2. Initialize both LOAD registers with zeros
  writeRegister(timerNumber, TLR0_OFFSET, 0);
  writeRegister(timerNumber, TLR1_OFFSET, 0);

  // 3. Call the _reload function to move the LOAD values into the Counters
  intervalTimer_reload(timerNumber);
}

// Load the values of TLR0 and TLR1 from LOAD0 and LOAD1 into TCR0 and TCR1,
// respectively
void intervalTimer_reload(uint32_t timerNumber) {
  // Set the LOAD0 and LOAD1 bits to 1 in their respective control registers,
  // enabling loading from TLR0 and TLR1
  writeRegister(timerNumber, TCSR0_OFFSET,
                readRegister(timerNumber, TCSR0_OFFSET) | TCSR0_LOAD0_MASK);
  writeRegister(timerNumber, TCSR1_OFFSET,
                readRegister(timerNumber, TCSR1_OFFSET) | TCSR1_LOAD1_MASK);

  // Set the LOAD0 and LOAD1 bits to 0 in their respective control registers,
  // finishing loading from TLR0 and TLR1
  writeRegister(timerNumber, TCSR0_OFFSET,
                readRegister(timerNumber, TCSR0_OFFSET) & ~TCSR0_LOAD0_MASK);
  writeRegister(timerNumber, TCSR1_OFFSET,
                readRegister(timerNumber, TCSR1_OFFSET) & ~TCSR1_LOAD1_MASK);
}

// If the timer 0 and timer 1 aren't running, then start them
void intervalTimer_start(uint32_t timerNumber) {
  writeRegister(timerNumber, TCSR0_OFFSET,
                (readRegister(timerNumber, TCSR0_OFFSET)) | TCSR0_ENT0_MASK);
}

// Stops a running interval timer if If the interval time is not currently
// stopped
// timerNumber indicates which timer should stop running.
void intervalTimer_stop(uint32_t timerNumber) {
  uint32_t TCSR0Value = readRegister(timerNumber, TCSR0_OFFSET);
  if (TCSR0Value & TCSR0_ENT0_MASK)
    writeRegister(timerNumber, TCSR0_OFFSET, TCSR0Value & ~TCSR0_ENT0_MASK);

  uint32_t TCSR1Value = readRegister(timerNumber, TCSR1_OFFSET);
  if (TCSR1Value & TCSR1_ENT1_MASK)
    writeRegister(timerNumber, TCSR1_OFFSET, TCSR1Value & ~TCSR1_ENT1_MASK);
}

// Use this function to ascertain how long a given timer has been running.
// Note that it should not be an error to call this function on a running timer
// though it usually makes more sense to call this after intervalTimer_stop()
// has been called. The timerNumber argument determines which timer is read.
// In cascade mode you will need to read the upper and lower 32-bit registers,
// concatenate them into a 64-bit counter value, and then perform the conversion
// to a double seconds value.
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber) {
  uint64_t timerDuration = readRegister(timerNumber, TCR1_OFFSET);
  timerDuration <<= 32;
  timerDuration += readRegister(timerNumber, TCR0_OFFSET);

  return timerDuration * CYCLE_DURATION;
}

// Initialize the timer given by timerNumber by setting it to 64-bit cascade
// mode, counting down from the `period` in seconds, reloading when it hits 0,
// and resetting the load registers
void intervalTimer_initCountDown(uint32_t timerNumber, double period) {
  // 1. Set the Timer Control/Status Registers such that:
  //  - The timer is in 64-bit cascade mode
  //  - The timer counts down
  //  - The timer automatically reloads when reaching zero
  writeRegister(timerNumber, TCSR0_OFFSET, 0);
  writeRegister(timerNumber, TCSR1_OFFSET, 0);

  // Set the CASC bit to 1, and set the UDT0 bit to 1, making the timer count
  // down
  writeRegister(timerNumber, TCSR0_OFFSET,
                0 | TCSR0_CASC_MASK | TCSR0_UDT0_MASK | TCSR0_ARHT0_MASK);

  // 2. Initialize LOAD registers with appropriate values, given the `period`.
  // Multiply the period by the Hz of the clock to get number of clock cycles
  uint64_t durationInCycles = (uint64_t)period * XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ;
  uint32_t upper = durationInCycles >> 32;
  uint32_t lower = durationInCycles;

  writeRegister(timerNumber, TLR0_OFFSET, lower);
  writeRegister(timerNumber, TLR1_OFFSET, upper);

  // 3. Call the _reload function to move the LOAD values into the Counters
  intervalTimer_reload(timerNumber);
}

// Enable the interrupt output of the given timer.
void intervalTimer_enableInterrupt(uint8_t timerNumber) {
  writeRegister(timerNumber, TCSR0_OFFSET,
                readRegister(timerNumber, TCSR0_OFFSET) | TCSR0_ENIT0_MASK);
  // do i need to enable the interrupt for timer 1? I don't think so
}

// Disable the interrupt output of the given timer.
void intervalTimer_disableInterrupt(uint8_t timerNumber) {
  writeRegister(timerNumber, TCSR0_OFFSET,
                readRegister(timerNumber, TCSR0_OFFSET) & ~TCSR0_ENIT0_MASK);
}

// Acknowledge the rollover to clear the interrupt output.
void intervalTimer_ackInterrupt(uint8_t timerNumber) {
  writeRegister(timerNumber, TCSR0_OFFSET,
                readRegister(timerNumber, TCSR0_OFFSET) | TCSR0_INT0_MASK);
}