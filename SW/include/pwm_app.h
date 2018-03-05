/*
 * pwm_app.h
 *
 *  Created on: Mar 5, 2018
 *      Author: Isaac.Rose
 */

#ifndef INCLUDE_PWM_APP_H_
#define INCLUDE_PWM_APP_H_

//Deadband added into period min and max.
#define PWM_PERIOD 400
#define MIN_PWM_PERIOD 150
#define MAX_PWM_PERIOD PWM_PERIOD - 1
#define PWM_HALF_PER_IQ _IQ(PWM_PERIOD/2)

void init_pwm();
void setPwm(uint16_t dc1);
__interrupt void pwm_isr(void);


#endif /* INCLUDE_PWM_APP_H_ */
