/*
 * PreAsm.h
 *
 *  Created on: 9 янв. 2018 г.
 *      Author: bekir
 */

#ifndef INC_PREASM_H_
#define INC_PREASM_H_

#include "FrameBuffer.h"

int TestMMC(int Do);
int FuncSPI_32MBit_NOR_Flash(int Do);
int FuncEEPROM(int Do);
int FuncAccelerometer_Calibration(int Do);
int FuncConfirm_Battery_Charger_Communication(int Do);
int FuncBarometer_Functionality(int Do);
int FuncAmbient_Light_Sensor_Functionality(int Do);
int FuncEMMY_163_Connectivity_Check(int Do);
int FuncConfirm_PMIC_Communication(int Do);
void power_init(void);
void power_set_direction(void);
void power_set_on(void);
void power_set_off(void);
int Init_LARA_SARA(char* port_name, int port_speed);
int FuncSARA_Module_Testing_Power_Antenna_Permission(int Do);
int FuncLARA_Module_Testing_Power_Antenna_Permission(int Do);
int Cameras_Test(int Do, CAMPARAM* camptr1, CAMPARAM* camptr2);
int Audio_Codec_Test(int Do);
int NEO_Test(int Do);
int FuncSN_Burn_In(int Do);
int FuncSN_Read_In(int Do);


#endif /* INC_PREASM_H_ */
