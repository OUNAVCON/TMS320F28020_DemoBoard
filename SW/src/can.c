/*
 * can.c
 *
 *  Created on: Oct 21, 2017
 *      Author: isaac
 */

#include "DSP28x_Project.h"
#include "gpio.h"
#include "spi.h"
#include "can.h"
#include "spi_app.h"

static uint8_t readSPI(uint8_t address);
static void writeSPI(uint8_t address, uint8_t value);
static void writeSPIWithMask(uint8_t address, uint8_t mask, uint8_t value);
static void convertIDToArray(uint32_t id, uint8_t* array);
static uint32_t convertArrayToID(uint8_t* array);
extern GPIO_Handle myGpio;

void initCAN(void)
{
    //initSPI();
    //writeSPIWithMask(CAN_CONTROL_ADDRESS, 0xE0, 0x80); //Set config mode.
    writeSPI(CNF1, 0xC0);
    writeSPI(CNF2, 0x9b); //0x91
    writeSPI(CNF3, 0x06); //0x01
}

void setCANNormalMode(void)
{
    writeSPIWithMask(CAN_CONTROL_ADDRESS, 0xE8, 0x08); //Set run mode.
}

uint8_t readCANTXStatus(CAN_MAILBOX_NUMBER mb)
{
    return readSPI(mb);
}

uint8_t readCANStatus(void)
{
    return readSPI(CAN_STATUS_ADDRESS);
}

void writeMailbox(CAN_MAILBOX_NUMBER mb, CAN_MSG* msg)
{
    uint8_t data[15];
    uint32_t index = 0;
    data[0] = MCP2515_WRITE;
    data[1] = mb | CAN_TX_ID_START_ADDRESS; //TX data reg start.
    convertIDToArray(msg->ID, &data[2]);
    //DLC 35h,45h,55h Lower Nibble
    data[6] = msg->dlc & 0x0F;
    //Data buffers 36-3DH, 46-4DH, 56-5dH
    for (index = 0; index < 8; index++)
    {
        data[index + 7] = msg->data[index];
    }

    //OUTPUT_SET(GPIOA, PTB5);
    SPI_TransferWait8(CS_MCP2515, data, data, 15);

    //initiate the transfer
    writeSPIWithMask(CAN_TX_MAILBOX_1 | CAN_TRANSMIT_CONTROL, 0x08, 0x08); //Set the tx bit.
}

uint8_t readCANRXStatus(CAN_MAILBOX_NUMBER mb)
{
    return readSPI(CAN_INT_FLAG);
}

void clearCANRXStatus(CAN_MAILBOX_NUMBER mb)
{
    return writeSPIWithMask(CAN_INT_FLAG, 0x01, 0x00);

}

void setCANRXEnable(CAN_MAILBOX_NUMBER mb)
{
    switch (mb)
    {
    default:
    case CAN_RX_MAILBOX_1:
        writeSPIWithMask(mb, 0x64, 0x40); //Set the rx bit.
        break;
    case CAN_RX_MAILBOX_2:
        writeSPIWithMask(mb, 0x64, 0x40); //Set the rx bit.
        break;
    }
}

void readMailbox(CAN_MAILBOX_NUMBER mb, CAN_MSG* msg)
{
    uint8_t data[14];
    uint8_t index = 0;
    data[0] = MCP2515_READ;
    data[1] = mb | 0x01;
    SPI_TransferWait8(CS_MCP2515, data, data, 14);

    msg->ID = convertArrayToID(&data[2]);
    msg->dlc = data[6];
    for (index = 0; index < 8; index++)
    {
        msg->data[index] = data[index + 7];
    }

}

void setCANRXFilter(CAN_MAILBOX_NUMBER mb, uint32_t filterValue,
                    CAN_FILTER_NUMBER filterNumber)
{

    uint8_t filterArray[6];
    filterArray[0] = MCP2515_WRITE;
    filterArray[1] =
            filterNumber < 2 ? filterNumber * 4 : (filterNumber + 1) * 4;
    convertIDToArray(filterValue, &filterArray[2]);

    SPI_TransferWait8(CS_MCP2515, filterArray, filterArray, 6);
}

void setCANRXMask(CAN_MAILBOX_NUMBER mb, uint32_t mask)
{
    uint8_t maskArray[6];
    maskArray[0] = MCP2515_WRITE;
    convertIDToArray(mask, &maskArray[2]);
    switch (mb)
    {
    default:
    case CAN_RX_MAILBOX_1:
        maskArray[1] = 0x20;
        break;
    case CAN_RX_MAILBOX_2:
        maskArray[1] = 0x24;
        break;
    }
    SPI_TransferWait8(CS_MCP2515, maskArray, maskArray, 6);
}

static void convertIDToArray(uint32_t id, uint8_t* array)
{
    uint8_t temp = 0;
    uint8_t temp2 = 0;
    //SID Upper SID10-SID3 31h
    *array = (uint8_t) ((id & 0x1FE00000) >> 21);
    //SID Lower SID2-SID0 x EXIDE x EID17-EID16 32h
    temp = (uint8_t) ((id & 0x30000) >> 16);
    temp2 = (uint8_t) ((id & 0x1C0000) >> 13);
    *(array + 1) = temp2 | 0x8 | temp;
    //ESID Upper EID15 - EID8 33h
    *(array + 2) = (uint8_t) ((id & 0xFF00) >> 8);
    //ESID Lower EID7-EID0 34h
    *(array + 3) = (uint8_t) ((id & 0xFF));
}

static uint32_t convertArrayToID(uint8_t* array)
{
    uint32_t id = 0x0;
    id |= *(array + 3);
    id |= ((uint32_t) *(array + 2)) << 8;
    id |= ((uint32_t) *array) << 21;
    id |= ((uint32_t) (0x03 & *(array + 1))) << 16;
    id |= ((uint32_t) (0xE0 & *(array + 1))) << 13;
    return id;
}

static uint8_t readSPI(uint8_t address)
{
    uint8_t readData[3];
    uint8_t writeData[3];
    //uint8_t index = 0;
    writeData[0] = MCP2515_READ; //command
    writeData[1] = address; //address
    writeData[2] = 0x00; //Dummy for get databack.
    SPI_TransferWait8(CS_MCP2515, readData, writeData, 3);

    return readData[2];
}

static void writeSPI(uint8_t address, uint8_t value)
{
    uint8_t readData[3];
    uint8_t writeData[3];
//uint8_t index = 0;
    writeData[0] = MCP2515_WRITE; //command
    writeData[1] = address; //address
    writeData[2] = value; //Dummy for get data back.
    SPI_TransferWait8(CS_MCP2515, readData, writeData, 3);

}

static void writeSPIWithMask(uint8_t address, uint8_t mask, uint8_t value)
{
    uint8_t readData[4];
    uint8_t writeData[4];
//uint8_t index = 0;
    writeData[0] = MCP2515_BIT_MODIFY; //command
    writeData[1] = address; //address
    writeData[2] = mask; //Dummy for get data back.
    writeData[3] = value; //Dummy for get data back.
    SPI_TransferWait8(CS_MCP2515, readData, writeData, 4);
}

