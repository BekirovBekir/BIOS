/*
 * TS.c
 *
 *  Created on: 26 дек. 2017 г.
 *      Author: bekir
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <error.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>

#include "i2c.h"
#include "TS.h"
#include "ComPort.h"
#include "FrameBuffer.h"
#include "Display.h"

#define DEBUG 1

#if (DEBUG > 0)
	#define DBG(x)	(x)
#else
	#define DBG(x)
#endif

extern Menu* active_menu;
extern Menu PostAsm;
extern Menu Download;
extern int timer_tick;

unsigned char pre_asm_active=0;
unsigned char flag_for_pre_asm=0;

// Function scanning TS and return struct with key parameters
ilitek_key_info Scan_TS_Key(void)
{
	unsigned char buf[31]={0};
	ilitek_key_info key={0,0,0,0};
	Read_i2c(0x10, buf, 31);
		for (unsigned int i=0; i<=4; i++)
		{
			key.key_num=-1;
			key.status=0;
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
						//DBG(printf("key%i %i, %i, %i\n", i, key.status, key.x, key.y));
						return key;
					}
				}
		}
	key.key_num=-1;
	key.status=0;
	key.x=0;
	key.y=0;
	//DBG(printf("key%i %i, %i, %i\n", -1, key.status, key.x, key.y));
	return key;
}

// finite state machine for TS
void FSM_TS (ilitek_key_info* key)
{
	static unsigned char fsm=0;
	static ilitek_key_info _key={0,0,0,0};

		switch (fsm)
		{
			case 0:
				if (key->status)
				{
					fsm=1;
					_key.key_num=key->key_num;
					_key.status=key->status;
					_key.x=key->x;
					_key.y=key->y;
					//DBG(printf("fsm - 0\n"));
				}
			break;
			case 1:
					if (((_key.status==key->status) && (_key.key_num==key->key_num)))
					{
						DBG(printf("key_pressed: %i\n", key->key_num));
						/*char buf[50];
						char cnt_byte;
						memset(buf, 0, 50);
						cnt_byte=snprintf(buf, sizeof(buf), "\x1b[0;0Hkey_pressed: %i\n", key->key_num);
						write(fd_fb, buf, cnt_byte);*/
							if ((key->key_num==4) && (pre_asm_active==0))
							{
								active_menu=active_menu->UP;
								active_menu->menudisplay();
							}
							if ((key->key_num==3) && (pre_asm_active==0))
							{
								active_menu=active_menu->DOWN;
								active_menu->menudisplay();
							}
							if ((key->key_num==2) && (flag_for_pre_asm==0))
							{
								active_menu=active_menu->ENTER;
									if ((active_menu!=&PostAsm) && (active_menu!=&Download)) // dummy for Post_asm test menu
									{
											pre_asm_active^=(1<<0);
											flag_for_pre_asm=(pre_asm_active ? 1 : 0);
											//if (pre_asm_active) flag_for_pre_asm=1;
											//else flag_for_pre_asm=0;
									}
								active_menu->menuaction();
							}
						timer_tick=0;
						fsm=2;
					}
					else
					{
						fsm=0;
					}
			break;
			case 2:
				if (((key->status==0) && (_key.key_num!=key->key_num)))
				{
					fsm=0;
					//DBG(printf("fsm - 2\n"));
				}
			break;

		}
}






