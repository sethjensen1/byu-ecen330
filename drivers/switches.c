#include "switches.h"
#include "xil_io.h"
#include "xparameters.h"

#define DEFAULT_REGISTER_VALUE 0x000F
#define TRISTATE_OFFSET 0x0004
#define TRISTATE_OFF 0x000F

// Helper function to read from the switches register
static uint32_t readRegister(uint32_t offset) {
  return Xil_In32(XPAR_SLIDE_SWITCHES_BASEADDR + offset);
}

// Helper function to write to the switches register
static void writeRegister(uint32_t offset, uint32_t value) {
  Xil_Out32(XPAR_SLIDE_SWITCHES_BASEADDR + offset, value);
}

// Initializes the SWITCHES driver software and hardware by writing default
// values to the switch register
void switches_init() {
  writeRegister(TRISTATE_OFFSET, TRISTATE_OFF);
  writeRegister(0, DEFAULT_REGISTER_VALUE);
}

// Returns the current value of all 4 switches as the lower 4 bits of the
// returned value. bit3 = SW3, bit2 = SW2, bit1 = SW1, bit0 = SW0.
uint8_t switches_read() { readRegister(0); }