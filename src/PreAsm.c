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
#include <termios.h>
#include <sys/epoll.h>


#include "PreAsm.h"
#include "eeprom.h"
#include "i2c.h"
#include "ComPort.h"
#include "GPIO.h"
#include "FrameBuffer.h"
#include "AudioCodec.h"

#define BUF_SIZE_DISP 200

#define BUFFER_SIZE_SPI 512
#define SPI_NOR_DEV_NAME		"/dev/mtdblock0"
#define SPI_NOR_SIZE			0x400000ul

#define USB_MODEM_DESING "8A"
#define UART_MODEM_DESING "8B"
#define USB_READ_TIMEOUT 30000

extern FILE *stdin;
extern int fd_fb;

extern int flush_flag;
extern pthread_mutex_t flush_mutex;

#define USB_PATH "/dev/ttyGS0"
extern int get_line(char* str, int size, int timeout);



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

int USB_getc(int timeout)
{
	FILE *f;
	int ch;

	struct pollfd pollin={STDIN_FILENO, POLLIN|POLLPRI};

		f = fopen(USB_PATH, "r");
			if (f == NULL)
			{
				//printf("Fail open usb!\n");
				return -1;
			}

		pollin.fd = fileno(f);
		tcflush(fileno(f), TCIOFLUSH);
		//printf("flush usb!\n");

			if( poll(&pollin, 1, timeout) )
			{
				ch=getc(f);
			}
			else
			{
				//Timeout
				ch=-1;
			}
		fclose(f);
		//printf("read usb!\n");
		return ch;
}

int USB_getline(char* str, int size, int timeout)
	{

	/*struct pollfd pollin = { STDIN_FILENO, POLLIN|POLLPRI };
	FILE *usbcon;
	char *line;
	ssize_t read;
	int ret = 0;
	int len;

	len = size+1;
	line = (void *)malloc(len);
	memset(line, 0, len);



	usbcon = fopen(USB_PATH, "r");
	if (usbcon == NULL)
	{
		ret = -1;
		printf("Error not opened\n");
		goto close_l;
	}


	tcflush(fileno(usbcon), TCIOFLUSH);

	pollin.fd = fileno(usbcon);
	pollin.events = POLLIN | POLLPRI;

	if( poll(&pollin, 1, timeout) )
	{
		fgets(line, len, usbcon);
	}
	else
	{
		//Timeout
		//printf("Error timeout\n");
		ret = -1;
	}

	if(ret == 0 )
	{
		if(len != NULL)
			strncpy(str, line, size);
		//printf("Enter - %s\n", str);
	}

close_l:
	if(usbcon)
		fclose(usbcon);
	if(line)
		free(line);

	return ret;*/

		static FILE *f;
		char state;
		int epfd;
		int nfds;
		static struct epoll_event ev;

		static int flag=0;

		epfd = epoll_create(1);
		if (flag==0)
		{


			f = fopen(USB_PATH, "r");
			//int flags = fcntl(fileno(f), F_GETFL, 0);
			//fcntl(fileno(f), F_SETFL, flags | O_NONBLOCK);

				if (f == NULL)
				{
					printf("Fail open usb!\n");
					close(epfd);
					return -1;
				}
		tcflush(fileno(f), TCIOFLUSH);
		flag=1;
		}

		pthread_mutex_lock(&flush_mutex);
			if (flush_flag==1)
			{
				tcflush(fileno(f), TCIOFLUSH);
				flush_flag=0;
			}
		pthread_mutex_unlock(&flush_mutex);

		ev.events = EPOLLIN | EPOLLPRI ;
		ev.data.fd = fileno(f);
		epoll_ctl(epfd, EPOLL_CTL_ADD, fileno(f), &ev);

		//tcflush(fileno(f), TCIOFLUSH);
		nfds = epoll_wait(epfd, &ev, 1, timeout);

		if (nfds > 0)
		{
		//fread(str, 1, size, f);
		fgets(str, size+1, f);
		//tcflush(fileno(f), TCIOFLUSH);
		state=0;
		}
		else
		{
		state=-1;
		//tcflush(fileno(f), TCIOFLUSH);
		}

		close(epfd);
		//fclose(f);
		return state;
	}

int USB_printf(char* buf, int timeout)
{
	int fd=-1;
	int cnt=0;

	fd=open(USB_PATH, O_WRONLY);
		if (fd>=0)
		{
			struct pollfd polldesc={fd, POLLOUT|POLLPRI};
			//tcflush(fd, TCIOFLUSH);
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
		else
		{
			return -1;
		}
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

	USB_printf("**EMMC Integrity Test**\n", 1000);

	memset(buf, 0, 200);
	//cnt_byte=snprintf(buf, sizeof(buf), "\x1b[1;3H**EMMC Integrity Test**\n");
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C**EMMC Integrity Test**\n");
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

		USB_printf("@Fail while open EMMC#\n^Test 2A: Fail\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Fail while open EMMC#\n\x1b[2C^Test 2A: Fail\n");
		write(fd_fb, buf, cnt_byte);

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

		USB_printf("@Fail while open EMMC#\n^Test 2A: Fail\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Fail while open EMMC#\n\x1b[2C^Test 2A: Fail\n");
		write(fd_fb, buf, cnt_byte);

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

		USB_printf("@Fail while open EMMC#\n^Test 2A: Fail\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Fail while open EMMC#\n\x1b[2C^Test 2A: Fail\n");
		write(fd_fb, buf, cnt_byte);

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

		USB_printf("@Fail while open EMMC#\n^Test 2A: Fail\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Fail while open EMMC#\n\x1b[2C^Test 2A: Fail\n");
		write(fd_fb, buf, cnt_byte);

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

			USB_printf("@Output and input buffer are not equal#\n^Test 2A: Fail\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Output and input buffer are not equal#\n\x1b[2C^Test 2A: Fail\n");
			write(fd_fb, buf, cnt_byte);


			return -1;
		}
	}

	printf("Output and input buffer are equal\n\n");

	USB_printf("&Test 2A: OK\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 2A: OK\n");
	write(fd_fb, buf, cnt_byte);

	return 0;

}

int FuncSPI_32MBit_NOR_Flash(int Do)
{
	//Performs checksum validation on all memory parts to ensure memory is free of corruption or defect. This includes testing
	char buffer_out[BUFFER_SIZE_SPI];
	char buffer_in[BUFFER_SIZE_SPI];
	char buffer_data[BUFFER_SIZE_SPI];

	off_t current_offset = 0;
	int fd, memory_size;
	int i, FailFlag=0;

	char buf[200];
	int cnt_byte=0;

	int dot_factor = 0;

	if(!Do) return -1;

	memset(buffer_in, 0, BUFFER_SIZE_SPI);
	memset(buffer_out, 0, BUFFER_SIZE_SPI);
	memset(buffer_data, 0, BUFFER_SIZE_SPI);

	printf("\n*** Memory Test Application ***\n\n");

	printf("Generate output buffer...");

	USB_printf("**32MBit NOR Flash Integrity Test**\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C**32MBit NOR Flash Integrity Test**\n");
	write(fd_fb, buf, cnt_byte);

	srand(time(NULL));
		for(i=0; i<BUFFER_SIZE_SPI; i++)
		{
			buffer_out[i] = rand() % 255;
		}
	printf("ok\n\n");


	fd = open(SPI_NOR_DEV_NAME, O_RDWR);
			if(fd < 0 )
			{
				printf("Error: %d\n", fd);

				USB_printf("^Test 2B: Fail\n@Fail while open SPI NOR flash#\n", 1000);

				memset(buf, 0, 200);
				cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 2A: Fail\n\x1b[2C@Fail while open SPI NOR flash#\n");
				write(fd_fb, buf, cnt_byte);
				return -1;
			}
			printf("ok\n");


			FailFlag = 0;
			memory_size = SPI_NOR_SIZE;


			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CChecking Memory Integrity");
			write(fd_fb, buf, cnt_byte);
			USB_printf("Checking Memory Integrity", 1000);

					while ((current_offset + BUFFER_SIZE_SPI) <= memory_size)
					{
						printf("Testing block #%d\t-->\t", (int)(current_offset / BUFFER_SIZE_SPI + 1));


						if(current_offset > dot_factor )
						{
							memset(buf, 0, 200);
							cnt_byte=snprintf(buf, sizeof(buf), "\x1b[1C.");
							write(fd_fb, buf, cnt_byte);

							USB_printf(".", 200);

							dot_factor += (int)(SPI_NOR_SIZE / 40);
						}


						lseek(fd, current_offset, SEEK_SET);
						i = read(fd, buffer_data, BUFFER_SIZE_SPI);
						if(i != BUFFER_SIZE_SPI)
						{
							printf("Reading memory\tFAILED\n");
							break;
						}

						lseek(fd, current_offset, SEEK_SET);

						i = write(fd, buffer_out, BUFFER_SIZE_SPI);
						if(i != BUFFER_SIZE_SPI)
						{
							printf("Writing test buffer to memory\tFAILED\n");
							FailFlag = 1;
						}
						else
						{
							lseek(fd, current_offset, SEEK_SET);
							i = read(fd, buffer_in, BUFFER_SIZE_SPI);
							if(i != BUFFER_SIZE_SPI)
							{
								printf("Reading test buffer from memory\tFAILED\n");
								FailFlag = 1;
							}
							else
							{
								for(i=0; i<BUFFER_SIZE_SPI; i++ )
								{
									if(buffer_in[i] != buffer_out[i])
									{
										printf("WARNING: output and input buffer are not equal\n");
										FailFlag = 1;
										break;
									}
								}
							}
						}

						lseek(fd, current_offset, SEEK_SET);

						i = write(fd, buffer_data, BUFFER_SIZE_SPI);
						if(i != BUFFER_SIZE_SPI)
						{
							printf("Writing data to memory\tFAILED\n");
							FailFlag = 1;
						}

						if (FailFlag) break;
						printf("ok\n");
						current_offset += BUFFER_SIZE_SPI;

						usleep(10000);
					}

					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C\n");
					write(fd_fb, buf, cnt_byte);

					USB_printf("\n", 200);


					printf("\nClose memory device ...");
					i = close(fd);
					if(i != 0 )
					{
						printf("Error: %d\n",i);

						USB_printf("^Test 2B: Fail\n@Fail while close SPI NOR flash#\n", 1000);

						memset(buf, 0, 200);
						cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 2B: Fail\n\x1b[2C@Fail while close SPI NOR flash#\n");
						write(fd_fb, buf, cnt_byte);
						close(fd);
						return -1;//return i;
					}

					if (FailFlag==1)
					{
						USB_printf("^Test 2B: Fail\n@", 1000);

						memset(buf, 0, 200);
						cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 2B: Fail\n");
						write(fd_fb, buf, cnt_byte);
					}
					else
					{
					printf("ok\n\n");

					USB_printf("&Test 2B: OK\n", 1000);

					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 2B: OK\n");
					write(fd_fb, buf, cnt_byte);
					}
				close(fd);
				return 0;




	/*printf("Open SPI NOR...");
	fd = open("/dev/mtdblock0", O_RDWR);
	if(fd < 0 )
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");

	i=read(fd, buffer_save, BUFFER_SIZE_SPI);
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
	for(i=0; i<BUFFER_SIZE_SPI; i++)
	{
		buffer_out[i] = rand() % 255;
	}

	for(i=0; i<BUFFER_SIZE_SPI; i++)
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
	i = write(fd, buffer_out, BUFFER_SIZE_SPI);
	if(i != BUFFER_SIZE_SPI)
	{
		printf("Write only %d bytes instead %d", i, BUFFER_SIZE_SPI);
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
	i = read(fd, buffer_in, BUFFER_SIZE_SPI);
	if(i != BUFFER_SIZE_SPI)
	{
		printf("Read only %d bytes instead %d", i, BUFFER_SIZE_SPI);
	}
	printf("ok\n\n");


	printf("Input buffer:\n");
	for(i=0; i<BUFFER_SIZE_SPI; i++)
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

	i = write(fd, buffer_save, BUFFER_SIZE_SPI);
		if(i != BUFFER_SIZE_SPI)
		{
			printf("Write only %d bytes instead %d", i, 100);
		}

	close(fd);
	printf("ok\n\n");

//------------------------------------------------

	for(i=0; i<BUFFER_SIZE_SPI; i++ )
	{
		if(buffer_in[i] != buffer_out[i])
		{
			printf("WARNING: output and input buffer are not equal\n");
			return -1;
		}
	}

	printf("Output and input buffer are equal\n\n");
	return 0;
	*/
}

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
	//Accelerometer X/Y/Z offset trim needs to be calculate (while board is in a stable flat position). Offset values need to stored in a protected EMMC sector and available for the accelerometer to load on start-up
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
	int Attempt = 0;
	int PositionOkFlag = 0;


	memset(AccelBuffer, 0, sizeof(AccelBuffer));
	if(!Do) return -1;

	printf("Start calibrate\n\n");

	USB_printf("**Accelerometer Check**\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C**Accelerometer Check**\n");
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

		USB_printf("^Test 3: Fail, Accelerometer Is not Responsive\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail, Accelerometer Is not Responsive\n");
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
		USB_printf("^Test 3: Fail, Accelerometer Is not Responsive\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail, Accelerometer Is not Responsive\n");
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

		USB_printf("^Test 3: Fail, Accelerometer Is not Responsive\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail, Accelerometer Is not Responsive\n");
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

		USB_printf("^Test 3: Fail, Accelerometer Is not Responsive\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail, Accelerometer Is not Responsive\n");
		write(fd_fb, buf, cnt_byte);
		return -1;
	}
	//printf("Open: %s  - OK\n" ,AccelZ_Read[indexFile]);

		memset(dataBuffer, 0, sizeof( dataBuffer ));
		sprintf(dataBuffer, "%i",0);
		fdC = open(CalibX_Read[indexFile], O_WRONLY);
		if(fdC == -1)
		{

			USB_printf("^Test 3: Fail, Accelerometer Is not Responsive\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail, Accelerometer Is not Responsive\n");
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
			USB_printf("^Test 3: Fail, Accelerometer Is not Responsive\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail, Accelerometer Is not Responsive\n");
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
			USB_printf("^Test 3: Fail, Accelerometer Is not Responsive\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail, Accelerometer Is not Responsive\n");
			write(fd_fb, buf, cnt_byte);
			return -1;
		}
		write(fdC, dataBuffer, sizeof(dataBuffer));
		close(fdC);

		memset(dataBuffer, 0, sizeof( dataBuffer ));
		fdS = open(AccelS_Read[indexFile], O_RDONLY);
		read(fdS, dataBuffer, sizeof(dataBuffer));
		close(fdS);

//*********************************************************************************************************

	for (Attempt = 0; Attempt < 3; Attempt++) {
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
		}

		ValueX = ValueX/FiltrTime;
		ValueY = ValueY/FiltrTime;
		ValueZ = ValueZ/FiltrTime;
		printf("Accel_X = %i \n", ValueX );
		printf("Accel_Y = %i \n", ValueY );
		printf("Accel_Z = %i \n", ValueZ );

		memset(buf, 0, 200);
		snprintf(buf, sizeof(buf), "Reading Accelerometer Values: x = %d, y = %d, z = %d\n", ValueX,ValueY,ValueZ);
		USB_printf(buf, 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CReading Accelerometer Values: x = %d, y = %d, z = %d\n", ValueX,ValueY,ValueZ);
		write(fd_fb, buf, cnt_byte);

		if ((ValueX<50) && (ValueX>-50) &&\
			(ValueY<50) && (ValueY>-50) &&\
			(ValueZ>1000) && (ValueZ<1048)) {

			CalibX=-ValueX/2;
			CalibY=-ValueY/2;
			CalibZ=(1024-ValueZ)/2;

			PositionOkFlag = 1;
			break;

		} else if (Attempt < 2){
			printf("Board is out of position. Waiting for user to hit enter\n");
			USB_printf("@Board is out of position. Press Enter to Retry#\n", 1000);

			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Board is out of position. Press Enter to Retry#\n\n");
			write(fd_fb, buf, cnt_byte);

			//while ((char)USB_getc(1000) != '\n');
			USB_getc(10000);
		}

	}
//**********************************************************************************************
	if (!PositionOkFlag) {
		printf("Test 3: Fail, Bad Device Position or Vibration\n");
		USB_printf("^Test 3: Fail, Bad Device Position or Vibration\n", 1000);

		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail, Bad Device Position or Vibration\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}


	memset(dataBuffer, 0, sizeof( dataBuffer ));
	sprintf(dataBuffer, "%i",CalibX);
	fdC = open(CalibX_Read[indexFile], O_WRONLY);
	if(fdC == -1)
	{
		printf("^Test 3: Fail, Unable to open device file for writing calibration value!\n");
		USB_printf("^Test 3: Fail, Unable to open device file for writing calibration value!\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail, Unable to open device file for writing calibration value!\n");
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

		printf("^Test 3: Fail, Unable to open device file for writing calibration value!\n");
		USB_printf("^Test 3: Fail, Unable to open device file for writing calibration value!\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail, Unable to open device file for writing calibration value!\n");
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

		printf("^Test 3: Fail, Unable to open device file for writing calibration value!\n");
		USB_printf("^Test 3: Fail, Unable to open device file for writing calibration value!\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail, Unable to open device file for writing calibration value!\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	write(fdC, dataBuffer, sizeof(dataBuffer));
	close(fdC);

	//Store Calibrate in file
	memset(buf, 0, 200);
	snprintf(buf, sizeof(buf), "Calibration Trim Values: calX = %d, calY = %d, calZ = %d\nSaving Offset to EEPROM Memory...\n", CalibX,CalibY,CalibZ);
	USB_printf(buf, 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CCalibration Trim Values: calX = %d, calY = %d, calZ = %d\n\x1b[2CSaving Offset to EEPROM Memory...\n", CalibX,CalibY,CalibZ);
	write(fd_fb, buf, cnt_byte);


	sprintf(dataBuffer, "%i %i %i ",CalibX,CalibY,CalibZ);
	if (Write_EEPROM(dataBuffer, 19)==0)
	{
		USB_printf("^Test 3: Fail, Error while writing calibration values to EEPROM!\n", 1000);
		printf("^Test 3: Fail, Error while writing calibration values to EEPROM!\n");

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 3: Fail, Error while writing calibration values to EEPROM!\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	else
	{
		memset(buf, 0, 200);
		snprintf(buf, sizeof(buf), "Confirming Saved Trim Value: calX = %d, calY = %d, calZ = %d\n", CalibX,CalibY,CalibZ);
		USB_printf(buf, 1000);
		USB_printf("&Test 3: OK\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CConfirming Saved Trim Value: calX = %d, calY = %d, calZ = %d\n", CalibX,CalibY,CalibZ);
		write(fd_fb, buf, cnt_byte);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 3: OK\n");
		write(fd_fb, buf, cnt_byte);

		printf("&Test 3: OK\n");
	}
	return 0;
}


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

		USB_printf("@Charger is available#\n&Test 4B: OK\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Charger is available#\n\x1b[2C&Test 4B: OK\n");
		write(fd_fb, buf, cnt_byte);
		return 0;
	}
	else{

		USB_printf("@Charger don't answer#\n^Test 4B: Fail\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Charger don't answer#\n\x1b[2C^Test 4B: Fail\n");
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
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C**Power Management Test**\n");
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

		USB_printf("@PMIC is available#\n&Test 4A: OK\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@PMIC is available#\n\x1b[2C&Test 4A: OK\n");
		write(fd_fb, buf, cnt_byte);

		return 0;
	}
	else{

		USB_printf("@PMIC don't answer#\n^Test 4A: Fail\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@PMIC don't answer#\n\x1b[2C^Test 4A: Fail\n");
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
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C**Wifi Test**\n");
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

		USB_printf("@Module sd8xxx not loaded#\n^Test 7: Fail, module sd8xxx not loaded\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Module sd8xxx not loaded#\n\x1b[2C^Test 7: Fail, module sd8xxx not loaded\n");
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

		USB_printf("@Module mlan not loaded#\n^Test 7A: Fail, module mlan not loaded\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Module mlan not loaded#\n\x1b[2C^Test 7A: Fail, module mlan not loaded\n");
		write(fd_fb, buf, cnt_byte);

		result=-1;
	}
	//print networks
	printf("\n\nScanning WiFi networks... Wait 10 sec...\n\n");

	USB_printf("Searching for Wifi Networks...\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CSearching for Wifi Networks...\n");
	write(fd_fb, buf, cnt_byte);

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
	cnt_byte=snprintf(buf, sizeof(buf), "@%s#\n&Test 7A: OK\n", EmmyWiFiBuffer);
	USB_printf(buf, 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@%s#\n\x1b[2C&Test 7A: OK\n", EmmyWiFiBuffer);
	write(fd_fb, buf, cnt_byte);

	//Validate Bluetooth Antenna by detecting and printing available Bluetooth devices to pair

	USB_printf("\n**Bluetooth Test**\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\n\x1b[2C**Bluetooth Test**\n");
	write(fd_fb, buf, cnt_byte);


	USB_printf("Searching for BT Device...\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CSearching for BT Device...\n");
	write(fd_fb, buf, cnt_byte);


	hiddenConsole = popen("lsmod | grep bt8xxx", "r");
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("hiddenConsole answer: \n%s", Answer);
	if(lastchar == 0){
		printf("\n Kernel module 'bt8xxx' not loaded!!! Load module and try again");
		sprintf (EmmyBTBuffer, "Module 'bt8xxx' not loaded");

		USB_printf("@Module bt8xxx not loaded#\n^Test 7B: Fail, module bt8xxx not loaded\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Module bt8xxx not loaded#\n\x1b[2C^Test 7B: Fail, module bt8xxx not loaded\n");
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
		cnt_byte=snprintf(buf, sizeof(buf), "@%s#\n&Test 7B: OK\n", EmmyBTBuffer);
		USB_printf(buf, 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@%s#\n\x1b[2C&Test 7B: OK\n", EmmyBTBuffer);
		write(fd_fb, buf, cnt_byte);

	printf("\nFound bluetooth devices: \n%s", Answer);

	USB_printf("\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\n");
	write(fd_fb, buf, cnt_byte);


	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "/root/NFC_test %s, %i", USB_PATH, fd_fb);
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
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C**Light Sensor Test**\n");
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

		USB_printf("@Light sensor isn't available#\n^Test 5: Fail\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Light sensor isn't available#\n\x1b[2C^Test 5: Fail\n");
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
		cnt_byte=snprintf(buf, sizeof(buf), "@Light sensor is available, lux=%i#\n&Test 5: OK\n", lux);
		USB_printf(buf, 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Light sensor is available, lux=%i#\n\x1b[2C&Test 5: OK\n", lux);
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
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C**Pressure Sensor Test**\n");
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

		USB_printf("@Pressure sensor isn't available#\n^Test 6: Fail, pressure sensor isn't available\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Pressure sensor isn't available#\n\x1b[2C^Test 6: Fail, pressure sensor isn't available\n");
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

		USB_printf("@Pressure sensor isn't available#\n^Test 6: Fail, pressure sensor isn't available\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Pressure sensor isn't available#\n\x1b[2C^Test 6: Fail, pressure sensor isn't available\n");
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
		cnt_byte=snprintf(buf, sizeof(buf), "@Pressure sensor is available, T=%.2f, P=%.2f#\n&Test 6: OK\n", temperature, pressure);
		USB_printf(buf, 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Pressure sensor is available, T=%.2f, P=%.2f#\n\x1b[2C&Test 6: OK\n", temperature, pressure);
		write(fd_fb, buf, cnt_byte);

	close( fileTemp );
	close( filePressure );

	return 0;
}

int FuncLARA_Module_Testing_Power_Antenna_Permission(int Do)
{
	//Test sending power on to device on UART port

	//Test toggling GPIO signal for RF antenna selection for UART modem versus USB modem

	#define BUFF_SIZE 100
	int LaraErr=0;
	memset(LaraBuffer, 0, sizeof(LaraBuffer));

	if(!Do) return -1;

	char buf[200];
	int cnt_byte=0;


	//if (pFile!=NULL) fprintf(pFile,"**UART Modem Test**\n");

	USB_printf("**UART Modem Response Test**\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C**UART Modem Response Test**\n");
	write(fd_fb, buf, cnt_byte);

		//setup GPIO
	printf( "Setup GPIO for UART modem \n" );

	if (Init_GPIO("50", "out")!=1) // init UART modem power enable
	{
		sprintf(LaraBuffer, "Error export pins");

		//if (pFile!=NULL) fprintf(pFile, "^Test 9A: Fail, error export pins\n@Error export pins#\n");
		//fclose (pFile);

		USB_printf("@Error export pins#\n^Test 8B: Fail, error export pins\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error export pins#\n\x1b[2C^Test 8B: Fail, error export pins\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	if (Init_GPIO("48", "out")!=1) // init modem antenna/SIM selector
	{
		sprintf(LaraBuffer, "Error export pins");

		//if (pFile!=NULL) fprintf(pFile, "^Test 9A: Fail, error export pins\n@Error export pins#\n");
		//fclose (pFile);

		USB_printf("@Error export pins#\n^Test 8B: Fail, error export pins\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error export pins#\n\x1b[2C^Test 8B: Fail, error export pins\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
	if (Init_GPIO("52", "out")!=1) // init modem ON pin
	{
		sprintf(LaraBuffer, "Error export pins");

		//if (pFile!=NULL) fprintf(pFile, "^Test 9A: Fail, error export pins\n@Error export pins#\n");
		//fclose (pFile);

		USB_printf("@Error export pins#\n^Test 8B: Fail, error export pins\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error export pins#\n\x1b[2C^Test 8B: Fail, error export pins\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}

	//----------------------Power test modem via UART-------------------------------------------
	printf( "UART modem power-on\n" );


	if (Write_GPIO("48", "0")!=1)
	{
		sprintf(LaraBuffer, "Error write pin 48");

		//if (pFile!=NULL) fprintf(pFile, "^Test 9A: Fail, error write pins\n@Error write pins#\n");
		//fclose (pFile);

		USB_printf("@Error write pins#\n^Test 8B: Fail, error write pins\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error write pins#\n\x1b[2C^Test 8B: Fail, error write pins\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}

	if (Write_GPIO("50", "1")!=1)
	{
		sprintf(LaraBuffer, "Error write pin 50");
		//if (pFile!=NULL) fprintf(pFile, "^Test 9A: Fail, error write pins\n@Error write pins#\n");
		//fclose (pFile);

		USB_printf("@Error write pins#\n^Test 8B: Fail, error write pins\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error write pins#\n\x1b[2C^Test 8B: Fail, error write pins\n");
		write(fd_fb, buf, cnt_byte);

		return -1;

	}

	usleep(200000);

	if (Write_GPIO("52", "1")!=1)
	{
		sprintf(LaraBuffer, "Error write pin 50");
		//if (pFile!=NULL) fprintf(pFile, "^Test 9A: Fail, error write pins\n@Error write pins#\n");
		//fclose (pFile);

		USB_printf("@Error write pins#\n^Test 8B: Fail, error write pins\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error write pins#\n\x1b[2C^Test 8B: Fail, error write pins\n");
		write(fd_fb, buf, cnt_byte);

		return -1;

	}

	usleep(500000);

	if (Write_GPIO("52", "0")!=1)
	{
		sprintf(LaraBuffer, "Error write pin 50");
		//if (pFile!=NULL) fprintf(pFile, "^Test 9A: Fail, error write pins\n@Error write pins#\n");
		//fclose (pFile);

		USB_printf("@Error write pins#\n^Test 8B: Fail, error write pins\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error write pins#\n\x1b[2C^Test 8B: Fail, error write pins\n");
		write(fd_fb, buf, cnt_byte);

		return -1;

	}

	usleep(500000);

	LaraErr = Init_LARA_SARA("/dev/ttymxc1", 115200);

	switch (LaraErr) {
		case 0:
			break;
		case -1:
		case -2:
		{
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "^Test 8B: Fail, Modem Not Responsive\n");
			USB_printf(buf, 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 8B: Fail, Modem Not Responsive\n");
			write(fd_fb, buf, cnt_byte);
			break;
		}
		case -3:
		{
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "^Test 8B: Fail, SIM Card Not Readable\n");
			USB_printf(buf, 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 8B: Fail, SIM Card Not Readable\n");
			write(fd_fb, buf, cnt_byte);
			break;
		}
		default:
		{
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "^Test 8B: Fail, non-expected answer!!!\n");
			USB_printf(buf, 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 8B: Fail, non-expected answer!!!\n");
			write(fd_fb, buf, cnt_byte);
			break;
		}
	}

	if (Write_GPIO("50", "0")!=1)
	{
		sprintf(LaraBuffer, "Error write pin 50");
		//if (pFile!=NULL) fprintf(pFile, "^Test 9A: Fail, error write pins\n@Error write pins#\n");
		//fclose (pFile);

		USB_printf("@Error write pins#\n^Test 8B: Fail, error write pins\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error write pins#\n\x1b[2C^Test 8B: Fail, error write pins\n");
		write(fd_fb, buf, cnt_byte);

		return -1;

	}

	printf( "Unexport GPIO for UART \n" );

		if (DeInit_GPIO("50")!=1)
		{
			sprintf(LaraBuffer, "Error unexport pin 50");

			//if (pFile!=NULL) fprintf(pFile, "^Test 9A: Fail, error unexport pins\n@Error unexport pins#\n");
			//fclose (pFile);

			USB_printf("@Error unexport pins#\n^Test 8B: Fail, error unexport pins\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error unexport pins#\n\x1b[2C^Test 8B: Fail, error unexport pins\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}

		if (DeInit_GPIO("48")!=1)
		{
			sprintf(LaraBuffer, "Error unexport pin 48");

			//if (pFile!=NULL) fprintf(pFile, "^Test 9A: Fail, error unexport pins\n@Error unexport pins#\n");
			//fclose (pFile);

			USB_printf("@Error unexport pins#\n^Test 8B: Fail, error unexport pins\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error unexport pins#\n\x1b[2C^Test 8B: Fail, error unexport pins\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}

		if (DeInit_GPIO("52")!=1)
		{
			sprintf(LaraBuffer, "Error unexport pin 48");

			//if (pFile!=NULL) fprintf(pFile, "^Test 9A: Fail, error unexport pins\n@Error unexport pins#\n");
			//fclose (pFile);

			USB_printf("@Error unexport pins#\n^Test 8B: Fail, error unexport pins\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error unexport pins#\n\x1b[2C^Test 8B: Fail, error unexport pins\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}


	return 0;
}

int FuncSARA_Module_Testing_Power_Antenna_Permission(int Do) //
{
	//Test sending power on to device on USB port
	//Test toggling GPIO signal for RF antenna selection for USB modem versus UART modem

	#define BUFF_SIZE 100
	int SaraErr=0;
	memset(SaraBuffer, 0, sizeof(SaraBuffer));


	if(!Do) return -1;

	char buf[200];
	int cnt_byte=0;

	//if (pFile!=NULL) fprintf(pFile,"**USB Modem Test**\n");

	USB_printf("**USB Modem Response Test**\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C**USB Modem Response Test**\n");
	write(fd_fb, buf, cnt_byte);

		//setup GPIO
	printf( "Setup GPIO for USB modem \n" );

	if (Init_GPIO("49", "out")!=1)
		{
			sprintf(SaraBuffer, "Error export pins");

			//if (pFile!=NULL) fprintf(pFile, "^Test 9B: Fail, error export pins\n@Error export pins#\n");
			//fclose (pFile);

			USB_printf("@Error export pins#\n^Test 8A: Fail, error export pins\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error export pins#\n\x1b[2C^Test 8A: Fail, error export pins\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}
		if (Init_GPIO("48", "out")!=1)
		{
			sprintf(SaraBuffer, "Error export pins");

			//if (pFile!=NULL) fprintf(pFile, "^Test 9B: Fail, error export pins\n@Error export pins#\n");
			//fclose (pFile);

			USB_printf("@Error export pins#\n^Test 8A: Fail, error export pins\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error export pins#\n\x1b[2C^Test 8A: Fail, error export pins\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}

	//----------------------Power test modem via USB------------------------------------------
	usleep(1000000);

	if (Write_GPIO("48", "1")!=1)
	{
		sprintf(SaraBuffer, "Error write pin 48");

		//if (pFile!=NULL) fprintf(pFile, "^Test 9B: Fail, error write pins\n@Error write pins#\n");
		//fclose (pFile);

		USB_printf("@Error write pins#\n^Test 8A: Fail, error write pins\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error write pins#\n\x1b[2C^Test 8A: Fail, error write pins\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}

	if (Write_GPIO("49", "1")!=1)
	{
		sprintf(SaraBuffer, "Error write pin 49");

		//if (pFile!=NULL) fprintf(pFile, "^Test 9B: Fail, error write pins\n@Error write pins#\n");
		//fclose (pFile);

		USB_printf("@Error write pins#\n^Test 8A: Fail, error write pins\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error write pins#\n\x1b[2C^Test 8A: Fail, error write pins\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}


	//TODO: проверить появился ли порт ttyACM0
	usleep(12000000);

	SaraErr = Init_LARA_SARA("/dev/ttyACM0", 115200);

	switch (SaraErr) {
		case 0:
			break;
		case -1:
		case -2:
		{
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "^Test 8A: Fail, Modem Not Responsive\n");
			USB_printf(buf, 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 8A: Fail, Modem Not Responsive\n");
			write(fd_fb, buf, cnt_byte);
			break;
		}
		case -3:
		{
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "^Test 8A: Fail, SIM Card Not Readable\n");
			USB_printf(buf, 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 8A: Fail, SIM Card Not Readable\n");
			write(fd_fb, buf, cnt_byte);
			break;
		}
		default:
		{
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "^Test 8A: Fail, non-expected answer!!!\n");
			USB_printf(buf, 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 8A: Fail, non-expected answer!!!\n");
			write(fd_fb, buf, cnt_byte);
			break;
		}
	}

	if (Write_GPIO("49", "0")!=1)
	{
		sprintf(SaraBuffer, "Error write pin 49");

		//if (pFile!=NULL) fprintf(pFile, "^Test 9B: Fail, error write pins\n@Error write pins#\n");
		//fclose (pFile);

		USB_printf("@Error write pins#\n^Test 8A: Fail, error write pins\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error write pins#\n\x1b[2C^Test 8A: Fail, error write pins\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}

	printf( "Unexport GPIO for USB modem \n" );

		if (DeInit_GPIO("48")!=1)
		{
			sprintf(SaraBuffer, "Error unexport pin 48");

			//if (pFile!=NULL) fprintf(pFile, "^Test 9B: Fail, error unexport pins\n@Error unexport pins#\n");
			//fclose (pFile);

			USB_printf("@Error unexport pins#\n^Test 8A: Fail, error unexport pins\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error unexport pins#\n\x1b[2C^Test 8A: Fail, error unexport pins\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}
		if (DeInit_GPIO("49")!=1)
		{
			sprintf(SaraBuffer, "Error unexport pin 49");

			//if (pFile!=NULL) fprintf(pFile, "^Test 9B: Fail, error unexport pins\n@Error unexport pins#\n");
			//fclose (pFile);

			USB_printf("@Error unexport pins#\n^Test 8A: Fail, error unexport pins\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error unexport pins#\n\x1b[2C^Test 8A: Fail, error unexport pins\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}

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
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C**Cameras Test**\n");
	write(fd_fb, buf, cnt_byte);

		if (Cameras_Test(Do, &cam1, &cam2)==0)
		{
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "@Parameters CAM1 f: %s res: %ix%i CAM2 f: %s res: %ix%i#\n&Test 11: OK\n", cam1.description, cam1.widht, cam1.height,
																															cam2.description, cam2.widht, cam2.height);
			USB_printf(buf, 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Parameters CAM1 f: %s res: %ix%i CAM2 f: %s res: %ix%i#\n\x1b[2C&Test 11: OK\n", cam1.description, cam1.widht, cam1.height,
																																		cam2.description, cam2.widht, cam2.height);
			write(fd_fb, buf, cnt_byte);
			return 0;
		}
		else
		{
			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "@Parameters CAM1 f: %s res: %ix%i CAM2 f: %s res: %ix%i#\n^Test 11: Fail\n", cam1.description, cam1.widht, cam1.height,
																																cam2.description, cam2.widht, cam2.height);
			USB_printf(buf, 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Parameters CAM1 f: %s res: %ix%i CAM2 f: %s res: %ix%i#\n\x1b[2C^Test 11: Fail\n", cam1.description, cam1.widht, cam1.height,
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
	char ch[10]={0};
	int state_L=-1;
	int state_R=-1;
	int state=-1;
	int repeat=0;

	USB_printf("**Audio output test**\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C**Audio output test**\n");
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

		USB_printf("^Test 10A: Fail, Audio Codec Not Detected\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@^Test 10A: Fail, Audio Codec Not Detected\n");
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

		USB_printf("Audio Codec Detected\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CAudio Codec Detected\n");
		write(fd_fb, buf, cnt_byte);

		if (Play_Sound()==0)
		{
			system("alsactl restore");

			USB_printf("\n**Audio Input Test**\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\n\x1b[2C**Audio Input Test**\n");
			write(fd_fb, buf, cnt_byte);

			USB_printf("Press Enter to Start 10 second recording\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CPress Enter to Start 10 second recording\n");
			write(fd_fb, buf, cnt_byte);

			ch[0]=(char)(USB_getc(10000));
				if (ch[0]=='\n')
				{
					state=0;
					USB_printf("Recording start...\n", 1000);

					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CRecording start...\n");
					write(fd_fb, buf, cnt_byte);
				}
				else
				{
					state=-1;
				}


			system("arecord -f dat -d 10 test.wav");

			repeat=0;
				while (repeat<=2)
				{

					system("amixer sset 'Right Playback Mixer Right DAC' mute");
					system("amixer sset 'Lineout' 100%");

					USB_printf("Playing Left Mic Recording\n", 1000);

					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CPlaying Left Mic Recording\n");
					write(fd_fb, buf, cnt_byte);

					system ("aplay -f dat test.wav");
					system("alsactl restore");

					USB_printf("@Confirm Left Mic Recording (Y/N):#", 1000);

					memset(buf, 0, 200);
					cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Confirm Left Mic Recording (Y/N):#\n");
					write(fd_fb, buf, cnt_byte);

					//ch=USB_getc(10000);
					get_line(ch, 1, USB_READ_TIMEOUT);
						if (ch[0]=='Y' || ch[0]=='y')
						{
							state_L=0;
							break;
						}
						else
						{
							state_L=-1;
						}
				USB_printf("\n", 1000);
				repeat++;
				}

				repeat=0;
					while (repeat<=2)
					{

						system("amixer sset 'Left Playback Mixer Left DAC' mute");
						system("amixer sset 'Lineout' 100%");
						system("amixer sset 'Lineout' 100%");

						USB_printf("Playing Right Mic Recording\n", 1000);

						memset(buf, 0, 200);
						cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CPlaying Right Mic Recording\n");
						write(fd_fb, buf, cnt_byte);

						system ("aplay -f dat test.wav");
						system("alsactl restore");

						USB_printf("@Confirm Right Mic Recording (Y/N):#", 1000);

						memset(buf, 0, 200);
						cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Confirm Right Mic Recording (Y/N):#\n");
						write(fd_fb, buf, cnt_byte);

						//ch=USB_getc(10000);
						get_line(ch, 1, USB_READ_TIMEOUT);
							if (ch[0]=='Y' || ch[0]=='y')
							{
								state_R=0;
								break;
							}
							else
							{
								state_R=-1;
							}
					USB_printf("\n", 1000);
					repeat++;
					}

					if (state_R==0 && state_L==0)
					{
						USB_printf("&Test 10B: OK\n", 1000);

						memset(buf, 0, 200);
						cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 10B: OK\n");
						write(fd_fb, buf, cnt_byte);
					}
					else
					{
						if (state_L==-1 && state_R==-1)
						{
							USB_printf("^Test 10B: Fail, Left and Right Mic Not Detected\n", 1000);

							memset(buf, 0, 200);
							cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 10B: Fail, Left and Right Mic Not Detected\n");
							write(fd_fb, buf, cnt_byte);
							return -1;
						}

						if (state_R==-1)
						{
							USB_printf("^Test 10B: Fail, Right Mic Not Detected\n", 1000);

							memset(buf, 0, 200);
							cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 10B: Fail, Right Mic Not Detected\n");
							write(fd_fb, buf, cnt_byte);
							return -1;
						}

						if (state_L==-1)
						{
							USB_printf("^Test 10B: Fail, Left Mic Not Detected\n", 1000);

							memset(buf, 0, 200);
							cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 10B: Fail, Left Mic Not Detected\n");
							write(fd_fb, buf, cnt_byte);
							return -1;
						}
					}
		}
		else
		{
			USB_printf("^Test 10A: Fail, Audio Codec Not Detected\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 10A: Fail, Audio Codec Not Detected\n");
			write(fd_fb, buf, cnt_byte);
		}

	}
	else
	{
		USB_printf("^Test 10A: Fail, Audio Codec Not Detected\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 10A: Fail, Audio Codec Not Detected\n");
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
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C**GPS Test**\n");
		write(fd_fb, buf, cnt_byte);

		if (Init_GPIO("63", "out")!=1)
		{
			printf("Error export pins 63\n");

			USB_printf("@Error export pins#\n^Test 9: Fail, error export pins\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error export pins#\n\x1b[2C^Test 9: Fail, error export pins\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}
		else
		{
			if (Write_GPIO("63", "1")!=1)
			{

				USB_printf("@Error write pins#\n^Test 9: Fail, error write pins\n", 1000);

				memset(buf, 0, 200);
				cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error write pins#\n\x1b[2C^Test 9: Fail, error write pins\n");
				write(fd_fb, buf, cnt_byte);

				return -1;
			}
		}
		sleep(1);

	fd=OpenPort("/dev/ttymxc2");
		if (fd<0)
		{
			printf("Error while open port\n");

			USB_printf("@Error while open port#\n^Test 9: Fail, error while open port\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error while open port#\n\x1b[2C^Test 9: Fail, error while open port\n");
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

				USB_printf("@Module received NMEA#\n&Test 9: OK\n", 1000);

				memset(buf, 0, 200);
				cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Module received NMEA#\n\x1b[2C&Test 9: OK\n");
				write(fd_fb, buf, cnt_byte);

				return 0;
			}
			else
			{
				close(fd);
				Write_GPIO("63", "0");
				DeInit_GPIO("63");

				USB_printf("@Error unexport pins#\n^Test 9: Fail, error unexport pins\n", 1000);

				memset(buf, 0, 200);
				cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error unexport pins#\n\x1b[2C^Test 9: Fail, error unexport pins\n");
				write(fd_fb, buf, cnt_byte);

				return -1;
			}
		}
		else
		{
			close(fd);
			Write_GPIO("63", "0");
			DeInit_GPIO("63");

			USB_printf("@Error unexport pins#\n^Test 9: Fail, error unexport pins\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error unexport pins#\n\x1b[2C^Test 9: Fail, error unexport pins\n");
			write(fd_fb, buf, cnt_byte);

			return -1;
		}

close(fd);
Write_GPIO("63", "0");
DeInit_GPIO("63");

USB_printf("@Error unexport pins#\n^Test 9: Fail, error unexport pins\n", 1000);

memset(buf, 0, 200);
cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Error unexport pins#\n\x1b[2C^Test 9: Fail, error unexport pins\n");
write(fd_fb, buf, cnt_byte);

return -1;
}

int Init_LARA_SARA(char* port_name, int port_speed) {

	int ret;
	int port_id;
	int size;
	char answr_buf[1000] = {0};
	char out_buf[1000] = {0};
	char curr_modem[10] = {0};

	char buf[200]={0};
	int cnt_byte;

	port_id=OpenPort(port_name);
	SetPort(port_id, port_speed);
	usleep(1000000);
	ClosePort(port_id);
	usleep(500000);

	///----***----***---***---***---***---***---***---***--
	port_id=OpenPort(port_name);
	printf( "Port ID: %i\n", port_id);
	SetPort(port_id, port_speed);

	if (!strcmp(port_name,"/dev/ttymxc1")) {
		sprintf(curr_modem, UART_MODEM_DESING);
	} else if (!strcmp(port_name,"/dev/ttyACM0")) {
		sprintf(curr_modem, USB_MODEM_DESING);
	}

	sleep(5);

	ret = sendWOPreParse(port_id, "ATE0\r", answr_buf, curr_modem);
	if (ret) goto error;

	usleep(250000);

	for(int i = 0; i < 3; i++){

		ret = sendWOPreParse(port_id, "AT\r", answr_buf, curr_modem);
		if (!ret) break;

		usleep(250000);

	}
	if (ret) goto error;

	//if (pFile!=NULL) fprintf(pFile, "Modem is Responsive\n");

	USB_printf("Modem is Responsive\n", 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CModem is Responsive\n");
	write(fd_fb, buf, cnt_byte);

	memset(answr_buf, 0, sizeof(answr_buf));
	ret = sendAndPreParse(port_id, "AT+CGMI\r", answr_buf, curr_modem);
	if (ret) goto error;

	size = answr_buf-strstr(answr_buf, "\n\n");
	sprintf(out_buf, "@Module Make/Model = ");
	strncat(out_buf, answr_buf, strstr(answr_buf, "\n\n") - answr_buf);

	strcat(out_buf, " ");

	memset(answr_buf, 0, sizeof(answr_buf));
	ret = sendAndPreParse(port_id, "AT+CGMM\r", answr_buf, curr_modem);
	if (ret) goto error;

	strncat(out_buf, answr_buf, strstr(answr_buf, "\n\n") - answr_buf);
	strcat(out_buf, "#\n");

	USB_printf(out_buf, 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C%s", out_buf);
	write(fd_fb, buf, cnt_byte);

	memset(answr_buf, 0, sizeof(answr_buf));
	ret = sendAndPreParse(port_id, "AT+CGSN\r", answr_buf, curr_modem);
	if (ret) goto error;

	memset(out_buf, 0, sizeof(out_buf));
	sprintf(out_buf, "@IMEI = ");
	strncat(out_buf, answr_buf, strstr(answr_buf, "\n\n") - answr_buf);
	strcat(out_buf, "#\n");

	USB_printf(out_buf, 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C%s", out_buf);
	write(fd_fb, buf, cnt_byte);

	for(int i = 0; i < 3; i++){

		ret = sendAndPreParse(port_id, "AT+CPIN?\r", answr_buf, curr_modem);
		if (!ret) break;

		sleep(2);

	}
	if (ret) {
		ret = -3; // ICCID read error;
		goto error;
	}

	memset(answr_buf, 0, sizeof(answr_buf));
	ret = sendAndPreParse(port_id, "AT+CCID\r", answr_buf, curr_modem);
	if (ret) {
		ret = -3; // ICCID read error;
		goto error;
	}

	memset(out_buf, 0, sizeof(out_buf));
	sprintf(out_buf, "@ICCID = ");
	strncat(out_buf, answr_buf+strlen("+CCID: "), strcspn(answr_buf+strlen("+CCID: "), "\n\n"));
	strcat(out_buf, "#\n");

	USB_printf(out_buf, 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C%s", out_buf);
	write(fd_fb, buf, cnt_byte);

	memset(answr_buf, 0, sizeof(answr_buf));
	ret = sendAndPreParse(port_id, "AT+CIMI\r", answr_buf, curr_modem);
	if (ret) {
		ret = -3; // IMSI read error;
		goto error;
	}

	memset(out_buf, 0, sizeof(out_buf));
	sprintf(out_buf, "@IMSI = ");
	strncat(out_buf, answr_buf, strcspn(answr_buf, "\n\n"));
	strcat(out_buf, "#\n");

	USB_printf(out_buf, 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C%s", out_buf);
	write(fd_fb, buf, cnt_byte);

	memset(answr_buf, 0, sizeof(answr_buf));
	ret = sendAndPreParse(port_id, "ATI9\r", answr_buf, curr_modem);
	if (ret) goto error;

	memset(out_buf, 0, sizeof(out_buf));
	sprintf(out_buf, "@FW Version = ");
	strncat(out_buf, answr_buf, strstr(answr_buf, "\n\n") - answr_buf);
	strcat(out_buf, "#\n");

	//if (pFile!=NULL) fputs (out_buf, pFile);

	USB_printf(out_buf, 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C%s", out_buf);
	write(fd_fb, buf, cnt_byte);


	if (!strcmp(curr_modem, UART_MODEM_DESING)) {
		ret = sendAndPreParse(port_id, "AT+CPWROFF\r", answr_buf, curr_modem);
		if (ret) goto error;
	}

	//if (pFile!=NULL) fprintf(pFile, "&Test %s: OK\n", curr_modem);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "&Test %s: OK\n", curr_modem);
	USB_printf(buf, 1000);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test %s: OK\n", curr_modem);
	write(fd_fb, buf, cnt_byte);

	ret = 0;

error:
	ClosePort(port_id);
	return ret;

}


int sendAndPreParse(int port_id, char* cmd_buf, char* ret_buf, char* type)
{

	char buf_tx[1000]={0};
	char buf_rx[1000]={0};
	int cnt_byte;

	char buf[200]={0};

	cnt_byte=sprintf(buf_tx, cmd_buf);
	puts (buf_tx);
	WritePort(port_id, (unsigned char*)buf_tx, cnt_byte);
	usleep(500000);
	cnt_byte=ReadPort(port_id, (unsigned char*)buf_rx, sizeof(buf_rx));
	puts(buf_rx);
	if (strstr(buf_rx, "OK") == NULL) // no OK returned
	{
		return -1;
	}
	if (!(strstr(buf_rx, "\n\n") == buf_rx)) // no header at the reply start
	{
		return -2;
	}
	memcpy(ret_buf, buf_rx + strlen("\n\n"), cnt_byte);
	return 0;
}

int sendWOPreParse(int port_id, char* cmd_buf, char* ret_buf, char* type)
{

	char buf_tx[1000]={0};
	char buf_rx[1000]={0};
	int cnt_byte;

	char buf[200]={0};

	cnt_byte=sprintf(buf_tx, cmd_buf);
	puts (buf_tx);
	WritePort(port_id, (unsigned char*)buf_tx, cnt_byte);
	usleep(500000);
	cnt_byte=ReadPort(port_id, (unsigned char*)buf_rx, sizeof(buf_rx));
	puts(buf_rx);
	if (strstr(buf_rx, "OK") == NULL) // no OK returned
	{
		return -1;
	}
	memcpy(ret_buf, buf_rx + strlen("\n\n"), cnt_byte);
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
