//Arduino M0 Timer Interrupt Library
//2018 Michele Stanchina

#ifndef __TIMER_INT_M0__
#define __TIMER_INT_M0__

#include <Arduino.h>

//Definitions
#define GCLK_CH         0x4
#define SRC_OSCULP32K   0x3

typedef void(*voidFuncPtr)(void);

class M0TimerInterrupt{
private:
  //Init GCLK4
  void initTimerGclk(void);
  //Init TC4
  void initTC4Int(void);
  //Init TC5
  void initTC5Int(void);

public:
  //Class Constructor
  void begin();

  //Timer 4 Interrupt methods
  void attachTC4InterruptMs(uint16_t time_ms, voidFuncPtr callback);
  void attachTC4InterruptS(uint16_t time_s, voidFuncPtr callback);
  void detachTC4Interrupt();

  //Timer 5 Interrupt methods
  void attachTC5InterruptMs(uint16_t time_ms, voidFuncPtr callback);
  void attachTC5InterruptS(uint16_t time_s, voidFuncPtr callback);
  void detachTC5Interrupt();

  //Standby method
  void standby(void);
};

#endif
