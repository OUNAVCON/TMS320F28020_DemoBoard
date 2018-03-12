/*
 * adc_app.c
 *
 *  Created on: Mar 12, 2018
 *      Author: Isaac.Rose
 */

#include "adc_app.h"
#include <DSP28x_Project.h>
#include "adc.h"
#include "clk.h"
#include "pie.h"
#include "cpu.h"

extern CPU_Handle myCpu;
extern PIE_Handle myPie;
extern CLK_Handle myClk;
ADC_Handle myAdc;

uint16_t adcValues[7];

void init_adc(){
    CLK_enableAdcClock(myClk);
    myAdc = ADC_init((void *) ADC_BASE_ADDR, sizeof(ADC_Obj));

    //
    // Register interrupt handlers in the PIE vector table
    //
    PIE_registerPieIntHandler(myPie, PIE_GroupNumber_10, PIE_SubGroupNumber_1,
                              (intVec_t) &adc_isr);

    // ADCINT1 trips after AdcResults latch
    //
    ADC_setIntPulseGenMode(myAdc, ADC_IntPulseGenMode_Prior);

    ADC_enableInt(myAdc, ADC_IntNumber_1);              // Enabled ADCINT1
//        ADC_enableInt(myAdc, ADC_IntNumber_2);              // Enabled ADCINT1

    //
    // Disable ADCINT1 Continuous mode
    //
    ADC_setIntMode(myAdc, ADC_IntNumber_1, ADC_IntMode_ClearFlag);
//        ADC_setIntMode(myAdc, ADC_IntNumber_2, ADC_IntMode_ClearFlag);

    //
    // setup EOC1 to trigger ADCINT1 to fire (EOC0 - A1, EOC1 - B1)
    // setup EOC1 to trigger ADCINT2 to fire (EOC0 - A3, EOC1 - B3)
    // setup EOC1 to trigger ADCINT3 to fire (EOC0 - A7, EOC1 - B7)
    //
    ADC_setIntSrc(myAdc, ADC_IntNumber_1, ADC_IntSrc_EOC0);
 //   ADC_setIntSrc(myAdc, ADC_IntNumber_2, ADC_IntSrc_EOC5);

    ADC_setSampleMode(myAdc, ADC_SampleMode_SOC0_and_SOC1_Together);
    ADC_setSampleMode(myAdc, ADC_SampleMode_SOC2_and_SOC3_Together);
    ADC_setSampleMode(myAdc, ADC_SampleMode_SOC4_and_SOC5_Together);
//       ADC_setSampleMode(myAdc, ADC_SampleMode_SOC6_and_SOC7_Together);

    ADC_setSocChanNumber(myAdc, ADC_SocNumber_0, ADC_SocChanNumber_A1); //
    ADC_setSocChanNumber(myAdc, ADC_SocNumber_1, ADC_SocChanNumber_B1); //
    ADC_setSocChanNumber(myAdc, ADC_SocNumber_2, ADC_SocChanNumber_A3); //
    ADC_setSocChanNumber(myAdc, ADC_SocNumber_3, ADC_SocChanNumber_B3); //
    ADC_setSocChanNumber(myAdc, ADC_SocNumber_4, ADC_SocChanNumber_A7); //
    ADC_setSocChanNumber(myAdc, ADC_SocNumber_5, ADC_SocChanNumber_B7); //
//        ADC_setSocChanNumber(myAdc, ADC_SocNumber_6, ADC_SocChanNumber_A0); //VBus

    ADC_setSocTrigSrc(myAdc, ADC_SocNumber_0, ADC_SocTrigSrc_EPWM4_ADCSOCA); //ADC_SocTrigSrc_EPWM1_ADCSOCA
    ADC_setSocTrigSrc(myAdc, ADC_SocNumber_2, ADC_SocTrigSrc_EPWM4_ADCSOCA);
    ADC_setSocTrigSrc(myAdc, ADC_SocNumber_4, ADC_SocTrigSrc_EPWM4_ADCSOCA);
    //
    // set SOC0 S/H Window to 14 ADC Clock Cycles, (13 ACQPS plus 1)
    //
    ADC_setSocSampleWindow(myAdc, ADC_SocNumber_0,
                           ADC_SocSampleWindow_7_cycles);
    ADC_setSocSampleWindow(myAdc, ADC_SocNumber_1,
                           ADC_SocSampleWindow_7_cycles);
    ADC_setSocSampleWindow(myAdc, ADC_SocNumber_2,
                           ADC_SocSampleWindow_7_cycles);
    ADC_setSocSampleWindow(myAdc, ADC_SocNumber_3,
                           ADC_SocSampleWindow_7_cycles);
    ADC_setSocSampleWindow(myAdc, ADC_SocNumber_4,
                           ADC_SocSampleWindow_7_cycles);
    ADC_setSocSampleWindow(myAdc, ADC_SocNumber_5,
                           ADC_SocSampleWindow_7_cycles);
    //
    // Initialize the ADC
    //
    ADC_enableBandGap(myAdc);
    ADC_enableRefBuffers(myAdc);
    ADC_powerUp(myAdc);
    ADC_enable(myAdc);
    ADC_setVoltRefSrc(myAdc, ADC_VoltageRefSrc_Int);

    //
    // Enable ADCINT1 in PIE
    //
    PIE_enableAdcInt(myPie, ADC_IntNumber_1);
    CPU_enableInt(myCpu, CPU_IntNumber_10);


}

__interrupt void adc_isr()
{

    adcValues[0] = ADC_readResult(myAdc, ADC_ResultNumber_0);
    adcValues[1] = ADC_readResult(myAdc, ADC_ResultNumber_1);
    adcValues[2] = ADC_readResult(myAdc, ADC_ResultNumber_2);
//    adcValues[0] = (adcValues[0].filt*7 + adcValues[0].raw)/8;
//    adcValues[1] = (adcValues[1].filt*7 + adcValues[1].raw)/8;
//    adcValues[2] = (adcValues[2].filt*7 + adcValues[2].raw)/8;

    ADC_clearIntFlag(myAdc, ADC_IntNumber_1);
    PIE_clearInt(myPie, PIE_GroupNumber_10);

    return;
}
