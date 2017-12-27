/*
 * i2c.h
 *
 *  Created on: 27 дек. 2017 г.
 *      Author: bekir
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

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

void Close_i2c(void);
void Open_i2c(char* i2c_bus, unsigned char addr);
int Write_i2c(char addr, unsigned char* buf, int len);
int Read_i2c(char addr, unsigned char* buf, int len);

#endif /* INC_I2C_H_ */
