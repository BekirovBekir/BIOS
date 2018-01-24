/*
 * FrameBuffer.c
 *
 *  Created on: 26 ���. 2017 �.
 *      Author: Bekir
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
#include "FrameBuffer.h"

#define DEBUG 1

#if (DEBUG > 0)
	#define DBG(x)	(x)
#else
	#define DBG(x)
#endif

struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;

struct v4l2_frmsizeenum  frminfo;
struct v4l2_fmtdesc fmtinfo;

unsigned long pixel_color(unsigned char r, unsigned char g, unsigned char b, struct fb_var_screeninfo *vinfo)
{
	return (r<<vinfo->red.offset) | (g<<vinfo->green.offset) | (b<<vinfo->blue.offset);
}

void Fill_Buffer(unsigned char r, unsigned char g, unsigned char b)
{
	int fb_fd = open("/dev/fb0",O_RDWR);

		//Get variable screen information
		ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
		vinfo.grayscale=0;
		vinfo.bits_per_pixel=32;
		ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);
		ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

		ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

		long screensize = vinfo.yres_virtual * finfo.line_length;

		uint8_t *fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);

		int x,y;

		for (y=0;y<vinfo.yres;y++)
		{
			for (x=0;x<vinfo.xres;x++)
			{
				long location =(x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) *finfo.line_length;
				*((uint32_t*)(fbp + location)) = pixel_color(r, g, b, &vinfo);
			//usleep(1000);
			}
		}
	close(fb_fd);
}

int Read_Cam_Param(char* path, CAMPARAM* param_ptr)
{
	int video_fd = open(path, O_RDONLY);
		if (video_fd<0) return -1;

		if ((ioctl(video_fd, VIDIOC_ENUM_FRAMESIZES, &frminfo))<0)
		{
			DBG(printf("\nFailed read CAM ioctl\n"));
			close(video_fd);
			return -1;
		}
		if (ioctl(video_fd, VIDIOC_ENUM_FMT, &fmtinfo)<0)
		{
			DBG(printf("Fail CAM2"));
			close(video_fd);
			return -1;
		}
	close(video_fd);
	sleep(1);
	memset(param_ptr->description, 0, 33);
	strncpy((char*)param_ptr->description, (char*)fmtinfo.description, 32);
	param_ptr->widht=frminfo.discrete.width;
	param_ptr->height=frminfo.discrete.height;
	param_ptr->pixel_format=frminfo.pixel_format;
	return 0;
}
