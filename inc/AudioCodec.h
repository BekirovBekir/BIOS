/*
 * AudioCodec.h
 *
 *  Created on: 1 февр. 2018 г.
 *      Author: bekir
 */

#ifndef INC_AUDIOCODEC_H_
#define INC_AUDIOCODEC_H_

typedef struct
{
	unsigned int index;
	unsigned char name[33];
	unsigned int capability;
	unsigned int mode;
}AUDIOPARAM;

int Read_Audio_Param(char* path, AUDIOPARAM* param_ptr);
int Play_Sound(void);
int Record_Sound(void);

#endif /* INC_AUDIOCODEC_H_ */
