/*
 * ComPort.c
 *
 *  Created on: 20 июн. 2017 г.
 *      Author: alex
 */

#include "ComPort.h"
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#define DEBUG 0

#if (DEBUG > 0)
	#define DBG(x)	(x)
#else
	#define DBG(x)
#endif



int OpenPort(char* portname)
	{
	int fd;

	DBG(printf("Opening port: %s ... ", portname));
	fd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0)
	  {
		DBG(printf("Error\n"));
	  }
	else
	  {
	  fcntl(fd, F_SETFL, 0);
	  DBG(printf("OK\n"));
	  }

	return fd;
	}



int ClosePort(int fd)
	{
	int ret;

	DBG(printf("Close port ..."));
	ret = close(fd);
	DBG(printf("OK\n"));
	return ret;
	}



int WritePort(int fd, unsigned char* buf, unsigned int buflen)
	{
	int n;

#if(DEBUG > 0)
	printf("Output buffer:\n");
	for(int i = 0; i < buflen; i++)
		{
		printf("0x%02X ", buf[i]);
		};
	printf("\n");
#endif

	n = write(fd, buf, buflen);

	if(n != buflen)
		{
		printf("Error while writing\n");
		}
	return n;
	}


int ReadPort(int fd, unsigned char* buf, unsigned int buflen)
	{
	int cnt_byte;
	cnt_byte=read(fd, buf, buflen);
	return cnt_byte;
	}



int SetPort(int fd, int speed)
	{
	struct termios options;
	int spd;

	tcgetattr(fd, &options);

	DBG(printf("Set speed%d\n",speed));

	switch(speed)
		{
		case 115200:
			spd = B115200;
			break;

		case 9600:
			spd = B9600;
			break;

		default:
			spd = B9600;
			break;
		}

	//cfsetispeed(&options, spd);
	//cfsetospeed(&options, spd);
	cfsetspeed(&options, spd);


	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	options.c_cflag &= ~CRTSCTS;

	options.c_cflag |= (CLOCAL | CREAD);

	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST;

	options.c_cc[VMIN]=0;	 // settings for timeout
	options.c_cc[VTIME]=10;

	tcsetattr(fd, TCSANOW, &options);

	return 0;
	}

int PrintConfigPort(int fd)
	{
	struct termios options;

	DBG(printf("Getting attributes:\n"));
	tcgetattr(fd, &options);

	printf("iflag : 0x%02X\n", options.c_iflag);
	printf("oflag : 0x%02X\n", options.c_oflag);
	printf("cflag : 0x%02X\n", options.c_cflag);
	printf("lflag : 0x%02X\n", options.c_lflag);
	printf("cline : 0x%02X\n", options.c_line);
	printf("ispeed : 0x%02X\n", options.c_ispeed);
	printf("ospeed : 0x%02X\n", options.c_ospeed);
	printf("\n\n");

	return 0;
	}

