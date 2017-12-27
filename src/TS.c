/*
 * TS.c
 *
 *  Created on: 26 дек. 2017 г.
 *      Author: bekir
 */

#include "../inc/i2c.h"
#include "../inc/TS.h"

#define DEBUG 1

#if (DEBUG > 0)
	#define DBG(x)	(x)
#else
	#define DBG(x)
#endif

// Function scanning TS and return struct with key parameters
ilitek_key_info Scan_TS_Key(void)
{
	unsigned char buf[31]={0};
	ilitek_key_info key={0,0,0,0};
	Read_i2c(0x10, buf, 31);
		for (unsigned int i=0; i<=4; i++)
		{
			key.key_num=-1;
			key.status=-1;
			key.x=0;
			key.y=0;
			key.status=buf[(i*5)+1]>>7;
			key.key_num=i;
			key.x=((buf[(i*5)+1]&0x3F)<<8)+buf[(i*5)+2];
			key.y=((buf[(i*5)+3])<<8)+buf[(i*5)+4];
				if (key.status)
				{
					if (key.y==0x25e4)
					{
						DBG(printf("key%i %i, %i, %i\n", i, key.status, key.x, key.y));
						return key;
					}
				}
		}
	key.key_num=-1;
	key.status=-1;
	key.x=0;
	key.y=0;
	DBG(printf("key%i %i, %i, %i\n", -1, key.status, key.x, key.y));
	return key;
}

// finite state machine for TS
void FSM_TS (ilitek_key_info* key)
{
	unsigned char fsm=0;
		switch (fsm)
		{
			case 0:

			break;
			case 1:

			break;

		}
}






