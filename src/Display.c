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

Menu PreAsm;
Menu PostAsm;
Menu GI;
Menu ShipMode;
Menu Exit;

extern int fd_fb;

Menu* active_menu;

void PreAsmDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[22;60H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[20;60H>1. Pre-Assembly Testing\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[60C2. Post-Assembly Testing\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[60C3. System Recovery\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[60C4. Power Down to Shipping Mode\n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[60C5. Exit - Boot OS\n");
	write(fd_fb, buf, cnt_byte);
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
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[24;60H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[20;60H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[59C>\n\n");
	write(fd_fb, buf, cnt_byte);

}

void GIDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[26;60H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[22;60H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[59C>\n\n");
	write(fd_fb, buf, cnt_byte);
}

void GIAct (void)
{

}

void ShipModeDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[28;60H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[24;60H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[59C>\n\n");
	write(fd_fb, buf, cnt_byte);
}

void ShipModeAct (void)
{

}

void ExitDisp (void)
{
	char buf[50];
	char cnt_byte;

	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[26;60H \n\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 50);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[59C>\n\n");
	write(fd_fb, buf, cnt_byte);
}

void ExitAct (void)
{

}

void MenuInit (void)
{
	PreAsm.DOWN=&PostAsm;
	PreAsm.UP=&PreAsm;
	PreAsm.ENTER=NULL;
	PreAsm.menudisplay=&PreAsmDisp;
	PreAsm.menuaction=&PreAsmAct;

	PostAsm.DOWN=&GI;
	PostAsm.UP=&PreAsm;
	PostAsm.ENTER=NULL;
	PostAsm.menudisplay=&PostAsmDisp;
	PostAsm.menuaction=&PostAsmAct;

	GI.DOWN=&ShipMode;
	GI.UP=&PostAsm;
	GI.ENTER=NULL;
	GI.menudisplay=&GIDisp;
	GI.menuaction=&GIAct;

	ShipMode.DOWN=&Exit;
	ShipMode.UP=&GI;
	ShipMode.ENTER=NULL;
	ShipMode.menudisplay=&ShipModeDisp;
	ShipMode.menuaction=&ShipModeAct;

	Exit.DOWN=&Exit;
	Exit.UP=&ShipMode;
	Exit.ENTER=NULL;
	Exit.menudisplay=&ExitDisp;
	Exit.menuaction=&ExitAct;

	char buf[200];
	char cnt_byte;


	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J\x1b[31;46m");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2J");
	write(fd_fb, buf, cnt_byte);

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[17;40H***************************************************************\n");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[18;40H*\x1b[19;40H*\x1b[20;40H*\x1b[21;40H*\x1b[22;40H*\x1b[23;40H*\x1b[24;40H*\x1b[25;40H*\x1b[26;40H*\x1b[27;40H*\x1b[28;40H*\x1b[29;40H*\x1b[30;40H*");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[18;102H*\x1b[19;102H*\x1b[20;102H*\x1b[21;102H*\x1b[22;102H*\x1b[23;102H*\x1b[24;102H*\x1b[25;102H*\x1b[26;102H*\x1b[27;102H*\x1b[28;102H*\x1b[29;102H*\x1b[30;102H*");
	write(fd_fb, buf, cnt_byte);
	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[31;40H***************************************************************\n");
	write(fd_fb, buf, cnt_byte);
	active_menu=&PreAsm;
	active_menu->menudisplay();
}


