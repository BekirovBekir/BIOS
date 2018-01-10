/*
 * PreAsm.h
 *
 *  Created on: 9 янв. 2018 г.
 *      Author: bekir
 */

#ifndef INC_PREASM_H_
#define INC_PREASM_H_

int TestMMC(int Do);
int FuncSPI_32MBit_NOR_Flash(int Do);
int FuncEEPROM(int Do);
int FuncAccelerometer_Calibration(int Do);
int FuncConfirm_Battery_Charger_Communication(int Do);
int FuncBarometer_Functionality(int Do);
int FuncAmbient_Light_Sensor_Functionality(int Do);
int FuncEMMY_163_Connectivity_Check(int Do);
int FuncConfirm_PMIC_Communication(int Do);


#endif /* INC_PREASM_H_ */
