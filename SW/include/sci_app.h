/*
 * sci.h
 *
 *  Created on: Mar 5, 2018
 *      Author: Isaac.Rose
 */

#ifndef INCLUDE_SCI_APP_H_
#define INCLUDE_SCI_APP_H_

#include <DSP28x_Project.h>

void init_sci();
void sendData(uint8_t* data, uint8_t len);
__interrupt void sci_tx_isr();


#endif /* INCLUDE_SCI_APP_H_ */
