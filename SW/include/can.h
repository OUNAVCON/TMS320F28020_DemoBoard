/*
 * can.h
 *
 *  Created on: Oct 21, 2017
 *      Author: isaac
 */

#ifndef INCLUDE_CAN_H_
#define INCLUDE_CAN_H_

typedef enum{
    CAN_TX_MAILBOX_1 = 0x30,
    CAN_TX_MAILBOX_2 = 0x40,
    CAN_TX_MAILBOX_3 = 0x50,
    CAN_RX_MAILBOX_1 = 0x60,
    CAN_RX_MAILBOX_2 = 0x70
}CAN_MAILBOX_NUMBER;


typedef enum {
    CAN_FILTER_0=0x0,
    CAN_FILTER_1=0x1,
    CAN_FILTER_2=0x2,
    CAN_FILTER_3=0x3,
    CAN_FILTER_4=0x4,
    CAN_FILTER_5=0x5
}CAN_FILTER_NUMBER;

typedef struct{
    uint8_t data[8];
    uint8_t dlc;
    uint32_t ID;
}CAN_MSG;

typedef enum {
    CAN_READ_FAIL,
    CAN_OK,
    CAN_WRITE_FAIL
}CAN_STATUS;


typedef enum {
    MCP2515_RESET = 0xC0,
    MCP2515_READ = 0x03,
    MCP2515_READ_RX_BUFFER = 0x90,
    MCP2515_WRITE = 0x02,
    MCP2515_LOAD_TX_BUFFER = 0x40,
    MCP2515_RTS = 0x80,
    MCP2515_READ_STATUS = 0xA0,
    MCP2515_RX_STATUS = 0xB0,
    MCP2515_BIT_MODIFY = 0x05
}MCP2515_CMDS;

typedef enum {
    CAN_STATUS_ADDRESS=0x0E,
    CAN_CONTROL_ADDRESS=0x0F,
    CAN_TRANSMIT_CONTROL = 0x00,
    CAN_TX_ID_START_ADDRESS = 0x01,
    CAN_DLC_START_ADDRESS = 0x05,
    CAN_RX_ID_START_ADDRESS = 0x01,
    CAN_INT_FLAG = 0x2C,
    CNF1 = 0x2A,
    CNF2 = 0x29,
    CNF3 = 0x28
}MCP2515_REGS;



void initCAN(void);
void setCANNormalMode(void);
void readMailbox(CAN_MAILBOX_NUMBER mb, CAN_MSG* msg);
void writeMailbox(CAN_MAILBOX_NUMBER mb, CAN_MSG* msg);
void sendMailbox(CAN_MAILBOX_NUMBER mb);
uint8_t readCANStatus(void);
uint8_t readCANTXStatus(CAN_MAILBOX_NUMBER mb);
uint8_t readCANRXStatus(CAN_MAILBOX_NUMBER mb);
void clearCANRXStatus(CAN_MAILBOX_NUMBER mb);
void setCANRXFilter(CAN_MAILBOX_NUMBER mb, uint32_t filter, CAN_FILTER_NUMBER filterNumber);
void setCANRXMask(CAN_MAILBOX_NUMBER mb, uint32_t mask);
void setCANRXEnable(CAN_MAILBOX_NUMBER mb);
void setCANRXMaskEnable(CAN_MAILBOX_NUMBER mb);



#endif /* INCLUDE_CAN_H_ */
