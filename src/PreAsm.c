/*
 * PreAsm.c
 *
 *  Created on: 9 янв. 2018 г.
 *      Author: bekir
 */


#include <stdio.h>
#include <unistd.h>
#include <poll.h>
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


#include "PreAsm.h"
#include "eeprom.h"
#include "i2c.h"
#include "ComPort.h"
#include "GPIO.h"
#include "FrameBuffer.h"
#include "AudioCodec.h"

#define BUF_SIZE_DISP 200

extern FILE *stdin;
extern int fd_fb;

#define USB_PATH "/dev/ttyGS0"


int CX=0;
int CY=0;
int CZ=0;
char LightDataBuffer[100]={0};
float temperature = 0;
float pressure = 0;
char EmmyWiFiBuffer[1024]={0};
char EmmyBTBuffer[1024]={0};
char SaraBuffer[1024]={0};
char LaraBuffer[1024]={0};
char AudioCodecBuffer[1024]={0};
char AccelBuffer[1024]={0};
char SerialNumber[100]={0};
char SerialNumber_1[100]={0};
unsigned char wifi_first_start_flag=0;
struct pollfd newpoll={ STDIN_FILENO, POLLIN|POLLPRI};

int USB_printf(char* buf, int timeout)
{
	int fd=-1;
	int cnt=0;

	fd=open(USB_PATH, O_WRONLY);
		if (fd>=0)
		{
			struct pollfd polldesc={fd, POLLOUT|POLLPRI};
				if(poll(&polldesc, 1, timeout))
				{
					cnt=write(fd, buf, strlen(buf));
					close(fd);
				}
				{
					close(fd);
					return -1;
				}
		}
		else return -1;
	return cnt;
}

int TestMMC(int Do)
{
	//Performs checksum validation on all memory parts to ensure memory is free of corruption or defect. This includes testing
	#define BUFFER_SIZE 100

	char buffer_out[BUFFER_SIZE];
	char buffer_in[BUFFER_SIZE];

	char buffer_save[BUFFER_SIZE];
	int fd;
	int i;

	if(!Do) return -1;

	char buf[200];
	int cnt_byte=0;

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[1;3H**Accelerometer Check**\n");
	write(fd_fb, buf, cnt_byte);

	memset(buffer_in, 0, BUFFER_SIZE);
	memset(buffer_out, 0, BUFFER_SIZE);
	memset(buffer_save, 0, BUFFER_SIZE);

	printf("Generate output buffer...\n");
	srand(time(NULL));
	for(i=0; i<BUFFER_SIZE; i++)
	{
		buffer_out[i] = rand() % 255;
	}

	for(i=0; i<BUFFER_SIZE; i++)
		{
		if(i==10) printf("\n");
		if(i==20) printf("\n");
		if(i==30) printf("\n");
		if(i==40) printf("\n");
		if(i==50) printf("\n");
		if(i==60) printf("\n");
		if(i==70) printf("\n");
		if(i==80) printf("\n");
		if(i==90) printf("\n");
		printf("0x%02X ",buffer_out[i]);
		}
	printf("\nok");
	printf("\n\n");



	printf("Open eMMC...");
	fd = open("/dev/mmcblk1", O_RDWR);
	if(fd < 0 )
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");

	i=read(fd, buffer_save, 100);
		if(i != 100)
		{
			printf("Read only %d bytes instead %d", i, 100);
		}

	printf("Close eMMC...");
	i = close(fd);
		if(i != 0 )
		{
			printf("Error: %d\n",i);
			return -1;
		}
	printf("ok\n\n");

	printf("Open eMMC...");
	fd = open("/dev/mmcblk1", O_RDWR);
	if(fd < 0 )
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");

	printf("Write output buffer to eMMC...");

	i = write(fd, buffer_out, BUFFER_SIZE);
	if(i != BUFFER_SIZE)
	{
		printf("Write only %d bytes instead %d", i, BUFFER_SIZE);
	}
	printf("ok\n");


	printf("Close eMMC...");
	i = close(fd);
	if(i != 0 )
	{
		printf("Error: %d\n",i);
		return -1;
	}
	printf("ok\n\n");

//----------------------------------------------

	fd = 0;

	printf("Open eMMC...");
	fd = open("/dev/mmcblk1", O_RDWR);
	if(fd < 0)
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");


	printf("Read input buffer from eMMC...");
	i = read(fd, buffer_in, BUFFER_SIZE);
	if(i != BUFFER_SIZE)
	{
		printf("Read only %d bytes instead %d", i, BUFFER_SIZE);
	}
	printf("ok\n\n");


	printf("Input buffer:\n");
	for(i=0; i<BUFFER_SIZE; i++)
		{
		if(i==10) printf("\n");
		if(i==20) printf("\n");
		if(i==30) printf("\n");
		if(i==40) printf("\n");
		if(i==50) printf("\n");
		if(i==60) printf("\n");
		if(i==70) printf("\n");
		if(i==80) printf("\n");
		if(i==90) printf("\n");
		printf("0x%02X ",buffer_in[i]);
		}
	printf("\n\n");

	printf("Close eMMC...");

	printf("Close eMMC...");
	i = close(fd);
	if(i != 0 )
	{
		printf("Error: %d\n",i);
		return -1;
	}
	printf("ok\n\n");

	printf("Open eMMC...");
	fd = open("/dev/mmcblk1", O_RDWR);
	if(fd < 0)
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");

	i = write(fd, buffer_save, BUFFER_SIZE);
		if(i != BUFFER_SIZE)
		{
			printf("Write only %d bytes instead %d", i, 100);
		}

	close(fd);
	printf("ok\n\n");

//------------------------------------------------

	for(i=0; i<BUFFER_SIZE; i++ )
	{
		if(buffer_in[i] != buffer_out[i])
		{
			printf("WARNING: output and input buffer are not equal\n");
			return -1;
		}
	}

	printf("Output and input buffer are equal\n\n");

	return 0;

}

int FuncSPI_32MBit_NOR_Flash(int Do)
{
	//Performs checksum validation on all memory parts to ensure memory is free of corruption or defect. This includes testing
	char buffer_out[BUFFER_SIZE];
	char buffer_in[BUFFER_SIZE];
	char buffer_save[BUFFER_SIZE];
	int fd;
	int i;

	if(!Do) return -1;

	memset(buffer_in, 0, BUFFER_SIZE);
	memset(buffer_out, 0, BUFFER_SIZE);
	memset(buffer_save, 0, BUFFER_SIZE);

	printf("Open SPI NOR...");
	fd = open("/dev/mtdblock0", O_RDWR);
	if(fd < 0 )
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");

	i=read(fd, buffer_save, 100);
		if(i != 100)
		{
			printf("Read only %d bytes instead %d", i, 100);
		}

	printf("Close SPI NOR...");
	i = close(fd);
		if(i != 0 )
		{
			printf("Error: %d\n",i);
			return -1;
		}
	printf("ok\n\n");

	printf("Generate output buffer...\n");
	srand(time(NULL));
	for(i=0; i<BUFFER_SIZE; i++)
	{
		buffer_out[i] = rand() % 255;
	}

	for(i=0; i<BUFFER_SIZE; i++)
	{
		if(i==10) printf("\n");
		if(i==20) printf("\n");
		if(i==30) printf("\n");
		if(i==40) printf("\n");
		if(i==50) printf("\n");
		if(i==60) printf("\n");
		if(i==70) printf("\n");
		if(i==80) printf("\n");
		if(i==90) printf("\n");
		printf("0x%02X ",buffer_out[i]);
	}
	printf("\nok");
	printf("\n\n");



	printf("Open SPI NOR...");
	fd = open("/dev/mtdblock0", O_WRONLY);
	if(fd < 0 )
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");



	printf("Write output buffer to SPI NOR...");
	i = write(fd, buffer_out, BUFFER_SIZE);
	if(i != BUFFER_SIZE)
	{
		printf("Write only %d bytes instead %d", i, BUFFER_SIZE);
	}
	printf("ok\n");


	printf("Close SPI NOR...");
	i = close(fd);
	if(i != 0 )
	{
		printf("Error: %d\n",i);
		return -1;
	}
	printf("ok\n\n");

//----------------------------------------------

	fd = 0;

	printf("Open SPI NOR...");
	fd = open("/dev/mtdblock0", O_RDONLY);
	if(fd < 0)
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");


	printf("Read input buffer from SPI NOR...");
	i = read(fd, buffer_in, BUFFER_SIZE);
	if(i != BUFFER_SIZE)
	{
		printf("Read only %d bytes instead %d", i, BUFFER_SIZE);
	}
	printf("ok\n\n");


	printf("Input buffer:\n");
	for(i=0; i<BUFFER_SIZE; i++)
		{
		if(i==10) printf("\n");
		if(i==20) printf("\n");
		if(i==30) printf("\n");
		if(i==40) printf("\n");
		if(i==50) printf("\n");
		if(i==60) printf("\n");
		if(i==70) printf("\n");
		if(i==80) printf("\n");
		if(i==90) printf("\n");
		printf("0x%02X ",buffer_in[i]);
		}
	printf("\n\n");

	printf("Close SPI NOR...");
	close(fd);
	printf("ok\n\n");

	printf("Open SPI NOR...");
	fd = open("/dev/mtdblock0", O_RDWR);
	if(fd < 0)
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");

	i = write(fd, buffer_save, BUFFER_SIZE);
		if(i != BUFFER_SIZE)
		{
			printf("Write only %d bytes instead %d", i, 100);
		}

	close(fd);
	printf("ok\n\n");

//------------------------------------------------

	for(i=0; i<BUFFER_SIZE; i++ )
	{
		if(buffer_in[i] != buffer_out[i])
		{
			printf("WARNING: output and input buffer are not equal\n");
			return -1;
		}
	}

	printf("Output and input buffer are equal\n\n");
	return 0;
};

int FuncEEPROM(int Do)
{
	//Performs checksum validation on all memory parts to ensure memory is free of corruption or defect. This includes testing
	#define BUFFER_SIZE 100

	char buffer_out[BUFFER_SIZE];
	char buffer_in[BUFFER_SIZE];
	char buffer_save[BUFFER_SIZE];
	char buf[BUF_SIZE_DISP];
	int fd;
	int i;
	int cnt_byte=0;

	if(!Do) return -1;

	memset(buffer_in, 0, BUFFER_SIZE);
	memset(buffer_out, 0, BUFFER_SIZE);
	memset(buffer_save, 0, BUFFER_SIZE);

	printf("**EEPROM Integrity Check**\n");

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[6;3H**EEPROM Integrity Check**\n");
	write(fd_fb, buf, cnt_byte);

	fd = open("/sys/class/i2c-dev/i2c-1/device/1-0054/eeprom", O_RDWR);
	if(fd < 0 )
	{
		printf("^Test 1A: Fail\n");
		printf("@EEPROM opening error#\n");

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test (1A): Fail\n\x1b[2C@EEPROM opening error#\n");
		write(fd_fb, buf, cnt_byte);
		return -1;
	}

	i=read(fd, buffer_save, 100);
		if(i != 100)
		{
			printf("^Test 1A: Fail\n");
			printf("@File opening error#\n");
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test (1A): Fail\n\x1b[2C@EEPROM opening error#\n");
			write(fd_fb, buf, cnt_byte);
		}

	//printf("Close EEPROM...\n");
	i = close(fd);
		if(i != 0 )
		{
			//printf("Error: %d\n",i);
			return -1;
		}
		//printf("ok\n\n");


	//printf("\nGenerate output buffer...\n");
	srand(time(NULL));
	for(i=0; i<BUFFER_SIZE; i++)
	{
		buffer_out[i] = rand() % 255;
	}

	/*for(i=0; i<BUFFER_SIZE; i++)
		{
		if(i==10) printf("\n");
		if(i==20) printf("\n");
		if(i==30) printf("\n");
		if(i==40) printf("\n");
		if(i==50) printf("\n");
		if(i==60) printf("\n");
		if(i==70) printf("\n");
		if(i==80) printf("\n");
		if(i==90) printf("\n");
		printf("0x%02X ",buffer_out[i]);
		}
	printf("\nok");
	printf("\n\n");*/



	//printf("Open EEPROM...");
	fd = open("/sys/class/i2c-dev/i2c-1/device/1-0054/eeprom", O_WRONLY);
	if(fd < 0 )
	{
		//printf("Error: %d\n", fd);
		printf("^Test 1A: Fail\n");
		printf("@EEPROM opening error#\n");
		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test (1A): Fail\n\x1b[2C@EEPROM opening error#\n");
		write(fd_fb, buf, cnt_byte);
		return -1;
	}
	//printf("ok\n");


	//printf("Write output buffer to EEPROM...");
	i = write(fd, buffer_out, BUFFER_SIZE);
	if(i != BUFFER_SIZE)
	{
		//printf("Write only %d bytes instead %d", i, BUFFER_SIZE);
		printf("^Test 1A: Fail\n");
		printf("@EEPROM write error#\n");
		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test (1A): Fail\n\x1b[2C@EEPROM write error#\n");
		write(fd_fb, buf, cnt_byte);
	}
	//printf("ok\n");


	printf("Close EEPROM...");
	i = close(fd);
	if(i != 0 )
	{
		//printf("Error: %d\n",i);
		printf("^Test 1A: Fail\n");
		printf("@EEPROM close error#\n");
		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test (1A): Fail\n\x1b[2C@EEPROM close error#\n");
		write(fd_fb, buf, cnt_byte);
		return -1;
	}
	//printf("ok\n\n");

//----------------------------------------------

	fd = 0;

	//printf("Open EEPROM...");
	fd = open("/sys/class/i2c-dev/i2c-1/device/1-0054/eeprom", O_RDONLY);
	if(fd < 0)
	{
		//printf("Error: %d\n", fd);
		printf("^Test 1A: Fail\n");
		printf("@EEPROM opening error#\n");
		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test (1A): Fail\n\x1b[2C@EEPROM opening error#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	//printf("ok\n");


	//printf("Read input buffer from EEPROM...");
	i = read(fd, buffer_in, BUFFER_SIZE);
	if(i != BUFFER_SIZE)
	{
		//printf("Read only %d bytes instead %d", i, BUFFER_SIZE);
		printf("^Test 1A: Fail\n");
		printf("@EEPROM write error#\n");
		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test (1A): Fail\n\x1b[2C@EEPROM write error#\n");
		write(fd_fb, buf, cnt_byte);
	}
	//printf("ok\n\n");


	/*printf("Input buffer:\n");
	for(i=0; i<BUFFER_SIZE; i++)
		{
		if(i==10) printf("\n");
		if(i==20) printf("\n");
		if(i==30) printf("\n");
		if(i==40) printf("\n");
		if(i==50) printf("\n");
		if(i==60) printf("\n");
		if(i==70) printf("\n");
		if(i==80) printf("\n");
		if(i==90) printf("\n");
		printf("0x%02X ",buffer_in[i]);
		}
	printf("\n\n");

	printf("Close EEPROM...");*/
	close(fd);
	//printf("ok\n\n");

//------------------------------------------------

	for(i=0; i<BUFFER_SIZE; i++ )
	{
		if(buffer_in[i] != buffer_out[i])
		{
			//printf("WARNING: output and input buffer are not equal\n");

			printf("^Test 1A: Fail\n");
			printf("@EEPROM write error#\n");
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test (1A): Fail\n\x1b[2C@WARNING: output and input buffer are not equal#\n");
			write(fd_fb, buf, cnt_byte);
			return -1;
		}
	}

	//printf("Output and input buffer are equal\n\n");

	//printf("Open EEPROM...\n");
	fd = open("/sys/class/i2c-dev/i2c-1/device/1-0054/eeprom", O_RDWR);
		if(fd < 0)
		{
			printf("^Test 1A: Fail\n");
			printf("@EEPROM opening error#\n");
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test (1A): Fail\n\x1b[2C@EEPROM opening error#\n");
			write(fd_fb, buf, cnt_byte);
			return -1;
		}
		//printf("ok\n");

		i = write(fd, buffer_save, BUFFER_SIZE);
			if(i != BUFFER_SIZE)
			{
				//printf("Write only %d bytes instead %d", i, 100);
				printf("^Test 1A: Fail\n");
				printf("@EEPROM write error#\n");
				memset(buf, 0, 200);
				cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test (1A): Fail\n\x1b[2C@EEPROM write error#\n");
				write(fd_fb, buf, cnt_byte);
			}

		//printf("Close EEPROM...\n");
		close(fd);
		//printf("ok\n\n");

	if (Write_EEPROM("2", 0)!=-1)
	{
		printf("&Test 1A: OK\n");
		printf("@EEPROM OK#\n");
		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test (1A): Ok\n\x1b[2C@EEPROM OK#\n");
		write(fd_fb, buf, cnt_byte);

		return 0;
	}
return -1;
};

int FuncSN_Burn_In(int Do){
	//User will be able to enter a alphanumeric serial number. This value should be permanently stored in a protected sector in eMMC

	int fd;
	char SerialNumberRead[16];
	unsigned char SN[8];

	char buf[BUF_SIZE_DISP];
	int cnt_byte=0;

	memset (SN, 0, 8);
	memset(SerialNumber, 0, 16);
	memset(SerialNumberRead, 0, 16);
	//printf("\n\nPlease, enter S/N and press 'Enter' button: \n");
		if( poll(&newpoll, 1, 30000) )
		{

			__fpurge(stdin);
				if (scanf("%02x%02x%02x%02x%02x%02x%02x%02x", &SN[0],&SN[1],&SN[2],&SN[3],&SN[4],&SN[5],&SN[6],&SN[7])==8)
				{

					printf("%02x%02x%02x%02x%02x%02x%02x%02x\n", SN[0],SN[1],SN[2],SN[3],SN[4],SN[5],SN[6],SN[7]);
					sprintf(SerialNumber, "%02x%02x%02x%02x%02x%02x%02x%02x", SN[0],SN[1],SN[2],SN[3],SN[4],SN[5],SN[6],SN[7]);

					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "%s\n", SerialNumber);
					write(fd_fb, buf, cnt_byte);

					//return 0;
				}
				else
				{
					__fpurge(stdin);
					return -1;
				}
		}
		else
		{
			//puts("Read nothing\n");
			__fpurge(stdin);
			return -1;
		}

	fd = open("/sys/class/i2c-dev/i2c-1/device/1-0054/eeprom", O_WRONLY);
		if(fd < 0 )
		{
			//printf("Error: %d\n", fd);
			printf("^Test 1B: Fail\n");
			printf("@EEPROM opening error#\n");
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test (1B): Fail\n\x1b[2C@EEPROM opening error#\n");
			write(fd_fb, buf, cnt_byte);
			return -1;
		}
	lseek(fd, 2, SEEK_SET);

	//printf("Write S/N to EEPROM...\n");
	write(fd, SerialNumber, 16);

	//printf("Write %d bytes\n", cnt);

	//printf("Close EEPROM...\n");
	close(fd);

	fd = open("/sys/class/i2c-dev/i2c-1/device/1-0054/eeprom", O_RDONLY);
		if(fd < 0 )
		{
			printf("^Test 1B: Fail\n");
			printf("@EEPROM opening error#\n");
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test (1B): Fail\n\x1b[2C@EEPROM opening error#\n");
			write(fd_fb, buf, cnt_byte);
			return -1;
		}
	lseek(fd, 2, SEEK_SET);
	read(fd, SerialNumberRead, 16);
	close(fd);
		if(strncmp(SerialNumber, SerialNumberRead, 16) == 0)
		{
			//printf("\nRead and input S/N are equal\n\n");
			//sprintf(SerialNumber, "%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x", SN[0],SN[1],SN[2],SN[3],SN[4],SN[5],SN[6],SN[7]);

			printf("^Test 1B: OK\n");
			printf("@SN is successful stored#\n");
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 1B: OK\n\x1b[2C@SN is successful stored#\n");
			write(fd_fb, buf, cnt_byte);

			return 0;
		}
		else return -1;
}


int FuncSN_Read_In(int Do)
{
	char SerialNumberRead[16];
	unsigned char SN[8];
	unsigned int sn_read_1=0;
	unsigned int sn_read_2=0;
	int fd;
	char buf[BUF_SIZE_DISP];
	int cnt_byte=0;



	printf("\n**Device Serial Number Burn-In**\n");
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\n\x1b[1С **Device Serial Number Burn-In**\n");
	write(fd_fb, buf, cnt_byte);

	fd = open("/sys/class/i2c-dev/i2c-1/device/1-0054/eeprom", O_RDONLY);
		if(fd < 0 )
		{
			//printf("Error: %d\n", fd);
			printf("^Test (1B): Fail\n");
			printf("@EEPROM opening error#\n");

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test (1B): Fail\n\x1b[2C@EEPROM opening error#\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}
	lseek(fd, 2, SEEK_SET);

	read(fd, SerialNumberRead, 8);
	sn_read_1=strtoul(SerialNumberRead, NULL, 16);
	read(fd, SerialNumberRead, 8);
	sn_read_2=strtoul(SerialNumberRead, NULL, 16);
	close(fd);

		if ((sn_read_1==0)&&(sn_read_2==0))
		{
			//printf("\n\nPlease, enter S/N and press 'Enter' button: \n");
			//sprintf(SerialNumber_1, "\x1b[33mSN is None! Please enter 64-bit SN in HEX format!\x1b[0m");

			printf("@Device Serial Number: Not Set#\n");
			printf("Enter Device Serial Number (16 character Hex value):");

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Device Serial Number: Not Set#\n\x1b[2CEnter Device Serial Number (16 character Hex value):");
			write(fd_fb, buf, cnt_byte);

			int state=0;
				for (int i=0; i<=2; i++)
				{
					state=FuncSN_Burn_In(1);
					if (state==0)
					{
						printf("&Test 1B: OK\n");

						memset(buf, 0, 200);
						cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 1B: OK\n");
						write(fd_fb, buf, cnt_byte);
						return 0;

					}
				}
			printf("&Test 1A: Fail, EEPROM Corrupted or Inaccessible\n");

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 1A: Fail, EEPROM Corrupted or Inaccessible ");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}
		else
		{
			SN[0]=(unsigned char)(sn_read_2&0xFF);
			SN[1]=(unsigned char)((sn_read_2>>8)&0xFF);
			SN[2]=(unsigned char)((sn_read_2>>16)&0xFF);
			SN[3]=(unsigned char)((sn_read_2>>24)&0xFF);

			SN[4]=(unsigned char)(sn_read_1&0xFF);
			SN[5]=(unsigned char)((sn_read_1>>8)&0xFF);
			SN[6]=(unsigned char)((sn_read_1>>16)&0xFF);
			SN[7]=(unsigned char)((sn_read_1>>24)&0xFF);

			char buf[200];
			int cnt_byte=0;

			//memset(buf, 0, 200);
			//cnt_byte=snprintf(buf, sizeof(buf), "\x1b[7;3HSerial number TEST:\x1b[32m OK\x1b[0m - SN is %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x. \x1b[33mPress 'y' to enter SN or 'n' to exit\x1b[0m\n", SN[7], SN[6], SN[5], SN[4], SN[3], SN[2], SN[1], SN[0]);
			//write(fd_fb, buf, cnt_byte);

			printf("@Device Serial Number: %02x%02x%02x%02x%02x%02x%02x%02x#\n", SN[7], SN[6], SN[5], SN[4], SN[3], SN[2], SN[1], SN[0]);
			printf("Re-enter Serial Number? (Y/N):\n");

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Device Serial Number: %02x%02x%02x%02x%02x%02x%02x%02x#\n\x1b[2CRe-enter Serial Number? (Y/N):", SN[7], SN[6], SN[5], SN[4], SN[3], SN[2], SN[1], SN[0]);
			write(fd_fb, buf, cnt_byte);

			//sprintf(SerialNumber_1, "Serial number is %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x", SN[7], SN[6], SN[5], SN[4], SN[3], SN[2], SN[1], SN[0]);
			//printf("Press 'y' to enter SN or 'n' to exit:\n");
			//__fpurge(stdin);
			char ch;
				if( poll(&newpoll, 1, 30000) )
				{
					ch=getchar();
				}

				if ((ch !='y' && ch !='n'))
				{
					//sprintf(SerialNumber_1, "Serial number is %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x", SN[7], SN[6], SN[5], SN[4], SN[3], SN[2], SN[1], SN[0]);
					printf("@Format is Incorrect or no value entered#\n");

					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Format is Incorrect or no value entered#\n");
					write(fd_fb, buf, cnt_byte);
					return -1;
				}

				if (ch =='y')
				{
					sprintf(SerialNumber_1, "\x1b[33mPlease enter 64-bit SN in HEX format!\x1b[0m");
					return 0;
				}
				if (ch=='n')
				{
					sprintf(SerialNumber_1, "Serial number is %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x", SN[7], SN[6], SN[5], SN[4], SN[3], SN[2], SN[1], SN[0]);
					return -1;
				}

		}
return -1;
}

int FuncAccelerometer_Calibration(int Do)
{
	//Accelerometer X/Y/Z offet trim needs to be calculate (while board is in a stable flat position). Offset values need to stored in a protected EMMC sector and available for the accelerometer to load on start-up
	#define FiltrTime 32
	#define BUFF_SIZE 100
	#define SN_SIZE 100

	char buf[200];
	int cnt_byte=0;

	const char *AccelS_Read[3] = {"/sys/bus/iio/devices/iio:device0/in_accel_scale","/sys/bus/iio/devices/iio:device1/in_accel_scale","/sys/bus/iio/devices/iio:device2/in_accel_scale"};
	const char *AccelX_Read[3] = {"/sys/bus/iio/devices/iio:device0/in_accel_x_raw","/sys/bus/iio/devices/iio:device1/in_accel_x_raw","/sys/bus/iio/devices/iio:device2/in_accel_x_raw"};
	const char *AccelY_Read[3] = {"/sys/bus/iio/devices/iio:device0/in_accel_y_raw","/sys/bus/iio/devices/iio:device1/in_accel_y_raw","/sys/bus/iio/devices/iio:device2/in_accel_y_raw"};
	const char *AccelZ_Read[3] = {"/sys/bus/iio/devices/iio:device0/in_accel_z_raw","/sys/bus/iio/devices/iio:device1/in_accel_z_raw","/sys/bus/iio/devices/iio:device2/in_accel_z_raw"};

	const char *CalibX_Read[3] = {"/sys/bus/iio/devices/iio:device0/in_accel_x_calibbias","/sys/bus/iio/devices/iio:device1/in_accel_x_calibbias","/sys/bus/iio/devices/iio:device2/in_accel_x_calibbias"};
	const char *CalibY_Read[3] = {"/sys/bus/iio/devices/iio:device0/in_accel_y_calibbias","/sys/bus/iio/devices/iio:device1/in_accel_y_calibbias","/sys/bus/iio/devices/iio:device2/in_accel_y_calibbias"};
	const char *CalibZ_Read[3] = {"/sys/bus/iio/devices/iio:device0/in_accel_z_calibbias","/sys/bus/iio/devices/iio:device1/in_accel_z_calibbias","/sys/bus/iio/devices/iio:device2/in_accel_z_calibbias"};

	char dataBuffer[BUFF_SIZE];

	int fdS, fdX,fdY,fdZ, fdC, indexFile = 0;
	int ValueX=0,ValueY=0,ValueZ=0;//ValueS=0,
	int CalibX=0,CalibY=0,CalibZ=0; //i=0;

	int FounFileFlag = 0;


	memset(AccelBuffer, 0, sizeof(AccelBuffer));
	if(!Do) return -1;

	printf("Start calibrate\n\n");

	USB_printf("**Accelerometer Check**\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[1;3H**Accelerometer Check**\n");
	write(fd_fb, buf, cnt_byte);

	for(int i=0; i<=2; i++){
		//printf("Open: %s\n", AccelS_Read[i]);
		fdS = open( AccelS_Read[i], O_RDONLY );
		if(fdS == -1){
			//printf("%s - not found\n", AccelS_Read[i]);
		}
		else{
			FounFileFlag=1;
			indexFile= i;
			break;
		}
	}

	if(FounFileFlag==0)
	{
		printf( "ERROR: Unable to open device file! \n" );

		USB_printf("^Test 3: Fail\n@Unable to open device file!#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail\n\x1b[2C@Unable to open device file!#\n");
		write(fd_fb, buf, cnt_byte);
		return -1;
	}
	printf("Open: %s  - OK\n" ,AccelS_Read[indexFile]);
	FounFileFlag = 0;

	printf("Open: %s\n", AccelX_Read[indexFile]);
	fdX = open( AccelX_Read[indexFile], O_RDONLY );
	if(fdX == -1)
	{
		sprintf(AccelBuffer, "Unable to open device file!");
		printf( "ERROR: Unable to open device file! \n" );
		USB_printf("^Test 3: Fail\n@Unable to open device file!#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail\n\x1b[2C@Unable to open device file!#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	//printf("Open: %s  - OK\n" ,AccelX_Read[indexFile]);

	//printf("Open: %s\n", AccelY_Read[indexFile]);
	fdY = open( AccelY_Read[indexFile], O_RDONLY );
	if(fdY == -1)
	{
		sprintf(AccelBuffer, "Unable to open device file!");
		printf( "ERROR: Unable to open device file! \n" );

		USB_printf("^Test 3: Fail\n@Unable to open device file!#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail\n\x1b[2C@Unable to open device file!#\n");
		write(fd_fb, buf, cnt_byte);
		return -1;
	}
	//printf("Open: %s  - OK\n" ,AccelY_Read[indexFile]);

	//printf("Open: %s\n", AccelZ_Read[indexFile]);

	fdZ = open( AccelZ_Read[indexFile], O_RDONLY );
	if(fdZ == -1)
	{
		//sprintf(AccelBuffer, "Unable to open device file!");
		//printf( "ERROR: Unable to open device file! \n" );

		USB_printf("^Test 3: Fail\n@Unable to open device file!#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail\n\x1b[2C@Unable to open device file!#\n");
		write(fd_fb, buf, cnt_byte);
		return -1;
	}
	//printf("Open: %s  - OK\n" ,AccelZ_Read[indexFile]);

		memset(dataBuffer, 0, sizeof( dataBuffer ));
		sprintf(dataBuffer, "%i",0);
		fdC = open(CalibX_Read[indexFile], O_WRONLY);
		if(fdC == -1)
		{

			USB_printf("^Test 3: Fail\n@Unable to open device file!#\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail\n\x1b[2C@Unable to open device file!#\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}
		write(fdC, dataBuffer, sizeof(dataBuffer));
		close(fdC);

		memset(dataBuffer, 0, sizeof( dataBuffer ));
		sprintf(dataBuffer, "%i",0);
		fdC = open(CalibY_Read[indexFile], O_WRONLY);
		if(fdC == -1)
		{
			USB_printf("^Test 3: Fail\n@Unable to open device file!#\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail\n\x1b[2C@Unable to open device file!#\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}
		write(fdC, dataBuffer, sizeof(dataBuffer));
		close(fdC);

		memset(dataBuffer, 0, sizeof( dataBuffer ));
		sprintf(dataBuffer, "%i",0);
		fdC = open(CalibZ_Read[indexFile], O_WRONLY);
		if(fdC == -1)
		{
			USB_printf("^Test 3: Fail\n@Unable to open device file!#\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail\n\x1b[2C@Unable to open device file!#\n");
			write(fd_fb, buf, cnt_byte);
			return -1;
		}
		write(fdC, dataBuffer, sizeof(dataBuffer));
		close(fdC);

		memset(dataBuffer, 0, sizeof( dataBuffer ));
		fdS = open(AccelS_Read[indexFile], O_RDONLY);
		read(fdS, dataBuffer, sizeof(dataBuffer));
		close(fdS);

		ValueX = 0;
		ValueY = 0;
		ValueZ = 0;

		for(int i=0; i<FiltrTime; i++){
			//X
			memset(dataBuffer, 0, sizeof( dataBuffer ));
			fdX = open(AccelX_Read[indexFile], O_RDONLY);
			read(fdX, dataBuffer, sizeof(dataBuffer));
			close(fdX);
			ValueX +=  atoi(dataBuffer);
			//Y
			memset(dataBuffer, 0, sizeof( dataBuffer ));
			fdY = open(AccelY_Read[indexFile], O_RDONLY);
			read(fdY, dataBuffer, sizeof(dataBuffer));
			close(fdY);
			ValueY +=  atoi(dataBuffer);
			//Z
			memset(dataBuffer, 0, sizeof( dataBuffer ));
			fdZ = open(AccelZ_Read[indexFile], O_RDONLY);
			read(fdZ, dataBuffer, sizeof(dataBuffer));
			close(fdZ);
			ValueZ +=  atoi(dataBuffer);
			usleep(10);
		};

		ValueX = ValueX/FiltrTime;
		ValueY = ValueY/FiltrTime;
		ValueZ = ValueZ/FiltrTime;
		printf("Accel_X = %i \n", ValueX );
		printf("Accel_Y = %i \n", ValueY );
		printf("Accel_Z = %i \n", ValueZ );

		char buf_axis[10];
		int res_calib=0;
		memset(buf_axis, 0, 10);
		memset(AccelBuffer, 0, sizeof(AccelBuffer));

			if ((ValueX<40) && (ValueX>-40))
				CalibX=-ValueX/2; //-1
			else
			{
				//memset(AccelBuffer, 0, sizeof(AccelBuffer));
				//printf("Wrong position - OX axis!\n");
				//sprintf(AccelBuffer, "Wrong position - OX");
				strcpy(buf_axis, " OX");
				res_calib=-1;
			}
			if ((ValueY<40) && (ValueY>-40))
				CalibY=-ValueY/2;
			else
			{
				//memset(AccelBuffer, 0, sizeof(AccelBuffer));
				//printf("Wrong position - OY axis!\n");
				//sprintf(AccelBuffer, "Wrong position - OX, OY axis!");
				strcat(buf_axis, " OY");
				res_calib=-1;
			}
			if ((ValueZ>1000) && (ValueZ<1048))
				CalibZ=(1024-ValueZ)/2;
			else
			{
				//memset(AccelBuffer, 0, sizeof(AccelBuffer));
				//printf("Wrong position - OZ axis!\n");
				//sprintf(AccelBuffer, "Wrong position - OZ axis!");
				strcat(buf_axis, " OZ");
				res_calib=-1;
			}
			if (res_calib==-1)
			{
				sprintf(AccelBuffer, "Wrong position - %s! x=%i, y=%i, z=%i", buf_axis, ValueX, ValueY, ValueZ);
			}
		CX=ValueX; CY=ValueY; CZ=ValueZ;

	close(fdX);
	close(fdY);
	close(fdZ);

	memset(dataBuffer, 0, sizeof( dataBuffer ));
	sprintf(dataBuffer, "%i",CalibX);
	fdC = open(CalibX_Read[indexFile], O_WRONLY);
	if(fdC == -1)
	{
		USB_printf("^Test 3: Fail\n@Unable to open device file!#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail\n\x1b[2C@Unable to open device file!#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	write(fdC, dataBuffer, sizeof(dataBuffer));
	close(fdC);

	memset(dataBuffer, 0, sizeof( dataBuffer ));
	sprintf(dataBuffer, "%i",CalibY);
	fdC = open(CalibY_Read[indexFile], O_WRONLY);
	if(fdC == -1)
	{

		USB_printf("^Test 3: Fail\n@Unable to open device file!#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail\n\x1b[2C@Unable to open device file!#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	write(fdC, dataBuffer, sizeof(dataBuffer));
	close(fdC);

	memset(dataBuffer, 0, sizeof( dataBuffer ));
	sprintf(dataBuffer, "%i",CalibZ);
	fdC = open(CalibZ_Read[indexFile], O_WRONLY);
	if(fdC == -1)
	{

		USB_printf("^Test 3: Fail\n@Unable to open device file!#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail\n\x1b[2C@Unable to open device file!#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	write(fdC, dataBuffer, sizeof(dataBuffer));
	close(fdC);

	//Store Calibrate in file
	if (res_calib==0)
	{
	sprintf(dataBuffer, "%i %i %i ",CalibX,CalibY,CalibZ);
		if (Write_EEPROM(dataBuffer, 19)==0)
		{

			USB_printf("^Test 3: Fail\n@Error while writing calibration values!#\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail\n\x1b[2C@Error while writing calibration values!#\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}
		else
		{

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 3: OK\n\x1b[2C@Calibration OK, dx=%i, dy=%i, dz=%i#\n", CalibX,CalibY,CalibZ);
			USB_printf(buf, 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 3: OK\n\x1b[2C@Calibration OK, dx=%i, dy=%i, dz=%i#\n", CalibX,CalibY,CalibZ);
			write(fd_fb, buf, cnt_byte);
		}
	}
	else
	{

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "^Test 3: Fail\n@%s#\n", AccelBuffer);
		USB_printf(buf, 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail\n\x1b[2C@%s#\n", AccelBuffer);
		write(fd_fb, buf, cnt_byte);
	}

	return res_calib;
};

int FuncConfirm_Battery_Charger_Communication(int Do)
{
	//Confirm detection and communication with battery charger
	FILE * hiddenConsole;
	#define ANSWER_L 1024
	char Answer[ANSWER_L] ="";
	int lastchar;

	char buf[200];
	int cnt_byte=0;

	if(!Do) return -1;

	const char* ChipInfo = "0x0001";

	hiddenConsole = popen("lsmod | grep ltc4015_charger", "r"); //lsmod | grep pfuze100_regulator
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("hiddenConsole answer: \n%s", Answer);
	if(lastchar){
		hiddenConsole = popen("rmmod ltc4015_charger", "r"); //rmmod pfuze100_regulator
		lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
		Answer[lastchar] = '\0';
		pclose(hiddenConsole);
		printf("hiddenConsole answer: \n%s", Answer);
	}
	hiddenConsole = popen("i2cget -y 2 0x68 0x4A w", "r");
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("hiddenConsole answer: \n%s", Answer);

	if(strncmp(Answer, ChipInfo,6) == 0){

		USB_printf("&Test 4B: OK\n@Charger is available#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 4B: OK\n\x1b[2C@Charger is available#\n");
		write(fd_fb, buf, cnt_byte);
		return 0;
	}
	else{

		USB_printf("^Test 4B: Fail\n@Charger don't answer#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 4B: Fail\n\x1b[2C@Charger don't answer#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	};
	return 0;
};

int FuncConfirm_PMIC_Communication(int Do)
{
	//Validate that PMIC is present and responsive via I2C
	FILE * hiddenConsole;
	char Answer[ANSWER_L] ="";
	int lastchar;

	const char* ChipInfo = "0x10";

	if(!Do) return -1;

	char buf[200];
	int cnt_byte=0;

	USB_printf("**Power Management Test**\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[1;3H**Power Management Test**\n");
	write(fd_fb, buf, cnt_byte);

	hiddenConsole = popen("lsmod | grep pfuze100_regulator", "r"); //lsmod | grep pfuze100_regulator
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("hiddenConsole answer: \n%s", Answer);
	if(lastchar){
		hiddenConsole = popen("rmmod pfuze100_regulator", "r"); //rmmod pfuze100_regulator
		lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
		Answer[lastchar] = '\0';
		pclose(hiddenConsole);
		printf("hiddenConsole answer: \n%s", Answer);
	}
	hiddenConsole = popen("i2cget -y 2 0x08 0x00 b", "r"); //i2cget -y 2 0x08 0x00 b
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("hiddenConsole answer: \n%s", Answer);

	if(strncmp(Answer, ChipInfo,4) == 0){

		USB_printf("&Test 4A: OK\n@PMIC is available#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 4A: OK\n\x1b[2C@PMIC is available#\n");
		write(fd_fb, buf, cnt_byte);

		return 0;
	}
	else{

		USB_printf("^Test 4A: Fail\n@PMIC don't answer#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 4A: Fail\n\x1b[2C@PMIC don't answer#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	};

};

int FuncEMMY_163_Connectivity_Check(int Do)
{
	//Confirm EMMY module is responsive
	//Validate Wi-Fi antenna by detecting and printing available Wifi Networks

	FILE * hiddenConsole;
	char Answer[ANSWER_L];
	int lastchar;
	int result = 0;

	char buf[200];
	int cnt_byte=0;

	if(!Do) return -1;

	USB_printf("**Wifi Test**\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[1;3H**Wifi Test**\n");
	write(fd_fb, buf, cnt_byte);

		if (wifi_first_start_flag!=1)
		{
			hiddenConsole = popen("/root/wifi_on.sh", "r");
			lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
			Answer[lastchar] = '\0';
			pclose(hiddenConsole);
			wifi_first_start_flag=1;
			sleep(5);
		}

	hiddenConsole = popen("lsmod | grep sd8xxx", "r");
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("hiddenConsole answer: \n%s", Answer);
	if(lastchar == 0){
		printf("\n Kernel module 'sd8xxx' not loaded!!! Load module and try again\n");

		USB_printf("^Test 7: Fail, module sd8xxx not loaded\n@Module sd8xxx not loaded#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 7: Fail, module sd8xxx not loaded\n\x1b[2C@Module sd8xxx not loaded#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	hiddenConsole = popen("lsmod | grep mlan", "r");
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("hiddenConsole answer: \n%s", Answer);
	if(lastchar == 0){
		printf("\n Kernel module 'mlan' not loaded!!! Load module and try again\n");
		sprintf (EmmyWiFiBuffer, "Module 'mlan' not loaded");

		USB_printf("^Test 7A: Fail, module mlan not loaded\n@Module mlan not loaded#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 7A: Fail, module mlan not loaded\n\x1b[2C@Module mlan not loaded#\n");
		write(fd_fb, buf, cnt_byte);

		result=-1;
	}
	//print networks
	printf("\n\nScanning WiFi networks... Wait 10 sec...\n\n");
	hiddenConsole = popen("iw dev mlan0 scan | grep SSID", "r");

	sleep(10);

	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("\nFound networks: \n%s", Answer);

	char* ptr_1;
	char* ptr_2;
	int cnt=0;
	ptr_1=strstr(Answer, "SSID");
	memset(EmmyWiFiBuffer, 0, 1024);
	if (ptr_1!=NULL)
	{
		ptr_2=strstr(ptr_1, "\n");
		cnt=ptr_2-ptr_1;
		strncpy(EmmyWiFiBuffer, ptr_1, cnt);
		//sprintf (EmmyWiFiBuffer, "Wi-Fi networks are available");
		printf("\nWiFi Ok\n");
		result=0;
	}
	else
	{
		sprintf (EmmyWiFiBuffer, "Wi-Fi networks aren't available");
		printf("\nWiFi Failed\n");
		result=0;
	}
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "^&Test 7A: OK\n@%s#\n", EmmyWiFiBuffer);
	USB_printf(buf, 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 7A: OK\n\x1b[2C@%s#\n", EmmyWiFiBuffer);
	write(fd_fb, buf, cnt_byte);

	//Validate Bluetooth Antenna by detecting and printing available Bluetooth devices to pair

	USB_printf("**Bluetooth Test**\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C**Bluetooth Test**\n");
	write(fd_fb, buf, cnt_byte);

	hiddenConsole = popen("lsmod | grep bt8xxx", "r");
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("hiddenConsole answer: \n%s", Answer);
	if(lastchar == 0){
		printf("\n Kernel module 'bt8xxx' not loaded!!! Load module and try again");
		sprintf (EmmyBTBuffer, "Module 'bt8xxx' not loaded");

		USB_printf("^Test 7B: Fail, module bt8xxx not loaded\n@Module bt8xxx not loaded#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 7B: Fail, module bt8xxx not loaded\n\x1b[2C@Module bt8xxx not loaded#\n");
		write(fd_fb, buf, cnt_byte);

		result=-1;
	}

	hiddenConsole = popen("/root/bt_scan.sh", "r");
	printf("\n\nScanning bluetooth devices networks... Wait 10 sec...\n\n");
	sleep(10);
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);

	char* ptr_3;
	ptr_3=strstr(Answer, ":");
	memset(EmmyBTBuffer, 0, 1024);
		if (ptr_3!=NULL)
		{
			//sprintf (EmmyBTBuffer, "BT devices are available");
			strcat(EmmyBTBuffer, "MAC: ");
			strncpy((EmmyBTBuffer+5), (ptr_3-2), 17);
			printf("\nBT Ok\n");
			result=0;
		}
		else
		{
			sprintf (EmmyBTBuffer, "BT devices aren't available");
			printf("\nBT Failed\n");
			result=0;
		}

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "^&Test 7B: OK\n@%s#\n", EmmyBTBuffer);
		USB_printf(buf, 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 7B: OK\n\x1b[2C@%s#\n", EmmyBTBuffer);
		write(fd_fb, buf, cnt_byte);

	printf("\nFound bluetooth devices: \n%s", Answer);


	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "./NFC_test %s, %i", USB_PATH, fd_fb);
	system(buf);

	if(result == 0){

		return 0;
	}
	else{

		return -1;
	}

}

int FuncAmbient_Light_Sensor_Functionality(int Do)
{
	//Provide live printed results of light sensor brightness detection (user can hit enter to exit)
	char F=0;
	const char *devName[3] = {"/sys/bus/iio/devices/iio:device0/in_illuminance_raw","/sys/bus/iio/devices/iio:device1/in_illuminance_raw","/sys/bus/iio/devices/iio:device2/in_illuminance_raw"};
	int file, i_t=0;

	if(!Do) return -1;

	char buf[200];
	int cnt_byte=0;

	USB_printf("**Light Sensor Test**\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[1;3H**Light Sensor Test**\n");
	write(fd_fb, buf, cnt_byte);

	for(int i=0; i<=2; i++){
		printf("Open: %s\n", devName[i]);
		file = open( devName[i], O_RDONLY );
		if(file == -1){
			printf("%s - not found\n", devName[i]);
		}
		else{
			F=1;
			i_t= i;
			break;
		}
	}

	if(F==0)
	{
		printf( "ERROR: Unable to open device file! \n" );

		USB_printf("^Test 5: Fail\n@Light sensor isn't available#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 5: Fail\n\x1b[2C@Light sensor isn't available#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	printf("Open OK\n");

		memset(LightDataBuffer, 0, sizeof( LightDataBuffer ) );
		file = open(devName[i_t], O_RDONLY);
		read( file, LightDataBuffer, sizeof(LightDataBuffer));
		close(file);
		printf("Light sensor value = %s \n", LightDataBuffer );

		int lux=atoi(LightDataBuffer);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "&Test 5: OK\n@Light sensor is available, lux=%i#\n", lux);
		USB_printf(buf, 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 5: OK\n\x1b[2C@Light sensor is available, lux=%i#\n", lux);
		write(fd_fb, buf, cnt_byte);



	close(file);
	printf("Close OK\n");

	return 0;
}

int FuncBarometer_Functionality(int Do)
{
	//Provide live printed results of pressure readings and temperature readings (user can hit enter to exit)
	char F=0;
	char dataBuffer[100];
	const char *pressureRead[3] = {"/sys/bus/iio/devices/iio:device0/in_pressure_raw","/sys/bus/iio/devices/iio:device1/in_pressure_raw","/sys/bus/iio/devices/iio:device2/in_pressure_raw"};
	const char *tempRead[3] =     {"/sys/bus/iio/devices/iio:device0/in_temp_raw","/sys/bus/iio/devices/iio:device1/in_temp_raw","/sys/bus/iio/devices/iio:device2/in_temp_raw"};
	int fileTemp,filePressure, i_t=0, i_p=0;

	if(!Do) return -1;

	char buf[200];
	int cnt_byte=0;

	USB_printf("**Pressure Sensor Test**\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[1;3H**Pressure Sensor Test**\n");
	write(fd_fb, buf, cnt_byte);

	for(int i=0; i<=2; i++){
		printf("Open: %s\n", tempRead[i]);
		fileTemp = open( tempRead[i], O_RDONLY );
		if(fileTemp == -1){
			printf("%s - not found\n", tempRead[i]);
		}
		else{
			F=1;
			i_t= i;
			break;//continue;
		}
	}

	if(F==0)
	{
		printf( "ERROR: Unable to open device file! \n" );

		USB_printf("^Test 6: Fail, pressure sensor isn't available\n@Pressure sensor isn't available#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 6: Fail, pressure sensor isn't available\n\x1b[2C@Pressure sensor isn't available#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	printf("Open OK\n");

	F=0;
	for(int i=0; i<=2; i++){
		printf("Open: %s\n", pressureRead[i]);
		filePressure = open( pressureRead[i], O_RDONLY );
		if(filePressure == -1){
			printf("%s - not found\n", pressureRead[i]);
		}
		else{
			F=1;
			i_p = i;
			break;
		}
	}

	if(F==0)
	{
		printf( "ERROR: Unable to open device file! \n" );

		USB_printf("^Test 6: Fail, pressure sensor isn't available\n@Pressure sensor isn't available#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 6: Fail, pressure sensor isn't available\n\x1b[2C@Pressure sensor isn't available#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	printf("Open OK\n");

	// Temperature data reading
		memset( dataBuffer, 0, sizeof( dataBuffer ) );
		fileTemp = open( tempRead[i_t], O_RDONLY );
		read( fileTemp, dataBuffer, sizeof(dataBuffer) );
		close( fileTemp );

		temperature = atof( dataBuffer );
		temperature *= 0.0625;

		printf( "Temperature Celsius value = %f \n", temperature );
		// 100us delay for sensor
		usleep(100000);

	// Pressure data reading
		memset( dataBuffer, 0, sizeof( dataBuffer ) );
		filePressure = open( pressureRead[i_p], O_RDONLY );//O_RDWR
		read( filePressure, dataBuffer, sizeof( dataBuffer) );
		close( filePressure );
		pressure = atof( dataBuffer );
		pressure *= 0.25;
		pressure /= 100;
		printf( "Pressure hPa value = %i \n", (int)pressure);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "&Test 6: OK\n@Pressure sensor is available, T=%.2f, P=%.2f#\n", temperature, pressure);
		USB_printf(buf, 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 6: OK\n\x1b[2C@Pressure sensor is available, T=%.2f, P=%.2f#\n", temperature, pressure);
		write(fd_fb, buf, cnt_byte);

	close( fileTemp );
	close( filePressure );

	return 0;
}

int FuncSARA_Module_Testing_Power_Antenna_Permission(int Do)
{
	//Test sending power on to device on UART port
	//Test sending power on to device on USB port
	//Test toggling GPIO signal for RF antenna selection for UART modem versus USB modem

	#define BUFF_SIZE 100
	int LaraErr=0;
	memset(LaraBuffer, 0, sizeof(LaraBuffer));

	if(!Do) return -1;

	char buf[200];
	int cnt_byte=0;

	USB_printf("**USB Modem Test**\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[1;3H**USB Modem Test**\n");
	write(fd_fb, buf, cnt_byte);

		//setup GPIO
	printf( "Setup GPIO for SARA-R410M \n" );

		if (Init_GPIO("50", "out")!=1)
		{
			sprintf(LaraBuffer, "Error export pins");

			USB_printf("^Test 9A: Fail, error export pins\n@Error export pins#\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9A: Fail, error export pins\n\x1b[2C@Error export pins#\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}
		if (Init_GPIO("52", "out")!=1)
		{
			sprintf(LaraBuffer, "Error export pins");

			USB_printf("^Test 9A: Fail, error export pins\n@Error export pins#\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9A: Fail, error export pins\n\x1b[2C@Error export pins#\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}

	//----------------------Power test modem via UART-------------------------------------------
	printf( "SARA-R410M power-on\n" );

		if (Write_GPIO("50", "1")!=1)
		{
			sprintf(LaraBuffer, "Error write pin 50");

			USB_printf("^Test 9A: Fail, error write pins\n@Error write pins#\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9A: Fail, error write pins\n\x1b[2C@Error write pins#\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}
	usleep(500000);//sleep(1);

	if (Write_GPIO("52", "0")!=1)
	{
		sprintf(LaraBuffer, "Error write pin 52");

		USB_printf("^Test 9A: Fail, error write pins\n@Error write pins#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9A: Fail, error write pins\n\x1b[2C@Error write pins#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	usleep(100000);//sleep(1);

	if (Write_GPIO("52", "1")!=1)
	{
		sprintf(LaraBuffer, "Error write pin 52");

		USB_printf("^Test 9A: Fail, error write pins\n@Error write pins#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9A: Fail, error write pins\n\x1b[2C@Error write pins#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	usleep(200000);//sleep(1);

	if (Write_GPIO("52", "0")!=1)
	{
		sprintf(LaraBuffer, "Error write pin 52");

		USB_printf("^Test 9A: Fail, error write pins\n@Error write pins#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9A: Fail, error write pins\n\x1b[2C@Error write pins#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	usleep(3000000);
	//ttymxc1
	printf( "Send 'ATE0' to SARA-R410M\n" );

	if(Init_LARA_SARA("/dev/ttymxc1", 115200) == -1){
		printf( "ERROR init SARA-R410M! \n" );
		sprintf(LaraBuffer, "ERROR init SARA-R410M");
		LaraErr = 1;//return testFailed;
	}
	else{
		printf( "LARA answer 'OK' \n" );
	}
	if(LaraErr){

		USB_printf("^Test 9A: Fail, error init module\n@Error init module#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9A: Fail, error init module\n\x1b[2C@Error init module#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}

	printf( "Unexport GPIO for SARA-R410M \n" );

		if (DeInit_GPIO("50")!=1)
		{
			sprintf(LaraBuffer, "Error unexport pin 50");

			USB_printf("^Test 9A: Fail, error unexport pins\n@Error unexport pins#\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9A: Fail, error unexport pins\n\x1b[2C@Error unexport pins#\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}
		if (DeInit_GPIO("52")!=1)
		{
			sprintf(LaraBuffer, "Error unexport pin 52");

			USB_printf("^Test 9A: Fail, error unexport pins\n@Error unexport pins#\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9A: Fail, error unexport pins\n\x1b[2C@Error unexport pins#\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}

	sprintf(LaraBuffer, "SARA-R410M module received ATE0 command");

	USB_printf("&Test 9A: OK\n@Module received ATE0 command#\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 9A: OK\n\x1b[2C@Module received ATE0 command#\n");
	write(fd_fb, buf, cnt_byte);

	return 0;
}

int FuncLARA_Module_Testing_Power_Antenna_Permission(int Do)
{
	//Test sending power on to device on UART port
	//Test sending power on to device on USB port
	//Test toggling GPIO signal for RF antenna selection for UART modem versus USB modem

	#define BUFF_SIZE 100
	int SaraErr=0;
	memset(SaraBuffer, 0, sizeof(SaraBuffer));


	if(!Do) return -1;

	char buf[200];
	int cnt_byte=0;

	USB_printf("**UART Modem Test**\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C**UART Modem Test**\n");
	write(fd_fb, buf, cnt_byte);

		//setup GPIO
	printf( "Setup GPIO for SARA-U201 \n" );

		if (Init_GPIO("48", "out")!=1)
		{
			sprintf(SaraBuffer, "Error export pins");

			USB_printf("^Test 9B: Fail, error export pins\n@Error export pins#\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9B: Fail, error export pins\n\x1b[2C@Error export pins#\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}
		if (Init_GPIO("49", "out")!=1)
		{
			sprintf(SaraBuffer, "Error export pins");

			USB_printf("^Test 9B: Fail, error export pins\n@Error export pins#\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9B: Fail, error export pins\n\x1b[2C@Error export pins#\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}

	//----------------------Power test modem via USB------------------------------------------
	usleep(1000000);

	if (Write_GPIO("49", "1")!=1)
	{
		sprintf(SaraBuffer, "Error write pin 49");

		USB_printf("^Test 9B: Fail, error write pins\n@Error write pins#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9B: Fail, error write pins\n\x1b[2C@Error write pins#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	//TODO: проверить появился ли порт ttyACM0
	usleep(12000000);//sleep(12);
	printf( "Send 'ATE0' to SARA-U201\n" );

	if(Init_LARA_SARA("/dev/ttyACM0", 115200) == -1){
		printf( "ERROR init SARA-U201! \n" );
		sprintf(SaraBuffer, "ERROR init SARA-U201");
		SaraErr=1;//return testFailed;
	}
	else{
		printf( "SARA answer 'OK' \n" );
	}
	if(SaraErr){

		USB_printf("^Test 9B: Fail, error init module\n@Error init module#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9B: Fail, error init module\n\x1b[2C@Error init module#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}

	if (Write_GPIO("48", "1")!=1)
	{
		sprintf(SaraBuffer, "Error write pin 48");

		USB_printf("^Test 9B: Fail, error write pins\n@Error write pins#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9B: Fail, error write pins\n\x1b[2C@Error write pins#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}

	sleep(1);

	if (Write_GPIO("48", "0")!=1)
	{
		sprintf(SaraBuffer, "Error write pin 48");

		USB_printf("^Test 9B: Fail, error write pins\n@Error write pins#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9B: Fail, error write pins\n\x1b[2C@Error write pins#\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}

	printf( "Unexport GPIO for SARA-U201 \n" );

		if (DeInit_GPIO("48")!=1)
		{
			sprintf(SaraBuffer, "Error unexport pin 48");

			USB_printf("^Test 9B: Fail, error unexport pins\n@Error unexport pins#\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9B: Fail, error unexport pins\n\x1b[2C@Error unexport pins#\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}
		if (DeInit_GPIO("49")!=1)
		{
			sprintf(SaraBuffer, "Error unexport pin 49");

			USB_printf("^Test 9B: Fail, error unexport pins\n@Error unexport pins#\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 9B: Fail, error unexport pins\n\x1b[2C@Error unexport pins#\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}

		sprintf(SaraBuffer, "SARA-U201 module received ATE0 command");

		USB_printf("&Test 9B: OK\n@Module received ATE0 command#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 9B: OK\n\x1b[2C@Module received ATE0 command#\n");
		write(fd_fb, buf, cnt_byte);


	return 0;
}

int Cameras_Test(int Do, CAMPARAM* camptr1, CAMPARAM* camptr2)
{
	int state1=-1;
	int state2=-1;

	if(!Do) return -1;

	printf("Read camera parameters...\n");
	sleep(1);
	state1=(Read_Cam_Param("/dev/video0", camptr1) ? -1 : 0);
	state2=(Read_Cam_Param("/dev/video1", camptr2) ? -1 : 0);

	if ((state1==0)&&(state2==0)) return 0;
	else return -1;
}

int Cameras_Test_Full(int Do)
{
	if(!Do) return -1;

	char buf[200];
	int cnt_byte=0;

	CAMPARAM cam1={0};
	CAMPARAM cam2={0};

	USB_printf("**Cameras Test**\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[1;3H**Cameras Test**\n");
	write(fd_fb, buf, cnt_byte);

		if (Cameras_Test(Do, &cam1, &cam2)==0)
		{
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "&Test 12: OK\n@Parameters CAM1 f: %s res: %ix%i CAM2 f: %s res: %ix%i#\n", cam1.description, cam1.widht, cam1.height,
																															cam2.description, cam2.widht, cam2.height);
			USB_printf(buf, 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 12: OK\n\x1b[2C@Parameters CAM1 f: %s res: %ix%i CAM2 f: %s res: %ix%i#\n", cam1.description, cam1.widht, cam1.height,
																																		cam2.description, cam2.widht, cam2.height);
			write(fd_fb, buf, cnt_byte);
			return 0;
		}
		else
		{
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "^Test 12: Fail\n@Parameters CAM1 f: %s res: %ix%i CAM2 f: %s res: %ix%i#\n", cam1.description, cam1.widht, cam1.height,
																																cam2.description, cam2.widht, cam2.height);
			USB_printf(buf, 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 12: Fail\n\x1b[2C@Parameters CAM1 f: %s res: %ix%i CAM2 f: %s res: %ix%i#\n", cam1.description, cam1.widht, cam1.height,
																																			cam2.description, cam2.widht, cam2.height);
			write(fd_fb, buf, cnt_byte);
			return 0;
		}
return 0;
}

int Audio_Codec_Test(int Do)
{
	FILE * hiddenConsole;
	char Answer[ANSWER_L];
	int lastchar;
	int result = 0;
	memset(AudioCodecBuffer, 0, sizeof(AudioCodecBuffer));

	if(!Do) return -1;

	char buf[200];
	int cnt_byte=0;

	USB_printf("**Audio System (Codec/Amplifier test)**\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[1;3H**Audio System (Codec/Amplifier test)**\n");
	write(fd_fb, buf, cnt_byte);

	hiddenConsole = popen("cat /sys/class/i2c-dev/i2c-1/device/1-0038/uevent", "r");
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("hiddenConsole answer: \n%s", Answer);
	if(lastchar == 0)
	{
		printf("\n");
		sprintf (AudioCodecBuffer, "Audio codec not detected!");

		USB_printf("^Test 11: Fail, audio codec not detected\n@Audio codec not detected#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 11: Fail, audio codec not detected\n\x1b[2C@Audio codec not detected#\n");
		write(fd_fb, buf, cnt_byte);

		result=-1;
	}
	else
	{
		char* ptr1=NULL;
		char* ptr2=NULL;
		ptr1=strstr(Answer, "DRIVER");
			if (ptr1!=NULL)
			{
				ptr2=strchr(Answer, '\n');
				strncpy(AudioCodecBuffer, ptr1, (ptr2-ptr1));
			}
		ptr1=strstr(Answer, "OF_NAME");
			if (ptr1!=NULL)
			{
				ptr2=strchr(ptr1, '\n');
				strcat(AudioCodecBuffer, " ");
				strncat(AudioCodecBuffer, ptr1, (ptr2-ptr1));
			}
	printf("Audio codec: %s \n", AudioCodecBuffer);
	result=0;

	}
	if (result==0)
	{
		system("alsactl restore");

		USB_printf("@Recording is started. Time is 10 seconds#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Recording is started. Time is 10 seconds#\n");
		write(fd_fb, buf, cnt_byte);

		system("arecord -f dat -d 10 test.wav");
		system("amixer sset 'Lineout' 100%");

		USB_printf("@Playing sound is started#\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Playing sound is started#\n");
		write(fd_fb, buf, cnt_byte);

		system ("aplay -f dat test.wav");
		system("alsactl restore");

		USB_printf("&Test 11: OK\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 11: OK\n");
		write(fd_fb, buf, cnt_byte);

	}

return result;
}

int NEO_Test(int Do)
{
	int fd;
	char buf_rx[200]={0};

		if(!Do) return -1;

		char buf[200];
		int cnt_byte=0;

		USB_printf("**GPS Test**\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[1;3H**GPS Test**\n");
		write(fd_fb, buf, cnt_byte);

		if (Init_GPIO("63", "out")!=1)
		{
			printf("Error export pins 63\n");

			USB_printf("^Test 10: Fail, error export pins\n@Error export pins#\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 10: Fail, error export pins\n\x1b[2C@Error export pins#\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}
		else
		{
			if (Write_GPIO("63", "1")!=1)
			{

				USB_printf("^Test 10: Fail, error write pins\n@Error write pins#\n", 1000);

				memset(buf, 0, 200);
				cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 10: Fail, error write pins\n\x1b[2C@Error write pins#\n");
				write(fd_fb, buf, cnt_byte);

				return -1;
			}
		}
		sleep(1);

	fd=OpenPort("/dev/ttymxc2");
		if (fd<0)
		{
			printf("Error while open port\n");

			USB_printf("^Test 10: Fail, error while open port\n@Error while open port#\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 10: Fail, error while open port\n\x1b[2C@Error while open port#\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}
	SetPort(fd, 9600);
	sleep(1);
	cnt_byte=ReadPort(fd, (unsigned char*)buf_rx, sizeof(buf_rx));
		if (cnt_byte>0)
		{
			if (strstr(buf_rx, "$")!=NULL)
			{
				close(fd);
				Write_GPIO("63", "0");
				DeInit_GPIO("63");

				USB_printf("&Test 10: OK\n@Module received NMEA#\n", 1000);

				memset(buf, 0, 200);
				cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 10: OK\n\x1b[2C@Module received NMEA#\n");
				write(fd_fb, buf, cnt_byte);

				return 0;
			}
			else
			{
				close(fd);
				Write_GPIO("63", "0");
				DeInit_GPIO("63");

				USB_printf("^Test 10: Fail, error unexport pins\n@Error unexport pins#\n", 1000);

				memset(buf, 0, 200);
				cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 10: Fail, error unexport pins\n\x1b[2C@Error unexport pins#\n");
				write(fd_fb, buf, cnt_byte);

				return -1;
			}
		}
		else
		{
			close(fd);
			Write_GPIO("63", "0");
			DeInit_GPIO("63");

			USB_printf("^Test 10: Fail, error unexport pins\n@Error unexport pins#\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 10: Fail, error unexport pins\n\x1b[2C@Error unexport pins#\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}

close(fd);
Write_GPIO("63", "0");
DeInit_GPIO("63");

USB_printf("^Test 10: Fail, error unexport pins\n@Error unexport pins#\n", 1000);

memset(buf, 0, 200);
cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 10: Fail, error unexport pins\n\x1b[2C@Error unexport pins#\n");
write(fd_fb, buf, cnt_byte);

return -1;
}


int Init_LARA_SARA(char* port_name, int port_speed){
	char buf_tx[1000]={0};
	char buf_rx[1000]={0};
	int cnt_byte;
	//static
	int port_id;

	port_id=OpenPort(port_name);
	SetPort(port_id, port_speed);
	usleep(1000000);

	ClosePort(port_id);
	usleep(500000);


	///----***----***---***---***---***---***---***---***--
	port_id=OpenPort(port_name);
	printf( "Port ID: %i\n", port_id);
	SetPort(port_id, port_speed);

	sleep(5);

	cnt_byte=sprintf(buf_tx, "ATE0\r");	 // echo off
	puts (buf_tx);
	WritePort(port_id, (unsigned char*)buf_tx, cnt_byte);
	usleep(500000);
	cnt_byte=ReadPort(port_id, (unsigned char*)buf_rx, sizeof(buf_rx));
	puts(buf_rx);
	if (strstr(buf_rx, "OK")==NULL) //SARA-U201-03B-00 //LARA-R204-02B-00
	{
		perror("\r\nError while reading\r\n");
		ClosePort(port_id);
		return -1;
	}

	memset(buf_tx, 0, 1000);
	memset(buf_rx, 0, 1000);
	ClosePort(port_id);
	return 0;

}



void power_init(void)
{
	int fd;
	int n,m;
	char buf[20] = {0};

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if(fd < 0)
	{
		printf("Error while opening - %d\n", fd);
		return;
	}

	n = snprintf(buf, sizeof(buf), "63");
	m = write(fd, buf, n);
	if(m != n)
	{
		printf("Error, not writing\n");
		return;
	}

	m = close(fd);
	if (m != 0)
	{
		printf("Error while closing\n");
		return;
	}
}


void power_set_direction(void)
{
	int fd;
	int n,m;
	char buf[20] = {0};


	fd = open("/sys/class/gpio/gpio63/direction", O_WRONLY);
	if(fd < 0)
	{
		printf("Error while opening - %d\n", fd);
		return;
	}

	n = snprintf(buf, sizeof(buf), "out");
	m = write(fd, buf, n);
	if(m != n)
	{
		printf("Error, not writing\n");
		return;
	}

	m = close(fd);
	if (m != 0)
	{
		printf("Error while closing\n");
		return;
	}
}

void power_set_on(void)
{
	int fd;
	int n,m;
	char buf[20] = {0};


	fd = open("/sys/class/gpio/gpio63/value", O_WRONLY);
	if(fd < 0)
	{
		printf("Error while opening - %d\n", fd);
		return;
	}

	n = snprintf(buf, sizeof(buf), "1");
	m = write(fd, buf, n);
	if(m != n)
	{
		printf("Error, not writing\n");
		return;
	}

	m = close(fd);
	if (m != 0)
	{
		printf("Error while closing\n");
		return;
	}
}


void power_set_off(void)
{
	int fd;
	int n,m;
	char buf[20] = {0};


	fd = open("/sys/class/gpio/gpio63/value", O_WRONLY);
	if(fd < 0)
	{
		printf("Error while opening - %d\n", fd);
		return;
	}

	n = snprintf(buf, sizeof(buf), "0");
	m = write(fd, buf, n);
	if(m != n)
	{
		printf("Error, not writing\n");
		return;
	}

	m = close(fd);
	if (m != 0)
	{
		printf("Error while closing\n");
		return;
	}
}
