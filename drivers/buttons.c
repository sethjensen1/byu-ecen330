#include "buttons.h"
#include "xil_io.h"
#include "xparameters.h"

// initialize all buttons to be off, tri-state drivers to be off
#define DEFAULT_BUTTONS_VALUE 0x0003
#define TRISTATE_OFFSET 4
#define TRISTATE_OFF 0xF

// Helper function to read from the push button register
static uint32_t readRegister(uint32_t offset) {
  return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR + offset);
}

// Helper function to write to the push button register
static void writeRegister(uint32_t offset, uint32_t value) {
  Xil_Out32(XPAR_PUSH_BUTTONS_BASEADDR + offset, value);
}

// Initializes the buttons to their default values
void buttons_init() {
  writeRegister(TRISTATE_OFFSET, TRISTATE_OFF);
  writeRegister(0, DEFAULT_BUTTONS_VALUE);
}

// Returns the current value of all 4 buttons as the lower 4 bits of the
// returned value. bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
uint8_t buttons_read() { return readRegister(0); }