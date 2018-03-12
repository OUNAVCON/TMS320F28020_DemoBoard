#include "sci_app.h"
#include <DSP28x_Project.h>
#include "clk.h"
#include "gpio.h"
#include "sci.h"

extern CLK_Handle myClk;
extern GPIO_Handle myGpio;
static SCI_Handle mySci;

void init_sci(){


    mySci = SCI_init((void *)SCIA_BASE_ADDR, sizeof(SCI_Obj));
    GPIO_setPullUp(myGpio, GPIO_Number_28, GPIO_PullUp_Enable);
    GPIO_setPullUp(myGpio, GPIO_Number_29, GPIO_PullUp_Disable);
    GPIO_setQualification(myGpio, GPIO_Number_28, GPIO_Qual_ASync);
    GPIO_setMode(myGpio, GPIO_Number_28, GPIO_28_Mode_SCIRXDA);
    GPIO_setMode(myGpio, GPIO_Number_29, GPIO_29_Mode_SCITXDA);
    CLK_enableSciaClock(myClk);

    //
    // 1 stop bit,  No loopback, No parity,8 char bits, async mode,
    // idle-line protocol
    //
    SCI_disableParity(mySci);
    SCI_setNumStopBits(mySci, SCI_NumStopBits_One);
    SCI_setCharLength(mySci, SCI_CharLength_8_Bits);

    SCI_enableTx(mySci);
    SCI_enableRx(mySci);
 //   SCI_enableTxInt(mySci);
 //   SCI_enableRxInt(mySci);

    //SCI BRR = LSPCLK/(SCI BAUDx8) - 1
    //129 => 9600
    //64 => 19200
#if (CPU_FRQ_60MHZ)
    SCI_setBaudRate(mySci, (SCI_BaudRate_e)194);
#elif (CPU_FRQ_50MHZ)
    SCI_setBaudRate(mySci, (SCI_BaudRate_e)162);
#elif (CPU_FRQ_40MHZ)
    SCI_setBaudRate(mySci, (SCI_BaudRate_e)64);
#endif

    SCI_enable(mySci);
}

void sendData(uint8_t* data, uint8_t len){
    while(SCI_getTxFifoStatus(mySci) != SCI_FifoStatus_Empty)
    {

    }

    SCI_putDataBlocking(mySci, *data);
}

