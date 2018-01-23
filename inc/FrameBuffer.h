/*
 * FrameBuffer.h
 *
 *  Created on: 26 ���. 2017 �.
 *      Author: Bekir
 */

#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

unsigned long pixel_color(unsigned char r, unsigned char g, unsigned char b, struct fb_var_screeninfo *vinfo);

void Fill_Buffer(unsigned char r, unsigned char g, unsigned char b);

typedef struct
{
	char description[33];
	unsigned int height;
	unsigned int widht;
	unsigned int pixel_format;
}CAMPARAM;

int Read_Cam_Param(char* path, CAMPARAM* param_ptr);

#endif /* FRAMEBUFFER_H_ */
