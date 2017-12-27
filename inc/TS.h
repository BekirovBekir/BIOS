/*
 * TS.h
 *
 *  Created on: 26 дек. 2017 г.
 *      Author: bekir
 */

#ifndef TS_H_
#define TS_H_

typedef struct key_info
{
	int x;
	int y;
	char status;
	char key_num;
}ilitek_key_info;

typedef enum{TS_KEY_RELEASE=0, TS_KEY_PRESS}TS_KEY_STATUS;

ilitek_key_info Scan_TS_Key(void); // Function scanning TS and return struct with key parameters
void FSM_TS (ilitek_key_info* key); // finite state machine for TS

#endif /* TS.h */
