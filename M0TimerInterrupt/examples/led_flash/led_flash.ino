#include "M0TimerInterrupt.h"

M0TimerInterrupt TInt;

/* Timer Interrupt Example:
 *  Use the TC4 and TC5 to generate an interrupt every second.
 *  Wake the CPU every time an interrupt occours.
 *  The internal led will flash.
 *	2018 Michele "Migui" Stanchina.
*/

const int led = 13;
const int timeSec = 1;
const int timeMsec = 1000;

//Callback function
void callback(){
  digitalWrite(led, !digitalRead(led));  
}

void setup() {
  pinMode(led,OUTPUT);
  //Init the timer interrupts
  TInt.begin();
  //Attach an Interrupt every timeSec seconds, max time 512 sec.
  //Call the callback function
  TInt.attachTC4InterruptS(timeSec,callback);
  delay(500);
  //Is also possible to run another timer interrupt on TC5 while the one in TC4 is running.
  //Using Ms the resolution is 2 ms (is rounded by the method internally to nearest value).
  //The max time is 512000 millis.
  TInt.attachTC5InterruptMs(timeMsec,callback);
}

void loop() {
  //Put the CPU in Standby Mode and wait for an Interrupt
  TInt.standby();
  //Use the method detachTCXInterrupt (X=4,5) for detach the interrupt
}

