/*
 * eeprom.c
 *
 *  Created on: 3 янв. 2018 г.
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

#include "../inc/eeprom.h"

#define EEPROM_PATH "/sys/class/i2c-dev/i2c-1/device/1-0054/eeprom"

unsigned int Write_EEPROM(char* ptr_buf, unsigned int pos)
{
	int fd;
	char buf[50]={0};
	int cnt=0;

	snprintf(buf, sizeof(buf), EEPROM_PATH);
	fd=open(buf, O_WRONLY);
		if (fd<0)
		{
			//perror("\r\nError while opening eeprom");
			return -1;
		}
		cnt=strlen(ptr_buf);
		lseek(fd, pos, SEEK_SET);
		if (write(fd, ptr_buf, cnt)!=cnt)
		{
			perror("\r\nError while writing eeprom");
			return -1;
		}
	close(fd);
	return cnt;
}

unsigned int Read_EEPROM(char* ptr_buf, unsigned int pos, unsigned int cnt)
{
	int fd;
	char buf[50]={0};

	snprintf(buf, sizeof(buf), EEPROM_PATH);
	fd=open(buf, O_RDONLY);
		if (fd<0)
		{
			perror("\r\nError while opening eeprom");
			return -1;
		}
		if (lseek(fd, pos, SEEK_SET)!=-1)
		{
			if (read(fd, ptr_buf, cnt)!=cnt)
			{
				perror("\r\nError while writing eeprom");
				close(fd);
				return -1;
			}
		}
		else
		{
			close(fd);
			return -1;
		}
	close(fd);
	return cnt;
}
