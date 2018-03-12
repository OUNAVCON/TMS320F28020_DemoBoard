#include "sci_app.h"
#include <DSP28x_Project.h>
#include "clk.h"
#include "gpio.h"
#include "sci.h"
#include "pie.h"
#include "cpu.h"

extern CLK_Handle myClk;
extern GPIO_Handle myGpio;
extern PIE_Handle myPie;
extern CPU_Handle myCpu;
static SCI_Handle mySci;

static void scia_fifo_init();

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

    PIE_registerPieIntHandler(myPie, PIE_GroupNumber_9, PIE_SubGroupNumber_2,
                              (intVec_t) &sci_tx_isr);
    CPU_enableInt(myCpu, CPU_IntNumber_9);

    SCI_enableTx(mySci);
    SCI_enableRx(mySci);
    SCI_enableTxInt(mySci);

 //   SCI_enableRxInt(mySci);

    //SCI BRR = LSPCLK/(SCI BAUDx8) - 1
    //129 => 9600
    //64 => 19200
    SCI_setBaudRate(mySci, (SCI_BaudRate_e)64);
    scia_fifo_init();               // Initialize the SCI FIFO
    SCI_enable(mySci);
}

void sendData(uint8_t* data, uint8_t len){
    while(SCI_getTxFifoStatus(mySci) != SCI_FifoStatus_Empty)
    {

    }

    SCI_putDataBlocking(mySci, *data);
}

static void scia_fifo_init()
{
    SCI_enableFifoEnh(mySci);
    SCI_resetTxFifo(mySci);
    SCI_clearTxFifoInt(mySci);
    SCI_resetChannels(mySci);
    SCI_setTxFifoIntLevel(mySci, SCI_FifoLevel_Empty);

    SCI_resetRxFifo(mySci);
    SCI_clearRxFifoInt(mySci);
    SCI_setRxFifoIntLevel(mySci, SCI_FifoLevel_4_Words);

    return;
}

__interrupt void sci_tx_isr(){
    SCI_clearTxFifoInt(mySci);
    PIE_clearInt(myPie, PIE_GroupNumber_9);

    return;
}
