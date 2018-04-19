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


#include "Display.h"
#include "AudioCodec.h"
#include "FrameBuffer.h"
#include "eeprom.h"
#include "PreAsm.h"

Menu PreAsm;
Menu FullTest;
Menu PostAsm;
Menu GI;
Menu ShipMode;
Menu Exit;
Menu Download;

Menu FullTest;
Menu EEPROMTest;
Menu MemTest;
Menu AccelTest;
Menu PowerManTest;
Menu LightSensorTest;
Menu PressSensorTest;
Menu EMMYTest;
Menu ModemTest;
Menu GPSTest;
Menu AudiotTest;

Menu FullTestSub;
Menu EEPROMTestSub;
Menu MemTestSub;
Menu AccelTestSub;
Menu PowerManTestSub;
Menu LightSensorTestSub;
Menu PressSensorTestSub;
Menu EMMYTestSub;
Menu ModemTestSub;
Menu GPSTestSub;
Menu AudiotTestSub;


//Menu PreAsmTest;
Menu* active_menu;

/*
extern unsigned char flag_for_pre_asm;

extern int CX;
extern int CY;
extern int CZ;
extern char LightDataBuffer[100];
extern float temperature;
extern float pressure;
extern char EmmyWiFiBuffer[1024];
extern char EmmyBTBuffer[1024];
extern char SaraBuffer[1024];
extern char LaraBuffer[1024];
//extern char AudioCodecBuffer[1024];
extern char AccelBuffer[1024];
extern char SerialNumber[100];
extern char SerialNumber_1[100];


extern pthread_t preasm_thread;	//preasm test thread
extern int id_preasm_thread;
*/

extern int fd_fb;

pthread_mutex_t mutex;
unsigned char thread_flag=0;

int DisplayOut(char* buf)
{
	char cnt_byte=0;

	cnt_byte=sizeof(buf);
	//memset(buf, 0, cnt_byte);
		if (write(fd_fb, buf, cnt_byte)==cnt_byte)
		{
			return 0;
		}
		else return -1;
}

void PreAsmDisp (void)
{
	char buf[200];
	char cnt_byte;

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[37;40m");
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
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[11;22H*\x1b[12;22H*\x1b[13;22H*\x1b[21;22H*\x1b[14;22H*\x1b[15;22H*\x1b[16;22H*\x1b[17;22H*\x1b[18;22H*\x1b[19;22H*\x1b[20;22H*\x1b[21;22H*\x1b[22;22H*\x1b[23;22H*\x1b[24;22H*\x1b[25;22H*\x1b[26;22H*");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[11;84H*\x1b[12;84H*\x1b[13;84H*\x1b[14;84H*\x1b[15;84H*\x1b[16;84H*\x1b[17;84H*\x1b[18;84H*\x1b[19;84H*\x1b[20;84H*\x1b[21;84H*\x1b[22;84H*\x1b[23;84H*\x1b[24;84H*\x1b[25;84H*\x1b[26;84H*");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[27;22H***************************************************************\n");
	write(fd_fb, buf, cnt_byte);


	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[25;41H \n\n"); //23
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[17;41H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[15;41H>1. Pre-Assembly Testing\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37;40m\x1b[41C2. Post-Assembly Testing\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[41C3. System Recovery\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[41C4. Power Down to Shipping Mode\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[41C5. Exit - Boot OS\n\n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[41C6. Enter Into Download Mode OS\n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void PreAsmAct (void)
{
	/*static int trhread_state=-1;

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
	}*/
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
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[37;40m");
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

	system("mke2fs /dev/mmcblk1p6"); // format CACHE partition
	sleep(1);
	Write_EEPROM("1", 0);	// write eeprom 1, after reboot restore process will be srart
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
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[37;40m");
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

	Write_EEPROM("2", 0);	// write eeprom 2, after reboot android will be srart
	sleep(2);

	system("i2cset -y -f 2 0x68 0x19 0x534D w");	// ship mode

}

void ExitDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[21;41H \n\n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[40C>\n\n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[25;41H \n\n");
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
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[37;40m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 100);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[18;47HBoot OS...");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 100);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

	Write_EEPROM("2", 0);	// write eeprom 2, after reboot android will be srart
	sleep(2);

	system("reboot");
	//pthread_exit(0);
}

void DownloadDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[15;41H \n\n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[17;41H \n\n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[23;41H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[40C>\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void DownloadAct (void)
{
	char buf[100];
	char cnt_byte;

	memset(buf, 0, 100);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[37;40m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 100);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[18;32HFirmware flashing will start after reboot...");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 100);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

	Write_EEPROM("3", 0);	// write eeprom 3, after reboot android will be srart flashing
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

void FullTestDisp(void)
{
	char buf[200];
	char cnt_byte;

	//thread_flag=0;
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0;0H");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37m;40\x1b[0;0H*********************************************************************************************************");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37;40m\x1b[1;0H*\x1b[2;0H*\x1b[3;0H*\x1b[4;0H*\x1b[5;0H*\x1b[6;0H*\x1b[7;0H*\x1b[8;0H*\x1b[9;0H*\x1b[1;0H*\x1b[10;0H*\x1b[11;0H*\x1b[12;0H*\x1b[13;0H*\x1b[14;0H*\x1b[15;0H*\x1b[16;0H*\x1b[17;0H*");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37;40m\x1b[18;0H*\x1b[19;0H*\x1b[20;0H*\x1b[21;0H*\x1b[22;0H*\x1b[23;0H*\x1b[24;0H*\x1b[25;0H*\x1b[26;0H*\x1b[27;0H*\x1b[28;0H*\x1b[29;0H*\x1b[30;0H*\x1b[31;0H*\x1b[32;0H*\x1b[33;0H*\x1b[34;0H*\x1b[35;0H*\x1b[36;0H*");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37;40m\x1b[1;106H*\x1b[2;106H*\x1b[3;106H*\x1b[4;106H*\x1b[5;106H*\x1b[6;106H*\x1b[7;106H*\x1b[8;106H*\x1b[9;106H*\x1b[1;106H*\x1b[10;106H*\x1b[11;106H*\x1b[12;106H*\x1b[13;106H*\x1b[14;106H*\x1b[15;106H*\x1b[16;106H*\x1b[17;106H*");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37;40m\x1b[18;106H*\x1b[19;106H*\x1b[20;106H*\x1b[21;106H*\x1b[22;106H*\x1b[23;106H*\x1b[24;106H*\x1b[25;106H*\x1b[26;106H*\x1b[27;106H*\x1b[28;106H*\x1b[29;106H*\x1b[30;106H*\x1b[31;106H*\x1b[32;106H*\x1b[33;106H*\x1b[34;106H*\x1b[35;106H*\x1b[36;106H*");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37;40m\x1b[36;0H*********************************************************************************************************");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37;40m\x1b[4;0H*********************************************************************************************************");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37;40m\x1b[2;38H===== Pre-Assembly Test =====\x1b[0m");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[25;30H \n\n"); //23
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[16;30H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[15;30H>0. Run All Tests\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37;40m\x1b[30C1. EEPROM Memory Integrity Check / Serial Number Burn-in\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[30C2. Storage Memory Integrity Check\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[30C3. Accelerometer Test/Calibration\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[30C4. Power Management Test\n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[30C5. Light Sensor Test\n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[30C6. Pressure Sensor Test\n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[30C7. On-Board Wireless Module (EMMY) Test\n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[30C8. Modem Port Communication Tests\n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[30C9. External GPS Test\n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[30C10. Audio System Test\n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void FullTestAct(void)
{

}

void FullTestSubDisp (void)
{
	char buf[200];
	char cnt_byte;

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0;0H");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void FullTestSubAct (void)
{
	char buf[200];
	char cnt_byte;

	FuncAccelerometer_Calibration(1);
	sleep(2);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

	FuncConfirm_PMIC_Communication(1);
	FuncConfirm_Battery_Charger_Communication(1);
	sleep(2);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

	FuncAmbient_Light_Sensor_Functionality(1);
	sleep(2);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

	FuncBarometer_Functionality(1);
	sleep(2);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

	FuncEMMY_163_Connectivity_Check(1);
	sleep(2);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

	FuncSARA_Module_Testing_Power_Antenna_Permission(1);
	FuncLARA_Module_Testing_Power_Antenna_Permission(1);
	sleep(2);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

	NEO_Test(1);
	sleep(2);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

	 Audio_Codec_Test(1);
	sleep(2);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void EEPROMTestAct(void)
{

}

void EEPROMTestDisp(void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[17;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[15;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[29C>\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

}

void MemTestDisp(void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[18;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[16;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[29C>\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void MemTestAct(void)
{

}

void AccelTestDisp(void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[19;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[17;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[29C>\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void AccelTestAct(void)
{

}

void AccelTestSubDisp(void)
{
char buf[200];
char cnt_byte;

memset(buf, 0, 200);
cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
write(fd_fb, buf, cnt_byte);
memset(buf, 0, 200);
cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0;0H");
write(fd_fb, buf, cnt_byte);

/*memset(buf, 0, 200);
cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37m;40\x1b[0;0H*********************************************************************************************************");
write(fd_fb, buf, cnt_byte);
memset(buf, 0, 200);
cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37;40m\x1b[1;0H*\x1b[2;0H*\x1b[3;0H*\x1b[4;0H*\x1b[5;0H*\x1b[6;0H*\x1b[7;0H*\x1b[8;0H*\x1b[9;0H*\x1b[1;0H*\x1b[10;0H*\x1b[11;0H*\x1b[12;0H*\x1b[13;0H*\x1b[14;0H*\x1b[15;0H*\x1b[16;0H*\x1b[17;0H*");
write(fd_fb, buf, cnt_byte);
memset(buf, 0, 200);
cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37;40m\x1b[18;0H*\x1b[19;0H*\x1b[20;0H*\x1b[21;0H*\x1b[22;0H*\x1b[23;0H*\x1b[24;0H*\x1b[25;0H*\x1b[26;0H*\x1b[27;0H*\x1b[28;0H*\x1b[29;0H*\x1b[30;0H*\x1b[31;0H*\x1b[32;0H*\x1b[33;0H*\x1b[34;0H*\x1b[35;0H*\x1b[36;0H*");
write(fd_fb, buf, cnt_byte);
memset(buf, 0, 200);
cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37;40m\x1b[1;106H*\x1b[2;106H*\x1b[3;106H*\x1b[4;106H*\x1b[5;106H*\x1b[6;106H*\x1b[7;106H*\x1b[8;106H*\x1b[9;106H*\x1b[1;106H*\x1b[10;106H*\x1b[11;106H*\x1b[12;106H*\x1b[13;106H*\x1b[14;106H*\x1b[15;106H*\x1b[16;106H*\x1b[17;106H*");
write(fd_fb, buf, cnt_byte);
memset(buf, 0, 200);
cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37;40m\x1b[18;106H*\x1b[19;106H*\x1b[20;106H*\x1b[21;106H*\x1b[22;106H*\x1b[23;106H*\x1b[24;106H*\x1b[25;106H*\x1b[26;106H*\x1b[27;106H*\x1b[28;106H*\x1b[29;106H*\x1b[30;106H*\x1b[31;106H*\x1b[32;106H*\x1b[33;106H*\x1b[34;106H*\x1b[35;106H*\x1b[36;106H*");
write(fd_fb, buf, cnt_byte);
memset(buf, 0, 200);
cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37;40m\x1b[36;0H*********************************************************************************************************");
write(fd_fb, buf, cnt_byte);
memset(buf, 0, 200);
cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37;40m\x1b[4;0H*********************************************************************************************************");
write(fd_fb, buf, cnt_byte);
memset(buf, 0, 200);
cnt_byte=snprintf(buf, sizeof(buf), "\x1b[37;40m\x1b[2;38H===== Accelerometer Test =====\x1b[0m");
write(fd_fb, buf, cnt_byte);*/

memset(buf, 0, 200);
cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
write(fd_fb, buf, cnt_byte);
memset(buf, 0, 200);
cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
write(fd_fb, buf, cnt_byte);
}

void AccelTestSubAct(void)
{
	char buf[200];
	char cnt_byte;

	FuncAccelerometer_Calibration(1);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

}

void PowerManTestDisp(void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[20;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[18;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[29C>\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void PowerManTestAct(void)
{

}

void PowerManTestSubDisp(void)
{
	char buf[200];
	char cnt_byte;

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0;0H");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void PowerManTestSubAct(void)
{
	char buf[200];
	char cnt_byte;

	FuncConfirm_PMIC_Communication(1);
	FuncConfirm_Battery_Charger_Communication(1);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);

}

void LightSensorTestDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[21;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[19;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[29C>\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void LightSensorTestAct (void)
{

}

void LightSensorTestSubDisp (void)
{
	char buf[200];
	char cnt_byte;

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0;0H");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void LightSensorTestSubAct (void)
{
	char buf[200];
	char cnt_byte;

	FuncAmbient_Light_Sensor_Functionality(1);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void PressSensorTestDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[22;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[20;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[29C>\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void PressSensorTestAct (void)
{

}

void PressSensorTestSubAct (void)
{
	char buf[200];
	char cnt_byte;

	FuncBarometer_Functionality(1);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void PressSensorTestSubDisp (void)
{
	char buf[200];
	char cnt_byte;

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0;0H");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void EMMYTestDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[23;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[21;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[29C>\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void EMMYTestAct (void)
{

}

void EMMYTestSubDisp (void)
{
	char buf[200];
	char cnt_byte;

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0;0H");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void EMMYTestSubAct (void)
{
	char buf[200];
	char cnt_byte;

	FuncEMMY_163_Connectivity_Check(1);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void ModemTestDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[24;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[22;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[29C>\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void ModemTestAct (void)
{

}

void ModemTestSubDisp (void)
{
	char buf[200];
	char cnt_byte;

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0;0H");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void ModemTestSubAct (void)
{
	char buf[200];
	char cnt_byte;

	FuncSARA_Module_Testing_Power_Antenna_Permission(1);
	FuncLARA_Module_Testing_Power_Antenna_Permission(1);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void GPSTestDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[25;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[23;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[29C>\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void GPSTestAct (void)
{

}

void GPSTestSubDisp (void)
{
	char buf[200];
	char cnt_byte;

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0;0H");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void GPSTestSubAct (void)
{
	char buf[200];
	char cnt_byte;

	NEO_Test(1);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void AudiotTestDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[15;30H \n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[17;30H \n");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[24;30H \n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[29C>\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void AudiotTestAct (void)
{

}

void AudioTestSubDisp (void)
{
	char buf[200];
	char cnt_byte;

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[0;0H");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[35;34H\x1b[33mVOL BUTTON - ESC CENTRAL BUTTON - ENTER\x1b[0m");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void AudiotTestSubAct (void)
{
	char buf[200];
	char cnt_byte;

	Audio_Codec_Test(1);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[36;0H");
	write(fd_fb, buf, cnt_byte);
}

void MenuInit (void)
{
	PreAsm.DOWN=&PostAsm;
	PreAsm.UP=&Download;
	PreAsm.ENTER=&FullTest;//&PreAsm;
	PreAsm.ESC=&PreAsm;
	PreAsm.menudisplay=&PreAsmDisp;
	PreAsm.menuaction=&PreAsmAct;

	FullTest.DOWN=&EEPROMTest;
	FullTest.UP=&AudiotTest;
	FullTest.ENTER=&FullTestSub;
	FullTest.ESC=&PreAsm;
	FullTest.menudisplay=&FullTestDisp;
	FullTest.menuaction=&FullTestAct;

	FullTestSub.DOWN=NULL;
	FullTestSub.UP=NULL;
	FullTestSub.ENTER=&FullTestSub;
	FullTestSub.ESC=&FullTest;
	FullTestSub.menuaction=&FullTestSubAct;
	FullTestSub.menudisplay=&FullTestSubDisp;

	EEPROMTest.DOWN=&MemTest;
	EEPROMTest.UP=&FullTest;
	EEPROMTest.ENTER=&EEPROMTest;//EEPROMTestSub;
	EEPROMTest.ESC=&PreAsm;
	EEPROMTest.menuaction=&EEPROMTestAct;
	EEPROMTest.menudisplay=&EEPROMTestDisp;

	MemTest.DOWN=&AccelTest;
	MemTest.UP=&EEPROMTest;
	MemTest.ENTER=&MemTest;//MemTestSub;
	MemTest.ESC=&PreAsm;
	MemTest.menuaction=&MemTestAct;
	MemTest.menudisplay=&MemTestDisp;

	AccelTest.DOWN=&PowerManTest;
	AccelTest.UP=&MemTest;
	AccelTest.ENTER=&AccelTestSub;
	AccelTest.ESC=&PreAsm;
	AccelTest.menuaction=&AccelTestAct;
	AccelTest.menudisplay=&AccelTestDisp;

	AccelTestSub.DOWN=NULL;//&AccelTestSub;
	AccelTestSub.UP=NULL;//&AccelTestSub;
	AccelTestSub.ESC=&FullTest;
	AccelTestSub.ENTER=&AccelTestSub;
	AccelTestSub.menuaction=&AccelTestSubAct;
	AccelTestSub.menudisplay=&AccelTestSubDisp;

	PowerManTest.DOWN=&LightSensorTest;
	PowerManTest.UP=&AccelTest;
	PowerManTest.ENTER=&PowerManTestSub;
	PowerManTest.ESC=&PreAsm;
	PowerManTest.menuaction=&PowerManTestAct;
	PowerManTest.menudisplay=&PowerManTestDisp;

	PowerManTestSub.DOWN=NULL;
	PowerManTestSub.UP=NULL;
	PowerManTestSub.ESC=&FullTest;
	PowerManTestSub.ENTER=&PowerManTestSub;
	PowerManTestSub.menuaction=&PowerManTestSubAct;
	PowerManTestSub.menudisplay=&PowerManTestSubDisp;


	LightSensorTest.DOWN=&PressSensorTest;
	LightSensorTest.UP=&PowerManTest;
	LightSensorTest.ENTER=&LightSensorTestSub;
	LightSensorTest.ESC=&PreAsm;
	LightSensorTest.menuaction=&LightSensorTestAct;
	LightSensorTest.menudisplay=&LightSensorTestDisp;

	LightSensorTestSub.DOWN=NULL;
	LightSensorTestSub.UP=NULL;
	LightSensorTestSub.ENTER=&LightSensorTestSub;
	LightSensorTestSub.ESC=&FullTest;
	LightSensorTestSub.menuaction=&LightSensorTestSubAct;
	LightSensorTestSub.menudisplay=&LightSensorTestSubDisp;


	PressSensorTest.DOWN=&EMMYTest;
	PressSensorTest.UP=&LightSensorTest;
	PressSensorTest.ENTER=&PressSensorTestSub;
	PressSensorTest.ESC=&PreAsm;
	PressSensorTest.menuaction=&PressSensorTestAct;
	PressSensorTest.menudisplay=&PressSensorTestDisp;

	PressSensorTestSub.DOWN=NULL;
	PressSensorTestSub.UP=NULL;
	PressSensorTestSub.ENTER=&PressSensorTestSub;
	PressSensorTestSub.ESC=&FullTest;
	PressSensorTestSub.menuaction=&PressSensorTestSubAct;
	PressSensorTestSub.menudisplay=&PressSensorTestSubDisp;


	EMMYTest.DOWN=&ModemTest;
	EMMYTest.UP=&PressSensorTest;
	EMMYTest.ENTER=&EMMYTestSub;
	EMMYTest.ESC=&PreAsm;
	EMMYTest.menuaction=&EMMYTestAct;
	EMMYTest.menudisplay=&EMMYTestDisp;

	EMMYTestSub.DOWN=NULL;
	EMMYTestSub.UP=NULL;
	EMMYTestSub.ENTER=&EMMYTestSub;
	EMMYTestSub.ESC=&FullTest;
	EMMYTestSub.menuaction=&EMMYTestSubAct;
	EMMYTestSub.menudisplay=&EMMYTestSubDisp;

	ModemTest.DOWN=&GPSTest;
	ModemTest.UP=&EMMYTest;
	ModemTest.ENTER=&ModemTestSub;
	ModemTest.ESC=&PreAsm;
	ModemTest.menuaction=&ModemTestAct;
	ModemTest.menudisplay=&ModemTestDisp;

	ModemTestSub.DOWN=NULL;
	ModemTestSub.UP=NULL;
	ModemTestSub.ENTER=&ModemTestSub;
	ModemTestSub.ESC=&FullTest;
	ModemTestSub.menuaction=&ModemTestSubAct;
	ModemTestSub.menudisplay=&ModemTestSubDisp;

	GPSTest.DOWN=&AudiotTest;
	GPSTest.UP=&ModemTest;
	GPSTest.ENTER=&GPSTestSub;
	GPSTest.ESC=&PreAsm;
	GPSTest.menuaction=&GPSTestAct;
	GPSTest.menudisplay=&GPSTestDisp;

	GPSTestSub.DOWN=NULL;
	GPSTestSub.UP=NULL;
	GPSTestSub.ENTER=&GPSTestSub;
	GPSTestSub.ESC=&FullTest;
	GPSTestSub.menuaction=&GPSTestSubAct;
	GPSTestSub.menudisplay=&GPSTestSubDisp;

	AudiotTest.DOWN=&FullTest;
	AudiotTest.UP=&GPSTest;
	AudiotTest.ENTER=&AudiotTestSub;
	AudiotTest.ESC=&PreAsm;
	AudiotTest.menuaction=&AudiotTestAct;
	AudiotTest.menudisplay=&AudiotTestDisp;


	AudiotTestSub.DOWN=NULL;
	AudiotTestSub.UP=NULL;
	AudiotTestSub.ENTER=&AudiotTestSub;
	AudiotTestSub.ESC=&FullTest;
	AudiotTestSub.menuaction=&AudiotTestSubAct;
	AudiotTestSub.menudisplay=&AudioTestSubDisp;

	PostAsm.DOWN=&GI;
	PostAsm.UP=&PreAsm;
	PostAsm.ENTER=&PostAsm;
	PostAsm.ESC=&PostAsm;
	PostAsm.menudisplay=&PostAsmDisp;
	PostAsm.menuaction=&PostAsmAct;

	GI.DOWN=&ShipMode;
	GI.UP=&PostAsm;
	GI.ENTER=&GI;
	GI.ESC=&GI;
	GI.menudisplay=&GIDisp;
	GI.menuaction=&GIAct;

	ShipMode.DOWN=&Exit;
	ShipMode.UP=&GI;
	ShipMode.ENTER=&ShipMode;
	ShipMode.ESC=&ShipMode;
	ShipMode.menudisplay=&ShipModeDisp;
	ShipMode.menuaction=&ShipModeAct;

	Exit.DOWN=&Download;
	Exit.UP=&ShipMode;
	Exit.ENTER=&Exit;
	Exit.ESC=&Exit;
	Exit.menudisplay=&ExitDisp;
	Exit.menuaction=&ExitAct;

	//PreAsmTest.DOWN=&PreAsmTest;
	//PreAsmTest.UP=&PreAsmTest;
	//PreAsmTest.ENTER=&PreAsm;
	//PreAsmTest.menuaction=&PreAsmTestDisp;
	//PreAsmTest.menuaction=&PreAsmTestAct;

	Download.DOWN=&PreAsm;
	Download.UP=&Exit;
	Download.ENTER=&Download;
	Download.ESC=NULL;
	Download.menudisplay=&DownloadDisp;
	Download.menuaction=&DownloadAct;

	char buf[200];
	char cnt_byte;

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[37;40m");
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
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[11;22H*\x1b[12;22H*\x1b[13;22H*\x1b[21;22H*\x1b[14;22H*\x1b[15;22H*\x1b[16;22H*\x1b[17;22H*\x1b[18;22H*\x1b[19;22H*\x1b[20;22H*\x1b[21;22H*\x1b[22;22H*\x1b[23;22H*\x1b[24;22H*\x1b[25;22H*\x1b[26;22H*");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[11;84H*\x1b[12;84H*\x1b[13;84H*\x1b[14;84H*\x1b[15;84H*\x1b[16;84H*\x1b[17;84H*\x1b[18;84H*\x1b[19;84H*\x1b[20;84H*\x1b[21;84H*\x1b[22;84H*\x1b[23;84H*\x1b[24;84H*\x1b[25;84H*\x1b[26;84H*");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[27;22H***************************************************************\n");
	write(fd_fb, buf, cnt_byte);
	active_menu=&PreAsm;
	active_menu->menudisplay();
}

/*
void* preasm_thread_func(void* thread_data)
{

}
*/


