#ifndef EGGMANCONTROL
#define EGGMANCONTROL

// Tick the eggman controller state machine
void eggmanControl_tick();

// Initialize the eggman controller state machine,
// providing the tick period, in seconds.
void eggmanControl_init(double period_s);

#endif /* EGGMANCONTROL */
