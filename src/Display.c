/*
 * Display.c
 *
 *  Created on: 27 дек. 2017 г.
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


#include "../inc/Display.h"
#include "../inc/FrameBuffer.h"
#include "../inc/eeprom.h"
#include "../inc/PreAsm.h"

Menu PreAsm;
Menu PostAsm;
Menu GI;
Menu ShipMode;
Menu Exit;
//Menu PreAsmTest;
Menu* active_menu;

extern int CX;
extern int CY;
extern int CZ;
extern char LightDataBuffer[100];
extern float temperature;
extern float pressure;
extern char EmmyWiFiBuffer[1024];
extern char EmmyBTBuffer[1024];

extern int fd_fb;
extern pthread_t preasm_thread;	//preasm test thread
extern int id_preasm_thread;

pthread_mutex_t mutex;
unsigned char thread_flag=0;

void PreAsmDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[23;41H \n\n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[17;41H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[15;41H>1. Pre-Assembly Testing\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[31;40m\x1b[41C2. Post-Assembly Testing\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[41C3. System Recovery\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[41C4. Power Down to Shipping Mode\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[41C5. Exit - Boot OS\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void PreAsmAct (void)
{
	static int trhread_state=-1;

	if (trhread_state==-1)
	{
		trhread_state=pthread_create(&preasm_thread, NULL, preasm_thread_func, &id_preasm_thread);
			if (trhread_state==0)
			{
				printf ("\nPreasm thread started\n");
			}
			else
			{
				perror("\nPreasm thread fail\n");
			}
	}
	else
	{
		pthread_mutex_lock(&mutex);
			if (thread_flag==1)
			{
				trhread_state=-1;
				printf("\nPreasm thread stop\n");
				//pthread_cancel(preasm_thread);
				pthread_join(preasm_thread, NULL);
				MenuInit();
			}
		pthread_mutex_unlock(&mutex);
	}
}

void PostAsmAct (void)
{

}

void PostAsmDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[19;41H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[15;41H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[40C>\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

}

void GIDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[21;41H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[17;41H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[40C>\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void GIAct (void)
{
	char buf[100];
	char cnt_byte;

	memset(buf, 0, 100);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[31;40m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 100);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[17;30HRestore of the system will be start after reboot\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 100);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[19;49HReboot...\n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 100);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

	Write_EEPROM("1");	// write eeprom 1, after reboot restore process will be srart
	sleep(2);

	//system("reboot");
	pthread_exit(0);

}

void ShipModeDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[23;41H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[19;41H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[40C>\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void ShipModeAct (void)
{
	char buf[100];
	char cnt_byte;

	memset(buf, 0, 100);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[31;40m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 100);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[17;44HShip mode active");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 100);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[19;47HPower off...");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 100);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

	Write_EEPROM("2");	// write eeprom 2, after reboot android will be srart
	sleep(2);

	system("i2cset -y -f 2 0x68 0x19 0x534D w");	// ship mode

}

void ExitDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[15;41H \n\n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[21;41H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[40C>\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void ExitAct (void)
{
	char buf[100];
	char cnt_byte;

	memset(buf, 0, 100);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[31;40m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 100);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[18;47HBoot OS...");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 100);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

	Write_EEPROM("2");	// write eeprom 2, after reboot android will be srart
	sleep(2);

	//system("reboot");
	pthread_exit(0);
}

void PreAsmTestDisp(void)
{

}

void PreAsmTestAct(void)
{

}

void MenuInit (void)
{
	PreAsm.DOWN=&PostAsm;
	PreAsm.UP=&Exit;
	PreAsm.ENTER=&PreAsm;
	PreAsm.menudisplay=&PreAsmDisp;
	PreAsm.menuaction=&PreAsmAct;

	PostAsm.DOWN=&GI;
	PostAsm.UP=&PreAsm;
	PostAsm.ENTER=&PostAsm;
	PostAsm.menudisplay=&PostAsmDisp;
	PostAsm.menuaction=&PostAsmAct;

	GI.DOWN=&ShipMode;
	GI.UP=&PostAsm;
	GI.ENTER=&GI;
	GI.menudisplay=&GIDisp;
	GI.menuaction=&GIAct;

	ShipMode.DOWN=&Exit;
	ShipMode.UP=&GI;
	ShipMode.ENTER=&ShipMode;
	ShipMode.menudisplay=&ShipModeDisp;
	ShipMode.menuaction=&ShipModeAct;

	Exit.DOWN=&PreAsm;
	Exit.UP=&ShipMode;
	Exit.ENTER=&Exit;
	Exit.menudisplay=&ExitDisp;
	Exit.menuaction=&ExitAct;

	//PreAsmTest.DOWN=&PreAsmTest;
	//PreAsmTest.UP=&PreAsmTest;
	//PreAsmTest.ENTER=&PreAsm;
	//PreAsmTest.menuaction=&PreAsmTestDisp;
	//PreAsmTest.menuaction=&PreAsmTestAct;

	char buf[200];
	char cnt_byte;

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[31;40m");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[10;22H***************************************************************\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[11;42H=====BIOS PROGRAM=====\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[13;22H***************************************************************\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[11;22H*\x1b[12;22H*\x1b[13;22H*\x1b[21;22H*\x1b[14;22H*\x1b[15;22H*\x1b[16;22H*\x1b[17;22H*\x1b[18;22H*\x1b[19;22H*\x1b[20;22H*\x1b[21;22H*\x1b[22;22H*\x1b[23;22H*\x1b[24;22H*\x1b[25;22H*");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[11;84H*\x1b[12;84H*\x1b[13;84H*\x1b[14;84H*\x1b[15;84H*\x1b[16;84H*\x1b[17;84H*\x1b[18;84H*\x1b[19;84H*\x1b[20;84H*\x1b[21;84H*\x1b[22;84H*\x1b[23;84H*\x1b[24;84H*\x1b[25;84H*");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[26;22H***************************************************************\n");
	write(fd_fb, buf, cnt_byte);
	active_menu=&PreAsm;
	active_menu->menudisplay();
}

void* preasm_thread_func(void* thread_data)
{
	char buf[200];
	char cnt_byte;
	//unsigned char state_test=0;

	thread_flag=0;
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0;0H");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[31m;40\x1b[0;0H*********************************************************************************************************");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[31;40m\x1b[1;0H*\x1b[2;0H*\x1b[3;0H*\x1b[4;0H*\x1b[5;0H*\x1b[6;0H*\x1b[7;0H*\x1b[8;0H*\x1b[9;0H*\x1b[1;0H*\x1b[10;0H*\x1b[11;0H*\x1b[12;0H*\x1b[13;0H*\x1b[14;0H*\x1b[15;0H*\x1b[16;0H*\x1b[17;0H*");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[31;40m\x1b[18;0H*\x1b[19;0H*\x1b[20;0H*\x1b[21;0H*\x1b[22;0H*\x1b[23;0H*\x1b[24;0H*\x1b[25;0H*\x1b[26;0H*\x1b[27;0H*\x1b[28;0H*\x1b[29;0H*\x1b[30;0H*\x1b[31;0H*\x1b[32;0H*\x1b[33;0H*\x1b[34;0H*\x1b[35;0H*\x1b[36;0H*");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[31;40m\x1b[1;106H*\x1b[2;106H*\x1b[3;106H*\x1b[4;106H*\x1b[5;106H*\x1b[6;106H*\x1b[7;106H*\x1b[8;106H*\x1b[9;106H*\x1b[1;106H*\x1b[10;106H*\x1b[11;106H*\x1b[12;106H*\x1b[13;106H*\x1b[14;106H*\x1b[15;106H*\x1b[16;106H*\x1b[17;106H*");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[31;40m\x1b[18;106H*\x1b[19;106H*\x1b[20;106H*\x1b[21;106H*\x1b[22;106H*\x1b[23;106H*\x1b[24;106H*\x1b[25;106H*\x1b[26;106H*\x1b[27;106H*\x1b[28;106H*\x1b[29;106H*\x1b[30;106H*\x1b[31;106H*\x1b[32;106H*\x1b[33;106H*\x1b[34;106H*\x1b[35;106H*\x1b[36;106H*");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[31;40m\x1b[36;0H*********************************************************************************************************");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[31;40m\x1b[4;0H*********************************************************************************************************");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[31;40m\x1b[2;38H===== Pre-Assembly Test =====\x1b[0m");
	write(fd_fb, buf, cnt_byte);

		//for (;;)
		//{
			//if (state_test==0)
			//{
				if (TestMMC(1)==0)
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[6;3HeMMC TEST:\x1b[32m OK\x1b[0m - Output and input buffer are equal\n\n");
					write(fd_fb, buf, cnt_byte);
				}
				else
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CeMMC TEST:\x1b[31m Fail \x1b[0m\n\n");
					write(fd_fb, buf, cnt_byte);
				}

				if (FuncSPI_32MBit_NOR_Flash(1)==0)
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CSPI_32Mbit_NOR TEST:\x1b[32m OK \x1b[0m\n\n");
					write(fd_fb, buf, cnt_byte);
				}
				else
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CSPI_32Mbit_NOR TEST:\x1b[31m Fail \x1b[0m\n\n");
					write(fd_fb, buf, cnt_byte);
				}

				if (FuncEEPROM(1)==0)
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CEEPROM TEST:\x1b[32m OK\x1b[0m - Output and input buffer are equal\n\n");
					write(fd_fb, buf, cnt_byte);
				}
				else
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CEEPROM TEST:\x1b[31m Fail \x1b[0m\n\n");
					write(fd_fb, buf, cnt_byte);
				}

				if (FuncAccelerometer_Calibration(1)==0)
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CAccelerometer TEST:\x1b[32m OK\x1b[0m - Calibrate values: x = %i; y = %i; z = %i\n\n", CX, CY, CZ);
					write(fd_fb, buf, cnt_byte);
				}
				else
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CAccelerometer TEST:\x1b[31m Fail \x1b[0m\n\n");
					write(fd_fb, buf, cnt_byte);
				}

				if (FuncConfirm_Battery_Charger_Communication(1)==0)
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CCharger LTC4015 TEST:\x1b[32m OK\x1b[0m - LTC4015's telemetry system is valid\n\n");
					write(fd_fb, buf, cnt_byte);
				}
				else
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CCharger ltc4015 TEST:\x1b[31m Fail \x1b[0m\n\n");
					write(fd_fb, buf, cnt_byte);
				}

				if (FuncConfirm_PMIC_Communication(1)==0)
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CPMIC TEST:\x1b[32m OK\x1b[0m - PMIC ID is valid\n\n");
					write(fd_fb, buf, cnt_byte);
				}
				else
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CPMIC TEST:\x1b[31m Fail \x1b[0m\n\n");
					write(fd_fb, buf, cnt_byte);
				}

				if (FuncAmbient_Light_Sensor_Functionality(1)==0)
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CLight Sensor TEST:\x1b[32m OK\x1b[0m - Illumination is %s\n", LightDataBuffer);
					write(fd_fb, buf, cnt_byte);
				}
				else
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CLight Sensor TEST:\x1b[31m Fail \x1b[0m\n\n");
					write(fd_fb, buf, cnt_byte);
				}

				if (FuncBarometer_Functionality(1)==0)
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CPressure Sensor TEST:\x1b[32m OK\x1b[0m - Pressure value is %.2f Temperature is %.2f\n\n", pressure, temperature);
					write(fd_fb, buf, cnt_byte);
				}
				else
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CPressure Sensor TEST:\x1b[31m Fail \x1b[0m\n\n");
					write(fd_fb, buf, cnt_byte);
				}

				memset(buf, 0, 200);
				cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CEMMY TEST:\x1b[33m Please wait!\x1b[0m");
				write(fd_fb, buf, cnt_byte);
				if (FuncEMMY_163_Connectivity_Check(1)==0)
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[23DEMMY TEST:\x1b[32m OK\x1b[0m - %s, %s     \n\n", EmmyWiFiBuffer, EmmyBTBuffer);
					write(fd_fb, buf, cnt_byte);
				}
				else
				{
					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[23DEMMY TEST:\x1b[31m Fail \x1b[0m - %s, %s     \n\n", EmmyWiFiBuffer, EmmyBTBuffer);
					write(fd_fb, buf, cnt_byte);
				}

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;32H\x1b[31mPUSH THE ENTER BUTTON TO EXIT IN BIOS MENU\x1b[0m");
			write(fd_fb, buf, cnt_byte);
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
			write(fd_fb, buf, cnt_byte);
			//state_test=1;
			pthread_mutex_lock(&mutex);
			thread_flag=1;
			pthread_mutex_unlock(&mutex);
			pthread_exit(0);
			//}
		//usleep(1000000);
		//}
}



