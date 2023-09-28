#include "interrupts.h"
#include "armInterrupts.h"
#include "xil_io.h"
#include "xparameters.h"

#define MER_OFFSET 0x1C
#define MER_IRQ_ENABLE_MASK 0x03

#define CIE_OFFSET 0x14
#define CLEAR_ALL 0xFF

#define SIE_OFFSET 0x10

#define IPR_OFFSET 0x04

#define IAR_OFFSET 0x0C
#define NUM_INTERRUPT_INPUTS 3

static void (*isrFcnPtrs[NUM_INTERRUPT_INPUTS])() = {NULL};

// Helper function to read from the timer register, given the register's offset
// returns: value at the register defined above
static uint32_t readRegister(uint32_t offset) {
  return Xil_In32(XPAR_AXI_INTC_0_BASEADDR + offset);
}

// Helper function to write to the timer register, given the register's offset
static void writeRegister(uint32_t offset, uint32_t value) {
  Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + offset, value);
}

// ISR callback
static void interrupts_isr() {
  // Loop through each interrupt input
  for (uint8_t i = 0; i < NUM_INTERRUPT_INPUTS; i++) {

    // Check if it has an interrupt pending
    if (readRegister(IPR_OFFSET) & (1 << i)) {

      // Check if there is a callback
      if (isrFcnPtrs[i] != NULL) {
        // Call the callback function
        isrFcnPtrs[i]();
      }

      // Acknowledge interrupt
      writeRegister(IAR_OFFSET, (1 << i));
    }
  }
}

// Initialize interrupt hardware
// This function should:
// 1. Configure AXI INTC registers to:
//  - Enable interrupt output (see Master Enable Register)
//  - Disable all interrupt input lines.
// 2. Enable the Interrupt system on the ARM processor, and register an ISR
// handler function. This is done by calling:
//  - armInterrupts_init()
//  - armInterrupts_setupIntc(isr_fcn_ptr)
//  - armInterrupts_enable()
void interrupts_init() {
  writeRegister(MER_OFFSET, MER_IRQ_ENABLE_MASK);
  interrupts_irq_disable(0xFF);
  // writeRegister(CIE_OFFSET, CLEAR_ALL);
  armInterrupts_init();
  armInterrupts_setupIntc(interrupts_isr);
  armInterrupts_enable();
}

// Register a callback function (fcn is a function pointer to this callback
// function) for a given interrupt input number (irq).  When this interrupt
// input is active, fcn will be called.
void interrupts_register(uint8_t irq, void (*fcn)()) { isrFcnPtrs[irq] = fcn; }

// Enable single input interrupt line, given by irq number.
void interrupts_irq_enable(uint8_t irq) { writeRegister(SIE_OFFSET, 1 << irq); }

// Disable single input interrupt line, given by irq number.
void interrupts_irq_disable(uint8_t irq) {
  writeRegister(CIE_OFFSET, 1 << irq);
}