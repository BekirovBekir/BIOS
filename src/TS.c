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
#include <stdio_ext.h>
#include <termios.h>
#include <sys/epoll.h>

#include "i2c.h"
#include "TS.h"
#include "ComPort.h"
#include "FrameBuffer.h"
#include "Display.h"
#include "CellTest.h"

#define DEBUG 1

#if (DEBUG > 0)
	#define DBG(x)	(x)
#else
	#define DBG(x)
#endif

extern Menu* active_menu;
extern Menu PreAsm;
extern Menu FullTest;
extern Menu FullTestPostAsm;
extern Menu CellTestUART;
extern Menu CellTestUSB;
extern Menu CellTestUARTSub;
extern Menu CellTestUSBSub;
extern int timer_tick;

extern pthread_mutex_t mutex;
extern pthread_t cell_passthrough_thread;	// thread for select test from USB port

int preasm_flag=0;
extern int test_run_flag;

unsigned char pre_asm_active=0;
unsigned char flag_for_pre_asm=0;

FILE* f_desc_usb=NULL;

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
							if (key->key_num==4)
							{
								if (active_menu->UP!=NULL)
								{
									active_menu=active_menu->UP;
									active_menu->menudisplay();
								}
							}
							if (key->key_num==3)
							{
								if (active_menu->DOWN!=NULL)
								{
									active_menu=active_menu->DOWN;
									active_menu->menudisplay();
								}
							}
							if (key->key_num==2)
							{
								active_menu=active_menu->ENTER;

									if (active_menu==&FullTest) test_run_flag=1;
									if (active_menu==&FullTestPostAsm) test_run_flag=2;
									if (active_menu==&CellTestUART) test_run_flag=3;
									if (active_menu==&PreAsm) test_run_flag=4;

								pthread_mutex_lock(&mutex);
								preasm_flag=((active_menu==&FullTest) ? 1 : 0);
								//printf("PreAsm flag = %i\n", preasm_flag);
								pthread_mutex_unlock(&mutex);

								if (f_desc_usb!=NULL) tcflush(fileno(f_desc_usb), TCIOFLUSH);
								active_menu->menudisplay();
								active_menu->menuaction();

								preasm_flag=1;

							}
							if ((key->key_num==1))
							{
								if (active_menu->ESC!=NULL)
								{
									active_menu=active_menu->ESC;

									if (active_menu==&FullTest) test_run_flag=1;
									if (active_menu==&FullTestPostAsm) test_run_flag=2;
									if (active_menu==&CellTestUART) test_run_flag=3;
									if (active_menu==&PreAsm) test_run_flag=4;

									if (active_menu == &CellTestUART || active_menu == &CellTestUSB) {
										pthread_mutex_lock(&mutex);
										termState = false;
										pthread_mutex_unlock(&mutex);
										pthread_join(cell_passthrough_thread, NULL);
									}

									//if (active_menu!=NULL) active_menu->menudisplay();
									active_menu->menudisplay();
									active_menu->menuaction(); //

									pthread_mutex_lock(&mutex);
									preasm_flag=((active_menu==&FullTest) ? 1 : 0);
									//printf("PreAsm flag = %i\n", preasm_flag);
									pthread_mutex_unlock(&mutex);

									preasm_flag=1; // !!!!!!!!!! if need delete !!!!!!!!!!!
								}

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






