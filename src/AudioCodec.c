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

#include "AudioCodec.h"

#define DEBUG 1

#if (DEBUG > 0)
	#define DBG(x)	(x)
#else
	#define DBG(x)
#endif

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

