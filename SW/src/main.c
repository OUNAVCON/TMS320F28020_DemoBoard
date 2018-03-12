#include <IQmathLib.h>
#include <DSP28x_Project.h>
#include "sci.h"
#include "gpio.h"
#include "flash.h"
#include "pie.h"
#include "clock.h"
#include "clk.h"
#include "pwm_app.h"
#include "sci_app.h"

/**
 * main.c
 */

    GPIO_Handle myGpio;
    PIE_Handle myPie;
int main(void)
{
    ADC_Handle myAdc;
    FLASH_Handle myFlash;
    CPU_Handle myCpu;
    uint8_t data[10];

    //
    // Initialize all the handles needed for this application
    //
    myAdc = ADC_init((void *)ADC_BASE_ADDR, sizeof(ADC_Obj));
    myCpu = CPU_init((void *)NULL, sizeof(CPU_Obj));
    myFlash = FLASH_init((void *)FLASH_BASE_ADDR, sizeof(FLASH_Obj));
    myGpio = GPIO_init((void *)GPIO_BASE_ADDR, sizeof(GPIO_Obj));
    myPie = PIE_init((void *)PIE_BASE_ADDR, sizeof(PIE_Obj));

    init_clocks();

    //
    // Disable the PIE and all interrupts
    //
    PIE_disable(myPie);
    PIE_disableAllInts(myPie);
    CPU_disableGlobalInts(myCpu);
    CPU_clearIntFlags(myCpu);

    //
    // If running from flash copy RAM only functions to RAM
    //
#ifdef _FLASH
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
#endif

    //
    // Setup a debug vector table and enable the PIE
    //
 //   PIE_setDebugIntVectorTable(myPie);
    PIE_enable(myPie);

    //Init ADC

    //Init ECAP

    //Init USART
    init_sci();
    //init pwm
    init_pwm();


    //Enable Debug GPIO.
    GPIO_setPullUp(myGpio, GPIO_Number_12, GPIO_PullUp_Disable);
    GPIO_setQualification(myGpio, GPIO_Number_12, GPIO_Qual_ASync);
    GPIO_setMode(myGpio, GPIO_Number_12, GPIO_12_Mode_GeneralPurpose);
    GPIO_setDirection(myGpio, GPIO_Number_12, GPIO_Direction_Output);




    //Init Timer for 100hz, to perform calculations and to fire off serial port.
    uint16_t currentScaled;
    int i = 0;
    while(1){
        //Check if serial port go flag is set.
        i++; //Kill Some time.

        _iq current = _IQabs(_IQ(241.0));

      //  current = _IQmpy(current, _IQ(10.0L));
        currentScaled = (uint16_t)((uint32_t)current >> 17);
        if(i>1000){
            GPIO_toggle(myGpio, GPIO_Number_12);
            data[0] = 'a';
            sendData(data, 1);
            i=0;
        }
    }
	return 0;
}
