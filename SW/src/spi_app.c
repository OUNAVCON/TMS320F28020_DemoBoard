#include "spi_app.h"
#include "spi.h"
#include "gpio.h"
#include "clk.h"

SPI_Handle mySpi;
extern GPIO_Handle myGpio;
extern SPI_Handle mySpi;
extern CLK_Handle myClk;

void initSpi(void)
{
    mySpi = SPI_init((void *) SPIA_BASE_ADDR, sizeof(SPI_Obj));
    CLK_enableSpiaClock(myClk);
    //Chip Select for DRV8305
    GPIO_setPullUp(myGpio, GPIO_Number_19, GPIO_PullUp_Enable);
    GPIO_setDirection(myGpio, GPIO_Number_19, GPIO_Direction_Output);
    GPIO_setMode(myGpio, GPIO_Number_19, GPIO_19_Mode_GeneralPurpose);
    //Chip Select for MCP2515
    GPIO_setPullUp(myGpio, GPIO_Number_34, GPIO_PullUp_Enable);
    GPIO_setDirection(myGpio, GPIO_Number_34, GPIO_Direction_Output);
    GPIO_setMode(myGpio, GPIO_Number_34, GPIO_34_Mode_GeneralPurpose);
    //SPI
    //GPIO_setQualification(myGpio, GPIO_Number_16, GPIO_Qual_ASync);
    GPIO_setQualification(myGpio, GPIO_Number_17, GPIO_Qual_Sync);
    GPIO_setDirection(myGpio, GPIO_Number_16, GPIO_Direction_Output);
    GPIO_setDirection(myGpio, GPIO_Number_17, GPIO_Direction_Input);
    GPIO_setDirection(myGpio, GPIO_Number_18, GPIO_Direction_Output);
    GPIO_setMode(myGpio, GPIO_Number_16, GPIO_16_Mode_SPISIMOA);
    GPIO_setMode(myGpio, GPIO_Number_17, GPIO_17_Mode_SPISOMIA);
    GPIO_setMode(myGpio, GPIO_Number_18, GPIO_18_Mode_SPICLKA);

    //setup SPI
    //CCR = 0x0F
    //CTL = 0x06
    //BRR = 0x1E
    //PRI 0x10
    SPI_setBaudRate(mySpi, SPI_BaudRate_1_MBaud);
    SPI_setClkPhase(mySpi, SPI_ClkPhase_Normal);
    SPI_setClkPolarity(mySpi,
                       SPI_ClkPolarity_OutputRisingEdge_InputFallingEdge);
    SPI_setMode(mySpi, SPI_Mode_Master);
    SPI_setTriWire(mySpi, SPI_TriWire_NormalFourWire);
    SPI_setCharLength(mySpi, SPI_CharLength_16_Bits); //DRV8305 has to have a 16bit word or it ignores the message.
    SPI_enableTx(mySpi);
    SPI_setPriority(mySpi, SPI_Priority_FreeRun);
    //SPI_enableLoopBack(mySpi);
    SPI_enable(mySpi);
    //SPI_enableTxFifo(mySpi);
    //SPI_enableRxFifo(mySpi);

    //
    // Initialize SPI FIFO registers
    //
    SPI_enableChannels(mySpi);
    SPI_enableFifoEnh(mySpi);
    SPI_resetTxFifo(mySpi);
    SPI_clearTxFifoInt(mySpi);
    SPI_resetRxFifo(mySpi);
    SPI_clearRxFifoInt(mySpi);
    SPI_setRxFifoIntLevel(mySpi, SPI_FifoLevel_1_Word);
}

Uint16 spiTransferData(uint16_t data, SPI_CHIP_SELECT cs)
{
    Uint16 returnResult;
    switch (cs)
    {
    case CS_DRV8305:
        GPIO_setLow(myGpio, GPIO_Number_19); //DRV8305
        break;
    case CS_MCP2515:
        GPIO_setLow(myGpio, GPIO_Number_34); //DRV8305
        break;
    }

    SPI_write(mySpi, data);
//wait till all bytes have been sent.
    while (SPI_getRxFifoStatus(mySpi) == SPI_FifoStatus_Empty)
    {
        ;
    }
    returnResult = SPI_read(mySpi);
    switch (cs)
    {
    case CS_DRV8305:
        GPIO_setHigh(myGpio, GPIO_Number_19); //DRV8305
        break;
    case CS_MCP2515:
        GPIO_setHigh(myGpio, GPIO_Number_34); //DRV8305
        break;
    }

    return returnResult;
}

void SPI_TransferWait8(SPI_CHIP_SELECT cs, uint8_t* fromSlave, uint8_t* toSlave,
                       uint8_t len)
{
    uint16_t outData;
    uint8_t inData;
    uint16_t temp;
    uint8_t index = 0;
    uint8_t timeCntr = 0;
    SPI_setCharLength(mySpi, SPI_CharLength_8_Bits);
//    SPI_reset(mySpi);
    switch (cs)
    {
    case CS_DRV8305:
        GPIO_setLow(myGpio, GPIO_Number_19); //DRV8305
        break;
    case CS_MCP2515:
        GPIO_setLow(myGpio, GPIO_Number_34); //DRV8305
        break;
    }
    for (index=0; index < 25; index++){
        ;
    }
    for (index = 0; index < len; index++)
    {
        outData = (uint16_t) (*(toSlave + index));
        outData = outData << 8;
        SPI_write(mySpi, outData);

//wait till all bytes have been sent.
        while (SPI_getTxFifoStatus(mySpi) != SPI_FifoStatus_Empty)
        {
            ;
        }
        while (SPI_getRxFifoStatus(mySpi) != SPI_FifoStatus_1_Word)
        {
            ;
        }
        temp = SPI_read(mySpi);
        inData = (uint8_t)(temp & 0xFF);
        *(fromSlave + index) = inData;
        for(timeCntr=0; timeCntr<25; timeCntr++){
            ;
        }
    }
    switch (cs)
    {
    case CS_DRV8305:
        GPIO_setHigh(myGpio, GPIO_Number_19); //DRV8305
        break;
    case CS_MCP2515:
        GPIO_setHigh(myGpio, GPIO_Number_34); //DRV8305
        break;
    }
   SPI_setCharLength(mySpi, SPI_CharLength_16_Bits);
 //   SPI_reset(mySpi);
}
