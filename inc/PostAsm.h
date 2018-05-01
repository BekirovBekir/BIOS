/*
 * PostAsm.h
 *
 *  Created on: 30 апр. 2018 г.
 *      Author: bekir
 */

#ifndef INC_POSTASM_H_
#define INC_POSTASM_H_

#include "FrameBuffer.h"

int TestMMC_PostAsm(int Do);
int FuncSPI_32MBit_NOR_Flash_PostAsm(int Do);
int FuncEEPROM_PostAsm(int Do);
int FuncAccelerometer_Calibration_PostAsm(int Do);
int FuncConfirm_Battery_Charger_Communication_PostAsm(int Do);
int FuncBarometer_Functionality_PostAsm(int Do);
int FuncAmbient_Light_Sensor_Functionality_PostAsm(int Do);
int FuncEMMY_163_Connectivity_Check_PostAsm(int Do);
int FuncConfirm_PMIC_Communication_PostAsm(int Do);

int Init_LARA_SARA_PostAsm(char* port_name, int port_speed);
int FuncSARA_Module_Testing_Power_Antenna_Permission_PostAsm(int Do);
int FuncLARA_Module_Testing_Power_Antenna_Permission_PostAsm(int Do);
int Cameras_Test_PostAsm(int Do, CAMPARAM* camptr1, CAMPARAM* camptr2);
int Cameras_Test_Full_PostAsm(int Do);
int Audio_Codec_Test_PostAsm(int Do);
int NEO_Test_PostAsm(int Do);

void NFC_PostAsm(void);
void DisplayTest_PostAsm(int Do);

#endif /* INC_POSTASM_H_ */
