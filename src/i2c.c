/*
 * i2c.c


 *
 *  Created on: 27 дек. 2017 г.
 *      Author: bekir
 */

#include "../inc/i2c.h"

static unsigned char dev_adr;
static int file_desc;

void Close_i2c(void)
{
    close(file_desc);
}

void Open_i2c(char* i2c_bus, unsigned char addr)
{
    file_desc = open(i2c_bus, O_RDWR);
    if(file_desc < 0)
    {
		perror("\nError open I2C");
		exit(-1);
    }

    if(ioctl(file_desc, I2C_SLAVE, addr) < 0)
    {
		perror("\nError set I2C");
		Close_i2c();
		exit(-1);
    }
    dev_adr=addr;
}

int Write_i2c(char addr, unsigned char* buf, int len)
{
     struct i2c_rdwr_ioctl_data msg_rdwr;
     struct i2c_msg i2cmsg;
     //char buf_local[1];
     char* buf_local = (char*) malloc(len);
     int i;

     buf_local[0] = addr;
     if (buf!=NULL)
     {
     	 for (i=1; i<len; i++)
     	 {
     		buf_local[i]=buf[i];
     	 }
     }

     msg_rdwr.msgs = &i2cmsg;
     msg_rdwr.nmsgs = 1;
     i2cmsg.addr  = dev_adr;
     i2cmsg.flags = 0;
     i2cmsg.len   = len;
     i2cmsg.buf   = buf_local;

		if(ioctl(file_desc,I2C_RDWR,&msg_rdwr)<0)
		{
			perror("\nError write to I2C");
			return -1;
		}
	free(buf_local);

    return 0;

}

int Read_i2c(char addr, unsigned char* buf, int len)
{
    struct i2c_rdwr_ioctl_data msg_rdwr;
    struct i2c_msg i2cmsg;

		if(Write_i2c(addr,NULL,1)<0)
		{
			perror("\nError write to I2C");
			return -1;
		}

    msg_rdwr.msgs = &i2cmsg;
    msg_rdwr.nmsgs = 1;

    i2cmsg.addr  = dev_adr;
    i2cmsg.flags = I2C_M_RD;
    i2cmsg.len   = len;
    i2cmsg.buf   = buf;

		if(ioctl(file_desc,I2C_RDWR,&msg_rdwr)<0)
		{
			perror("\nError read to I2C");
			return -1;
		}


    return 0;
}
