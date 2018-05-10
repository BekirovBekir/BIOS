/*
 * AudioCodec.c
 *
 *  Created on: 1 февр. 2018 г.
 *      Author: bekir
 */

#include <linux/fb.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <linux/kd.h>

#include "AudioCodec.h"
#include "PreAsm.h"

extern int fd_fb;

#define USB_READ_TIMEOUT 30000

#define DEBUG 1

#if (DEBUG > 0)
	#define DBG(x)	(x)
#else
	#define DBG(x)
#endif

extern int get_line(char* str, int size, int timeout);

static struct v4l2_audio  audinfo;
static struct  v4l2_audioout audinfoout;


int Read_Audio_Param(char* path, AUDIOPARAM* param_ptr)
{
	int audio_fd = open(path, O_RDONLY);
		if (audio_fd<0) return -1;

		if ((ioctl(audio_fd, VIDIOC_G_AUDIO, &audinfo))<0)
		{
			DBG(printf("\nFailed read Audio ioctl\n"));
			close(audio_fd);
			return -1;
		}
		if (ioctl(audio_fd, VIDIOC_G_AUDOUT, &audinfoout)<0)
		{
			DBG(printf("\nFailed read Audio ioctl\n"));
			close(audio_fd);
			return -1;
		}
	close(audio_fd);
	sleep(1);
	memset(param_ptr->name, 0, 33);
	strncpy((char*)param_ptr->name, (char*)audinfo.name, 32);
	param_ptr->index=audinfo.index;
	param_ptr->mode=audinfo.mode;
	param_ptr->capability=audinfo.capability;
	return 0;
}

int Play_Sound(void)
{
	FILE * hiddenConsole;
	char Answer[1024];
	int lastchar;
	memset(Answer, 0, sizeof(Answer));

	char buf[200];
	int cnt_byte=0;
	char ch[10]={0};

	int state=-1;
	int repeat=0;

	system("alsactl restore");

	while (repeat<=2)
	{
		USB_printf("Playing Audio to Left Speaker\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CPlaying Audio to Left Speaker\n");
		write(fd_fb, buf, cnt_byte);

		system("speaker-test -t wav -c 2 -l 1 -s 1");

		USB_printf("@Confirm Sound on Left Speaker (Y/N):#", 1000);
		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Confirm Sound on Left Speaker (Y/N):#\n");
		write(fd_fb, buf, cnt_byte);

		//ch=USB_getc(10000);
		get_line(ch, 1, USB_READ_TIMEOUT);
			if (ch[0]=='Y' || ch[0]=='y')
			{
				state=0;
				break;
			}
			else
			{
				state=-1;
			}
	repeat++;
	}

	if (state==0)
	{
	repeat=0;
		while (repeat<=2)
		{
			USB_printf("Playing Audio to Right Speaker\n", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CPlaying Audio to Right Speaker\n");
			write(fd_fb, buf, cnt_byte);

			system("speaker-test -t wav -c 2 -l 1 -s 2");

			USB_printf("@Confirm Sound on Right Speaker (Y/N):#", 1000);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Confirm Sound on Right Speaker (Y/N):#\n");
			write(fd_fb, buf, cnt_byte);

			get_line(ch, 1, USB_READ_TIMEOUT);
				if (ch[0]=='Y' || ch[0]=='y')
				{
					state=0;
					break;
				}
				else
				{
					state=-1;
				}
		repeat++;
		}
	}

		if (state==0)
		{
		USB_printf("&Test 10A: OK\n", 1000);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 10A: OK\n");
		write(fd_fb, buf, cnt_byte);
		}

	return state;

	/*
	hiddenConsole = popen("speaker-test -t wav -c 2 -l 1 -s 1", "r");
	lastchar = fread(Answer, 1, 1024, hiddenConsole);
	pclose(hiddenConsole);
	Answer[lastchar] = '\0';

	printf("hiddenConsole answer: \n%s", Answer);

		if(lastchar == 0)
		{
			printf("\n");
			return -1;
		}

	char* ptr1=NULL;
	char* ptr2=NULL;


	ptr1=strstr(Answer, "Front Left");
	ptr2=strstr(Answer, "Front Right");
		if ((ptr1!=NULL) && (ptr2!=NULL))
		{
			return 0;
		}
		else
		{
			return -1;
		}
		*/
}

int Record_Sound(void)
{
	FILE * hiddenConsole;
	char Answer[1024];
	int lastchar;

	memset(Answer, 0, sizeof(Answer));

	system("alsactl restore");

	hiddenConsole = popen("arecord -f dat -d 10 test.wav", "r");
	lastchar = fread(Answer, 1, 1024, hiddenConsole);
	pclose(hiddenConsole);
	Answer[lastchar] = '\0';

	return 0;

	/*
	printf("hiddenConsole answer: \n%s", Answer);
		if(lastchar == 0)
		{
			printf("\n");
			return -1;
		}

	char* ptr1=NULL;

	ptr1=strstr(Answer, "Recording WAVE");
		if (ptr1!=NULL)
		{
			return 0;
		}
		else return -1;
	*/
}


