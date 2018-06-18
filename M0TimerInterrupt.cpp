//Arduino M0 Timer Interrupt Library
//2018 Michele Stanchina

#include "M0TimerInterrupt.h"

voidFuncPtr TC4Callback=NULL;
voidFuncPtr TC5Callback=NULL;

//Configure GCLK4
void M0TimerInterrupt::initTimerGclk(void){
  //Setup Generic CLK CH4
  //feed the TC4-TC5 with a source clock of 32.786 Hz / clk_div
  //Clock divider register config
  GCLK->GENDIV.reg = GCLK_CH                      //Select which channel of GCLK to use
                   | GCLK_GENDIV_DIV(0);          //Set the clock divider to 0
  while(GCLK->STATUS.bit.SYNCBUSY);               //Wait sync

  //Control register config
  GCLK->GENCTRL.reg = GCLK_CH                      //Select which channel of GCLK to use
                    | GCLK_GENCTRL_GENEN           //Enable channel
                    | GCLK_GENCTRL_SRC_OSCULP32K   //Use 32kHz ULP Oscillator as a CLK source
                    | GCLK_GENCTRL_IDC;            //Improve Duty Cycle
  while(GCLK->STATUS.bit.SYNCBUSY);                //Wait sync

  //Clock control register config
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN(GCLK_CH)       //Select which channel of GCLK to use
                    | GCLK_CLKCTRL_ID_TC4_TC5         //Feed TC4 and TC5
                    | GCLK_CLKCTRL_CLKEN;             //Enable GCLK CH4
  while (GCLK->STATUS.bit.SYNCBUSY);                  //Wait Sync
}

//Init TC4 and configure interrupt
void M0TimerInterrupt::initTC4Int(void){
  //Setup Timer Counter 4.
  //16 bit mode, match compare interrupt.
  //Control A register config
  TC4->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16    //16 bit mode
                         | TC_CTRLA_PRESCALER_DIV256//Prescaler 256
                         | TC_CTRLA_ENABLE          //Enable TC4
                         | TC_CTRLA_RUNSTDBY;       //Run in standby!
  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);         //Wait sync

  //Compare value register set
  TC4->COUNT16.CC[0].reg = TC_COUNT16_CC_CC(0);           //Set the Compare value to zero
  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);               //Wait Sync

  //Stop the counter.
  TC4->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_STOP;     //Stop the counter

  //Setup Interrupts
  NVIC_SetPriority(TC4_IRQn, 0);                //Set the Nested Vector Interrupt Controller (NVIC) priority for TC4 to 0 (highest)
  TC4->COUNT16.INTFLAG.reg |= TC_INTFLAG_MC0;   //Clear the interrupt flag
  TC4->COUNT16.INTENSET.reg = TC_INTENSET_MC0;  //Enable TC4 interrupt
}

//Init TC5 and configure interrupt
void M0TimerInterrupt::initTC5Int(void){
  //Setup Timer Counter 5
  //16 bit mode, match compare interrupt.
  //Control A register config
  TC5->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16    //16 bit mode
                         | TC_CTRLA_PRESCALER_DIV256//Prescaler 256
                         | TC_CTRLA_ENABLE          //Enable TC5
                         | TC_CTRLA_RUNSTDBY;       //Run in standby!
  while (TC5->COUNT16.STATUS.bit.SYNCBUSY);         //Wait sync

  //Compare value register set
  TC5->COUNT16.CC[0].reg = TC_COUNT16_CC_CC(0);           //Set the Compare value to 0
  while (TC5->COUNT16.STATUS.bit.SYNCBUSY);               //Wait Sync

  //Stop the counter.
  TC5->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_STOP;     //Stop the counter

  //Setup Interrupts
  NVIC_SetPriority(TC5_IRQn, 0);                //Set the Nested Vector Interrupt Controller (NVIC) priority for TC5 to 0 (highest)
  TC5->COUNT16.INTFLAG.reg |= TC_INTFLAG_MC0;   //Clear the interrupt flag
  TC5->COUNT16.INTENSET.reg = TC_INTENSET_MC0;  //Enable TC5 interrupt
}

//Init the GCLK and Timers
void M0TimerInterrupt::begin(void){
  initTimerGclk();
  initTC4Int();
  initTC5Int();
}

//TC4 interrupt methods
void M0TimerInterrupt::attachTC4InterruptMs(uint16_t time_ms, voidFuncPtr callback){
  //First step: set Prescaler to 256 and calculate CC0 register value
  uint16_t cc0v = round((time_ms * 128) / 1000) - 1;
  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);   //Wait sync
  TC4->COUNT16.CC[0].reg = TC_COUNT16_CC_CC(cc0v);
  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);   //Wait Sync

  //Set the function pointer to callback function
  TC4Callback=callback;

  //Trigger the counter and enable Interrupt Request
  TC4->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;    //Retrigger the counter
  NVIC_EnableIRQ(TC4_IRQn);                                 //Enable IRQ
}

void M0TimerInterrupt::attachTC4InterruptS(uint16_t time_s, voidFuncPtr callback){
  //First step: set Prescaler to 1024 and calculate CC0 register value
  uint16_t cc0v = (time_s * 128) - 1;
  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);   //Wait sync
  TC4->COUNT16.CC[0].reg = TC_COUNT16_CC_CC(cc0v);
  while (TC4->COUNT16.STATUS.bit.SYNCBUSY);   //Wait Sync

  //Set the function pointer to callback function
  TC4Callback=callback;

  //Trigger the counter and enable IRQ
  TC4->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;    //Retrigger the counter
  NVIC_EnableIRQ(TC4_IRQn);
}

void M0TimerInterrupt::detachTC4Interrupt(){
  //Put function pointer to NULL
  TC4Callback=NULL;

  //Stop Counter and disable IRQ
  TC4->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_STOP;   //Stop the counter
  NVIC_ClearPendingIRQ(TC4_IRQn);                     //Disable IRQ
  NVIC_DisableIRQ(TC4_IRQn);
}

//TC5 interrupt methods
void M0TimerInterrupt::attachTC5InterruptMs(uint16_t time_ms, voidFuncPtr callback){
  //First step: set Prescaler to 256 and calculate CC0 register value
  uint16_t cc0v = round((time_ms * 128) / 1000) - 1;
  while (TC5->COUNT16.STATUS.bit.SYNCBUSY);   //Wait sync
  TC5->COUNT16.CC[0].reg = TC_COUNT16_CC_CC(cc0v);
  while (TC5->COUNT16.STATUS.bit.SYNCBUSY);   //Wait Sync

  //Set the function pointer to callback function
  TC5Callback=callback;

  //Trigger the counter and enable Interrupt Request
  TC5->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;    //Retrigger the counter
  NVIC_EnableIRQ(TC5_IRQn);                                 //Enable IRQ
}

void M0TimerInterrupt::attachTC5InterruptS(uint16_t time_s, voidFuncPtr callback){
  //First step: set Prescaler to 1024 and calculate CC0 register value
  uint16_t cc0v = (time_s * 128) - 1;
  while (TC5->COUNT16.STATUS.bit.SYNCBUSY);   //Wait sync
  TC5->COUNT16.CC[0].reg = TC_COUNT16_CC_CC(cc0v);
  while (TC5->COUNT16.STATUS.bit.SYNCBUSY);   //Wait Sync

  //Set the function pointer to callback function
  TC5Callback=callback;

  //Trigger the counter and enable IRQ
  TC5->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;    //Retrigger the counter
  NVIC_EnableIRQ(TC5_IRQn);
}

void M0TimerInterrupt::detachTC5Interrupt(){
  //Put function pointer to NULL
  TC5Callback=NULL;

  //Stop Counter and disable IRQ
  TC5->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_STOP;   //Stop the counter
  NVIC_ClearPendingIRQ(TC5_IRQn);                     //Disable IRQ
  NVIC_DisableIRQ(TC5_IRQn);
}

//Sleep method
void M0TimerInterrupt::standby(){
  //Go to Sleep and wait for an Interrupt
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  __WFI();
}

//TC Interrupt Handlers
void TC4_Handler(){
    //Call the function
    if(TC4Callback!=NULL){
      TC4Callback();
    }

    TC4->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;  //Retrigger Counter
    TC4->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0;              //Clear Interrupt Flag
}

void TC5_Handler(){
    //Call Function
    if(TC5Callback!=NULL){
      TC5Callback();
    }

    TC5->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;  //Retrigger Counter
    TC5->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0;              //Clear Interrupt Flag
}
