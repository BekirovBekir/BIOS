/*
 * Button.c

 *
 *  Created on: 17 авг. 2017 г.
 *      Author: bekir
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <error.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>

#include "../inc/GPIO.h"


int Init_GPIO(const char* pin_name,  char* pin_mode)
{
	int fd;
	int cnt_byte;
	char buf[GPIO_BUF_SIZE]={0};

	//memset(buf, 0, BUT_BUF_SIZE);

	fd=open("/sys/class/gpio/export", O_WRONLY);
		if (fd<0)
		{
			snprintf(buf, sizeof(buf), "\r\nError while opening GPIO-%s", pin_name);
			perror(buf);
			return -1;
		}
	memset(buf, 0, GPIO_BUF_SIZE);
	cnt_byte=snprintf(buf, sizeof(buf), "%s", pin_name);
	write(fd, buf, cnt_byte);
	close(fd);

	memset(buf, 0, GPIO_BUF_SIZE);
	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%s/direction", pin_name);
	fd=open(buf, O_WRONLY);
		if (fd<0)
		{
			snprintf(buf, sizeof(buf), "\r\nError while set GPIO-%s direction", pin_name);
			perror(buf);
			return -1;
		}
	memset(buf, 0, GPIO_BUF_SIZE);
	cnt_byte=snprintf(buf, sizeof(buf), "%s", pin_mode);
		if(write(fd, buf, cnt_byte)!=cnt_byte)
		{
			perror("Error, not writing\n");
			return -1;
		}
	close(fd);
	return 1;
}

int DeInit_GPIO(const char* pin_name)
{
	int fd;
	int cnt_byte;
	char buf[GPIO_BUF_SIZE]={0};

	//memset(buf, 0, BUT_BUF_SIZE);

	fd=open("/sys/class/gpio/unexport", O_WRONLY);
		if (fd<0)
		{
			snprintf(buf, sizeof(buf), "\r\nError while opening GPIO-%s", pin_name);
			perror(buf);
			return -1;
		}
	memset(buf, 0, GPIO_BUF_SIZE);
	cnt_byte=snprintf(buf, sizeof(buf), "%s", pin_name);
	write(fd, buf, cnt_byte);
	close(fd);

	return 1;
}

int Read_GPIO(const char* pin_name)
{
	int fd;
	char state_pin;
	char buf[GPIO_BUF_SIZE]={0};

	state_pin=0;
	//memset(buf, 0, BUT_BUF_SIZE);
	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%s/value", pin_name);
	fd=open(buf, O_RDONLY);
		if (fd<0)
		{
			perror("\r\nError while opening GPIO value");
			return -1;
		}
		if (read(fd, &state_pin, sizeof(state_pin))!=1)
		{
			perror("\r\nError while reading GPIO");
			return -1;
		}
		close(fd);
		if (state_pin=='1') return 1;
		if (state_pin=='0') return 0;
		else return -1; // fail
}

int Write_GPIO(char* pin_name, char* pin_state)
{
	int fd;
	char buf[GPIO_BUF_SIZE]={0};

	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%s/value", pin_name);
	fd=open(buf, O_WRONLY);
		if (fd<0)
		{
			snprintf(buf, sizeof(buf), "\r\nError while opening GPIO-%s", pin_name);
			perror(buf);
			return -1;
		}
	memset(buf, 0, GPIO_BUF_SIZE);
		if (write(fd, pin_state, 1)!=1)
		{
			perror("\r\nError while writing GPIO");
			return -1;
		}
	close(fd);
	return 1;
}

int Set_PWM_LED (int pwm)
{
	int fd;
	char buf[GPIO_BUF_SIZE]={0};
	int cnt_byte;

	fd=open("/sys/class/backlight/backlight/brightness", O_WRONLY); // must be change
	if (fd==-1)
	{
		perror("\r\nError while opening PWM for LED");
		return -1;
	}
	cnt_byte=snprintf(buf, sizeof(buf), "%i", pwm);
	if (write(fd, buf, cnt_byte)!=cnt_byte)
	{
		perror("\r\nError while writing PWM for display");
		return -1;
	}
	close(fd);
	return 1;


}

int Set_I_Charge (int level)
{
	int fd;
	char buf[GPIO_BUF_SIZE]={0};
	int cnt_byte;
	long buf_I;

	system("chmod a+rw /sys/class/power_supply/ltc4015-node-charger/input_current_limit");
	usleep(200000);
	system("chmod +rw /sys/class/power_supply/ltc4015-node-charger/constant_charge_current");
	usleep(200000);
	//system("i2cset -y -f 2 0x68 0x29 0x00 w");

	fd=open("/sys/class/power_supply/ltc4015-node-charger/input_current_limit", O_WRONLY); // must be change
	if (fd==-1)
	{
		perror("\r\nError while opening Charge module");
		return -1;
	}
	if (write(fd, "2000000", 7)!=7)
	{
		perror("\r\nError while writing charge current");
		return -1;
	}
	close(fd);

	fd=open("/sys/class/power_supply/ltc4015-node-charger/constant_charge_current", O_WRONLY); // must be change
	if (fd==-1)
	{
		perror("\r\nError while opening Charge module");
		return -1;
	}

	memset(buf, 0, GPIO_BUF_SIZE);
	if ((level>=1)&&(level<=20))
	{
	buf_I=100000*level;
		cnt_byte=snprintf(buf, sizeof(buf), "%ld", buf_I);
		if (write(fd, buf, cnt_byte)!=cnt_byte)
		{
			perror("\r\nError while writing charge current");
			return -1;
		}
	}
	else
	{
		perror("\r\nCurrent level is fail");
		return -1;
	}
	close(fd);
	return 1;


}

int Display (int color)
{
	int fd;
	char buf[GPIO_BUF_SIZE]={0};
	int cnt_byte;

	fd=open("/dev/fb0", O_WRONLY); // must be change
	if (fd==-1)
	{
		perror("\r\nError while opening PWM for LED");
		return -1;
	}
	cnt_byte=snprintf(buf, sizeof(buf), "%i", color);
	if (write(fd, buf, cnt_byte)!=cnt_byte)
	{
		perror("\r\nError while writing PWM for display");
		return -1;
	}
	close(fd);
	return 1;

}






