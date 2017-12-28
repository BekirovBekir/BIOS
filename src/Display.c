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

Menu PreAsm;
Menu PostAsm;
Menu GI;
Menu ShipMode;
Menu Exit;

extern int fd;

Menu* active_menu;

void PreAsmDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[21;60H \n");
	write(fd, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[20;60H*1. Pre-Assembly Testing\n");
	write(fd, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[60C2. Post-Assembly Testing\n");
	write(fd, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[60C3. System Recovery\n");
	write(fd, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[60C4. Power Down to Shipping Mode\n");
	write(fd, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[60C5. Exit - Boot OS\n");
	write(fd, buf, cnt_byte);
}

void PreAsmAct (void)
{

}

void PostAsmAct (void)
{

}

void PostAsmDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[20;60H \n");
	write(fd, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[59C*\n");
	write(fd, buf, cnt_byte);

}

void MenuInit (void)
{
	PreAsm.DOWN=&PostAsm;
	PreAsm.UP=&PreAsm;
	PreAsm.ENTER=NULL;
	PreAsm.menudisplay=&PreAsmDisp;
	PreAsm.menuaction=&PreAsmAct;

	PostAsm.DOWN=&PostAsm;
	PostAsm.UP=&PreAsm;
	PostAsm.ENTER=NULL;
	PostAsm.menudisplay=&PostAsmDisp;
	PostAsm.menuaction=&PostAsmAct;

	char buf[10];
	char cnt_byte;

	memset(buf, 0, 10);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J");
	write(fd, buf, cnt_byte);
	active_menu=&PreAsm;
	active_menu->menudisplay();
}


