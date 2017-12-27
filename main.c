
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

#include  "./inc/i2c.h"
#include  "./inc/TS.h"



int main(int argc, char* argv[])
{
	ilitek_key_info ts_key={0,0,0,0};
	Open_i2c("/dev/i2c-2", 0x41); //setup

	    /*unsigned char newbuf[31];

	    if(Read_i2c(0x10, newbuf, 31)>=0)
	    {
	    	printf("ILITEK: %i, %i, %i, %i, %i, %i, %i\n", newbuf[0], newbuf[1], newbuf[2], newbuf[3], newbuf[4], newbuf[5], newbuf[6]);
	    }
	    else
	    {
	    	printf("Fail\n");
	    }*/

	while (1)
	{
	ts_key=Scan_TS_Key();
	usleep(300000);
	}
	Close_i2c(); //cleanup
	return EXIT_SUCCESS;

}
