#include "interrupt_test.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "leds.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdio.h>

#define PERIOD_10HZ 0.1
#define PERIOD_1HZ 1
#define PERIOD_P1HZ 10

#define LED_0 0x01
#define LED_1 0x02
#define LED_2 0x04

// Timer 0 callback
void isr_timer0() {
  leds_write(leds_read() ^ LED_0);
  intervalTimer_ackInterrupt(XPAR_AXI_TIMER_0_DEVICE_ID);
}

// Timer 1 callback
void isr_timer1() {
  leds_write(leds_read() ^ LED_1);
  intervalTimer_ackInterrupt(XPAR_AXI_TIMER_1_DEVICE_ID);
}

// Timer 2 callback
void isr_timer2() {
  leds_write(leds_read() ^ LED_2);
  intervalTimer_ackInterrupt(XPAR_AXI_TIMER_2_DEVICE_ID);
}

/*
This function is a small test application of your interrupt controller.  The
goal is to use the three AXI Interval Timers to generate interrupts at different
rates (10Hz, 1Hz, 0.1Hz), and create interrupt handler functions that change the
LEDs at this rate.  For example, the 1Hz interrupt will flip an LED value each
second, resulting in LED that turns on for 1 second, off for 1 second,
repeatedly.

For each interval timer:
    1. Initialize it as a count down timer with appropriate period.
    2. Enable the timer's interrupt output.
    3. Enable the associated interrupt input on the interrupt controller.
    4. Register an appropriate interrupt handler function (isr_timer0,
isr_timer1, isr_timer2).
    5. Start the timer.

Make sure you call `interrupts_init()` first!
*/
void interrupt_test_run() {
  interrupts_init();
  leds_init();

  intervalTimer_initCountDown(XPAR_AXI_TIMER_0_DEVICE_ID, PERIOD_10HZ);
  intervalTimer_initCountDown(XPAR_AXI_TIMER_1_DEVICE_ID, PERIOD_1HZ);
  intervalTimer_initCountDown(XPAR_AXI_TIMER_2_DEVICE_ID, PERIOD_P1HZ);

  intervalTimer_enableInterrupt(XPAR_AXI_TIMER_0_DEVICE_ID);
  intervalTimer_enableInterrupt(XPAR_AXI_TIMER_1_DEVICE_ID);
  intervalTimer_enableInterrupt(XPAR_AXI_TIMER_2_DEVICE_ID);

  interrupts_irq_enable(INTERVAL_TIMER_0_INTERRUPT_IRQ);
  interrupts_irq_enable(INTERVAL_TIMER_1_INTERRUPT_IRQ);
  interrupts_irq_enable(INTERVAL_TIMER_2_INTERRUPT_IRQ);

  interrupts_register(INTERVAL_TIMER_0_INTERRUPT_IRQ, isr_timer0);
  interrupts_register(INTERVAL_TIMER_1_INTERRUPT_IRQ, isr_timer1);
  interrupts_register(INTERVAL_TIMER_2_INTERRUPT_IRQ, isr_timer2);

  intervalTimer_start(XPAR_AXI_TIMER_0_DEVICE_ID);
  intervalTimer_start(XPAR_AXI_TIMER_1_DEVICE_ID);
  intervalTimer_start(XPAR_AXI_TIMER_2_DEVICE_ID);

  while (1)
    ;
}