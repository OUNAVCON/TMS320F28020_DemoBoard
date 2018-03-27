/*
 * spiApp.h
 *
 *  Created on: Oct 21, 2017
 *      Author: isaac
 */

#ifndef INCLUDE_SPI_APP_H_
#define INCLUDE_SPI_APP_H_
#include "DSP28x_Project.h"

typedef enum{
    CS_DRV8305,
    CS_MCP2515
}SPI_CHIP_SELECT;

void initSpi(void);
Uint16 spiTransferData(uint16_t data, SPI_CHIP_SELECT cs);
void SPI_TransferWait8(SPI_CHIP_SELECT cs,uint8_t* out, uint8_t* in, uint8_t len);

#endif /* INCLUDE_SPI_APP_H_ */
