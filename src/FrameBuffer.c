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
#include "../inc/FrameBuffer.h"

struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;

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

	struct v4l2_frmsizeenum  frminfo;
	struct v4l2_fmtdesc fmtinfo;

	int fb_fd = open("/dev/video0",O_RDONLY);
		if (fb_fd<0) return -1;

	ioctl(fb_fd, VIDIOC_ENUM_FRAMESIZES, &frminfo);
	ioctl(fb_fd, VIDIOC_ENUM_FMT, &fmtinfo);

	memset(param_ptr->description, 0, 33);
	strncpy(param_ptr->description, fmtinfo.description, 32);
	param_ptr->widht=frminfo.discrete.width;
	param_ptr->height=frminfo.discrete.height;
	param_ptr->pixel_format=frminfo.pixel_format;

	close(fb_fd);
	return 0;
}
