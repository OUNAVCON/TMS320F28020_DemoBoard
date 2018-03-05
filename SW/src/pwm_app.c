/*
 * pwm_app.c
 *
 *  Created on: Mar 5, 2018
 *      Author: Isaac.Rose
 */
#include <DSP28x_Project.h>
#include "pwm_app.h"
#include "pwm.h"
#include "clk.h"
#include "gpio.h"
#include "pie.h"


static void Config_PWM(uint16_t period, PWM_Handle *pwm);

extern CLK_Handle myClk;
extern PIE_Handle myPie;
extern GPIO_Handle myGpio;
extern CPU_Handle myCpu;
extern PIE_Handle myPie;

PWM_Handle myPwm4;

void init_pwm(){

/*  myPwm1 = PWM_init((void *)PWM_ePWM1_BASE_ADDR, sizeof(PWM_Obj));
  myPwm2 = PWM_init((void *)PWM_ePWM2_BASE_ADDR, sizeof(PWM_Obj));
  myPwm3 = PWM_init((void *)PWM_ePWM3_BASE_ADDR, sizeof(PWM_Obj));*/

  myPwm4 = PWM_init((void *)PWM_ePWM4_BASE_ADDR, sizeof(PWM_Obj));

  //Setup GPIO
/*  GPIO_setPullUp(myGpio, GPIO_Number_0, GPIO_PullUp_Disable);
  GPIO_setPullUp(myGpio, GPIO_Number_1, GPIO_PullUp_Disable);
  GPIO_setMode(myGpio, GPIO_Number_0, GPIO_0_Mode_EPWM1A);
  GPIO_setMode(myGpio, GPIO_Number_1, GPIO_1_Mode_EPWM1B);

  GPIO_setPullUp(myGpio, GPIO_Number_2, GPIO_PullUp_Disable);
  GPIO_setPullUp(myGpio, GPIO_Number_3, GPIO_PullUp_Disable);
  GPIO_setMode(myGpio, GPIO_Number_2, GPIO_2_Mode_EPWM2A);
  GPIO_setMode(myGpio, GPIO_Number_3, GPIO_3_Mode_EPWM2B);

  GPIO_setPullUp(myGpio, GPIO_Number_4, GPIO_PullUp_Disable);
  GPIO_setPullUp(myGpio, GPIO_Number_5, GPIO_PullUp_Disable);
  GPIO_setMode(myGpio, GPIO_Number_4, GPIO_4_Mode_EPWM3A);
  GPIO_setMode(myGpio, GPIO_Number_5, GPIO_5_Mode_EPWM3B);*/

  GPIO_setPullUp(myGpio, GPIO_Number_6, GPIO_PullUp_Disable);
  GPIO_setPullUp(myGpio, GPIO_Number_7, GPIO_PullUp_Disable);
  GPIO_setMode(myGpio, GPIO_Number_6, GPIO_6_Mode_EPWM4A);
  GPIO_setMode(myGpio, GPIO_Number_7, GPIO_7_Mode_EPWM4B);

  //Enable Clock
//  CLK_enablePwmClock(myClk, PWM_Number_1);
//  CLK_enablePwmClock(myClk, PWM_Number_2);
//  CLK_enablePwmClock(myClk, PWM_Number_3);
  CLK_enablePwmClock(myClk, PWM_Number_4);
  CLK_enableTbClockSync(myClk);

  //Enable ADC on PWM


  //
  // Setup PWM
  //
  PWM_enableSocAPulse(myPwm4);                     // Enable SOC on A group
//  PWM_enableSocBPulse(myPwm1);

  PWM_setSocBPulseSrc(myPwm4, PWM_SocPulseSrc_CounterEqualZero);
  PWM_setSocBPeriod(myPwm4, PWM_SocPeriod_FirstEvent);

  //Enable PWM
//  Config_PWM(PWM_PERIOD, &myPwm1);
//  Config_PWM(PWM_PERIOD, &myPwm2);
//  Config_PWM(PWM_PERIOD, &myPwm3);
  Config_PWM(PWM_PERIOD, &myPwm4);
 // CLK_enablePwmClock(myClk, PWM_Number_1);
 // CLK_enablePwmClock(myClk, PWM_Number_2);
  CLK_enablePwmClock(myClk, PWM_Number_4);

/*    PWM_setIntMode(myPwm1, PWM_IntMode_CounterEqualZero);
  PWM_setIntPeriod(myPwm1, PWM_IntPeriod_FirstEvent);
  PWM_enableInt(myPwm1);
  PWM_clearIntFlag(myPwm1);
  PIE_registerPieIntHandler(myPie, PIE_GroupNumber_3, PIE_SubGroupNumber_1,
                            (intVec_t) & pwm_isr);
  PIE_enablePwmInt(myPie, PWM_Number_1);
  CPU_enableInt(myCpu, CPU_IntNumber_3);*/
  setPwm(PWM_PERIOD>>1);
}

static void Config_PWM(uint16_t period, PWM_Handle *pwm)
{
  PWM_setPeriodLoad(*pwm, PWM_PeriodLoad_Shadow);
  PWM_setPeriod(*pwm, period-1);        // Set timer period
  PWM_setCmpA(*pwm, MAX_PWM_PERIOD);
  PWM_setCmpAHr(*pwm, (1 << 8));
  PWM_setCmpB(*pwm, MAX_PWM_PERIOD);
  PWM_setPhase(*pwm, 0x0000);           // Phase is 0
  PWM_setCount(*pwm, 0x0000);           // Clear counter

  PWM_setCounterMode(*pwm, PWM_CounterMode_UpDown);   // Count up then down.
  PWM_enableCounterLoad(*pwm);                   // Disable phase loading
  PWM_setSyncMode(*pwm, PWM_SyncMode_Disable);

  //
  // Clock ratio to SYSCLKOUT
  //
  PWM_setHighSpeedClkDiv(*pwm, PWM_HspClkDiv_by_8);

  PWM_setClkDiv(*pwm, PWM_ClkDiv_by_64);
  PWM_setDeadBandOutputMode(*pwm, PWM_DeadBandOutputMode_EPWMxA_Rising_EPWMxB_Falling);
  PWM_setDeadBandPolarity(*pwm, PWM_DeadBandPolarity_EPWMxB_Inverted);
  PWM_setDeadBandFallingEdgeDelay(*pwm, 1);
  PWM_setDeadBandRisingEdgeDelay(*pwm, 1);
  //
  // Load registers every ZERO
  //
  //PWM_setRunMode(*pwm, PWM_RunMode_FreeRun);
  PWM_setShadowMode_CmpA(*pwm, PWM_ShadowMode_Shadow);
  PWM_setShadowMode_CmpB(*pwm, PWM_ShadowMode_Shadow);

  PWM_setLoadMode_CmpA(*pwm, PWM_LoadMode_Zero);
  PWM_setLoadMode_CmpB(*pwm, PWM_LoadMode_Zero);

//    PWM_setActionQual_Zero_PwmA(*pwm, PWM_ActionQual_Clear);
  PWM_setActionQual_CntUp_CmpA_PwmA(*pwm, PWM_ActionQual_Set);
  PWM_setActionQual_CntDown_CmpA_PwmA(*pwm, PWM_ActionQual_Clear);

//    PWM_setActionQual_Zero_PwmB(*pwm, PWM_ActionQual_Set);
  PWM_setActionQual_CntUp_CmpA_PwmB(*pwm, PWM_ActionQual_Clear);
  PWM_setActionQual_CntDown_CmpA_PwmB(*pwm, PWM_ActionQual_Set);

  PWM_setHrEdgeMode(*pwm, PWM_HrEdgeMode_Both);
  PWM_setHrControlMode(*pwm, PWM_HrControlMode_Duty);
  PWM_setHrShadowMode(*pwm, PWM_HrShadowMode_CTR_EQ_0);
}


void setPwm(uint16_t dc1){
/*  PWM_setCmpA(myPwm1, dc1);
  PWM_setCmpA(myPwm2, dc1);
  PWM_setCmpA(myPwm3, dc1);*/
  PWM_setCmpA(myPwm4, dc1);
}
/*

void setMotorState(MOTOR_STATE newState){
  state = newState;
}

MOTOR_STATE getMotorState(void){
  return state;
}

void setHallState(uint16_t state){
  hallState = state;
}
*/

__interrupt void pwm_isr(void){
  PWM_clearIntFlag(myPwm4);
  PIE_clearInt(myPie, PIE_GroupNumber_3);
}
/*
void setMotorDirection(MOTOR_DIRECTION dir){
  direction = dir;
}

MOTOR_DIRECTION getMotorDirection(void){
  return direction;
}

void commutate(){
  hallState = readHallState();
  if(MOTOR_STOP == state){
      hallState = 0x00;
  }
  if(REVERSE == direction){
      hallState = (~hallState) & 0x7;
  }
  switch(hallState){
  case 0x6:
 //Active High
//        myPwm1->AQCSFRC = 0x0000;
//        myPwm2->AQCSFRC = 0x0005;
//        myPwm3->AQCSFRC = 0x0009;
      activePWM(&myPwm1);
      setPWMOff(&myPwm2);
      setPWMOutLow(&myPwm3);
      break;
  case 0x4:
      myPwm1->AQCSFRC = 0x0000;
      myPwm2->AQCSFRC = 0x0009;
      myPwm3->AQCSFRC = 0x0005;
      activePWM(&myPwm1);
      setPWMOutLow(&myPwm2);
      setPWMOff(&myPwm3);
      break;
  case 0x5:
        myPwm1->AQCSFRC = 0x0005;
      myPwm2->AQCSFRC = 0x0009;
      myPwm3->AQCSFRC = 0x0000;
      setPWMOff(&myPwm1);
      setPWMOutLow(&myPwm2);
      activePWM(&myPwm3);
      break;
  case 0x1:
        myPwm1->AQCSFRC = 0x0009;
      myPwm2->AQCSFRC = 0x0005;
      myPwm3->AQCSFRC = 0x0000;
      setPWMOutLow(&myPwm1);
      setPWMOff(&myPwm2);
      activePWM(&myPwm3);
      break;
  case 0x3:
        myPwm1->AQCSFRC = 0x0009;
      myPwm2->AQCSFRC = 0x0000;
      myPwm3->AQCSFRC = 0x0005;
      setPWMOutLow(&myPwm1);
      activePWM(&myPwm2);
      setPWMOff(&myPwm3);
      break;
  case 0x2:
      myPwm1->AQCSFRC = 0x0005;
      myPwm2->AQCSFRC = 0x0000;
      myPwm3->AQCSFRC = 0x0009;
      setPWMOff(&myPwm1);
      activePWM(&myPwm2);
      setPWMOutLow(&myPwm3);
      break;
  default:
      myPwm1->AQCSFRC = 0x0005;
      myPwm2->AQCSFRC = 0x0005;
      myPwm3->AQCSFRC = 0x0005;
      setPWMOff(&myPwm1);
      setPWMOff(&myPwm2);
      setPWMOff(&myPwm3);
      break;
  }
}*/
/*

void setPWMOutLow(PWM_Handle * pwm){
  PWM_setActionQual_CntUp_CmpA_PwmA(*pwm, PWM_ActionQual_Clear);
  PWM_setActionQual_CntDown_CmpA_PwmA(*pwm, PWM_ActionQual_Clear);
  PWM_setActionQual_CntUp_CmpB_PwmB(*pwm, PWM_ActionQual_Clear);
  PWM_setActionQual_CntDown_CmpB_PwmB(*pwm, PWM_ActionQual_Set);
}

void setPWMOff(PWM_Handle * pwm){
  PWM_setActionQual_CntUp_CmpA_PwmA(*pwm, PWM_ActionQual_Clear);
  PWM_setActionQual_CntDown_CmpA_PwmA(*pwm, PWM_ActionQual_Clear);
  PWM_setActionQual_CntUp_CmpB_PwmB(*pwm, PWM_ActionQual_Clear);
  PWM_setActionQual_CntDown_CmpB_PwmB(*pwm, PWM_ActionQual_Clear);
}

void activePWM(PWM_Handle * pwm){
  PWM_setActionQual_CntUp_CmpA_PwmA(*pwm, PWM_ActionQual_Clear);
  PWM_setActionQual_CntDown_CmpA_PwmA(*pwm, PWM_ActionQual_Set);
  PWM_setActionQual_CntUp_CmpB_PwmB(*pwm, PWM_ActionQual_Set);
  PWM_setActionQual_CntDown_CmpB_PwmB(*pwm, PWM_ActionQual_Clear);
}
*/
