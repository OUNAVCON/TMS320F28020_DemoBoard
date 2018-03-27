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
#include "adc_app.h"
#include "spi_app.h"

#include "can.h"
/**
 * main.c
 */

    GPIO_Handle myGpio;
    PIE_Handle myPie;
    CPU_Handle myCpu;

int main(void)
{
    FLASH_Handle myFlash;
    uint8_t data[10];

    CAN_MSG msg;
    CAN_MSG rxMsg;
    uint8_t canTXStatus = 0;
    uint8_t canRXStatus = 0;
    static uint8_t canStatus;

    uint32_t index = 0;
    uint32_t canIndex = 0;

    //
    // Initialize all the handles needed for this application
    //
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


    //Init ADC
    init_adc();
    //Init ECAP

    initSpi();
    //Init USART
    init_sci();
    //init pwm
    init_pwm();

        initCAN();
        setCANRXFilter(CAN_RX_MAILBOX_1, 0x000000F9, CAN_FILTER_0);
        setCANRXFilter(CAN_RX_MAILBOX_1, 0x00000000, CAN_FILTER_1);
        setCANRXMask(CAN_RX_MAILBOX_1, 0x000000FF);
        setCANRXEnable(CAN_RX_MAILBOX_1);
        setCANNormalMode();

    //Enable Debug GPIO.
    GPIO_setPullUp(myGpio, GPIO_Number_12, GPIO_PullUp_Disable);
    GPIO_setQualification(myGpio, GPIO_Number_12, GPIO_Qual_ASync);
    GPIO_setMode(myGpio, GPIO_Number_12, GPIO_12_Mode_GeneralPurpose);
    GPIO_setDirection(myGpio, GPIO_Number_12, GPIO_Direction_Output);


    //Turn interrupts on for the system.
    PIE_enable(myPie);
    CPU_enableGlobalInts(myCpu);

    //Init Timer for 100hz, to perform calculations and to fire off serial port.
    uint16_t currentScaled;
    int i = 0;



    msg.ID = 0x18FF0013;
    msg.dlc = 8;
    msg.data[0] = 0x00;
    msg.data[1] = 0x00;
    msg.data[2] = 0x00;
    msg.data[3] = 0x00;
    msg.data[4] = 0x00;
    msg.data[5] = 0x00;
    msg.data[6] = 0x00;
    msg.data[7] = 0x00;
    //canStatus = readCANStatus();


    while(1){
        //Check if serial port go flag is set.
        i++; //Kill Some time.

        _iq current = _IQabs(_IQ(241.0));

      //  current = _IQmpy(current, _IQ(10.0L));
        currentScaled = (uint16_t)((uint32_t)current >> 17);
        if(i>1000){
            GPIO_toggle(myGpio, GPIO_Number_12);
           // data[0] = 'a';
         //   sendData(data, 1);
            i=0;





            /*      if(canIndex > 10){
                      // canStatus = readCANStatus();
                       canTXStatus = readCANTXStatus(CAN_TX_MAILBOX_1);
                       canRXStatus = readCANRXStatus(CAN_RX_MAILBOX_1);
                       if ((canRXStatus & 0x01) == 0x01)
                       {
                           readMailbox(CAN_RX_MAILBOX_1, &rxMsg);
                           clearCANRXStatus(CAN_RX_MAILBOX_1);
                           msg.data[1]++;
                           msg.data[2] = rxMsg.data[0];
                       }
                      if ((canTXStatus & 0x08) == 0)
                       {
                           msg.data[0]++;
                           writeMailbox(CAN_TX_MAILBOX_1, &msg);
                       }

                       canIndex = 0;
                   }
           */




        }
    }
	return 0;
}
