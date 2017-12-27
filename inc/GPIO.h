/*
 * GPIO.h
 *
 *  Created on: 17 авг. 2017 г.
 *      Author: bekir
 */

#ifndef GPIO_H_
#define GPIO_H_

#define SENSOR_PWR_EN "138"	// sensors power
#define CODEC_PWR_EN "112"	// codec power
#define DISP_PWR_EN "92" 	// display power
#define GPS_PWR_EN "63" 	// gps power
#define WIFI_PWR_EN "7" 	// wifi power
#define WIFI_EN "8" 	// wifi enable
#define MODEM_MDM1_PWR "49" 	// modem sara power

//typedef enum {PIN_IN=0, PIN_OUT, PIN_PULL_UP, PIN_PULL_DOWN} GPIO_MODE;

#define GPIO_BUF_SIZE 100

int Init_GPIO(const char* pin_name, char* pin_mode);
int Read_GPIO(const char* pin_name);
int Write_GPIO(char* pin_name, char* pin_state);
int Set_PWM_LED (int pwm);
int Set_I_Charge (int level);

#endif /* GPIO_H_ */
