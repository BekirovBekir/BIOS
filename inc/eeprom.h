/*
 * eeprom.h
 *
 *  Created on: 3 янв. 2018 г.
 *      Author: bekir
 */

#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_

unsigned int Write_EEPROM(char* ptr_buf, unsigned int pos);
unsigned int Read_EEPROM(char* ptr_buf, unsigned int pos, unsigned int cnt);

#endif /* INC_EEPROM_H_ */
