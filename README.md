# Arduino M0 Timer Interrupt Library.
A simple library for manage Timer Interrupts on Arduino M0

Tested on Adafruit Feather M0, not tested on Arduino M0.
Used Peripherials: GCKL4 with OSCULP32K, TC4 and TC5.

# Description.
With this library is possible to configure and use two different timer interrupts, from a minimum time of 2 milliseconds to a max time of 512 seconds. Is also possible to put the CPU in Sleep mode and wake up when an interrupt occours.
