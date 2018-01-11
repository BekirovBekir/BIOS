
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

#include "./inc/i2c.h"
#include "./inc/TS.h"
#include "./inc/FrameBuffer.h"
#include "./inc/Display.h"
#include "./inc/eeprom.h"

#define DEBUG 1

#if (DEBUG > 0)
	#define DBG(x)	(x)
#else
	#define DBG(x)
#endif

pthread_t fsm_ts_thread;	//thread for timer
int id_fsm_ts_thread=1;

pthread_t preasm_thread;	//preasm test thread
int id_preasm_thread=1;

int timer_tick=0;
int fd_fb;

void* fsm_ts_thread_func(void* thread_data)
{
static ilitek_key_info key={0,0,0,0};
	for (;;)
	{
		key=Scan_TS_Key();
		FSM_TS(&key);
		timer_tick++;
			if (timer_tick>=5500)
			{
				Write_EEPROM("2");	// write eeprom 2, after reboot android will be srart
				pthread_exit(0);
			}
		usleep(30000);
	}
}

int main(int argc, char* argv[])
{
	//ilitek_key_info ts_key={0,0,0,0};

	Open_i2c("/dev/i2c-2", 0x41); //	open ILITEK TS
	fd_fb = open("/dev/tty0", O_RDWR); // open fb
	Fill_Buffer(0x00, 0x00, 0x00);
	MenuInit();

	int trhread_state=pthread_create(&fsm_ts_thread, NULL, fsm_ts_thread_func, &id_fsm_ts_thread);
		if (trhread_state==0)
		{
			printf ("\nFSM thread started\n");
		}
		else
		{
			perror("\nFSM thread fail\n");
		}


	//pthread_cancel(preasm_thread);
	//pthread_join(preasm_thread, NULL);
		//while (getchar()!='q');



	//pthread_cancel(fsm_ts_thread);
	pthread_join(fsm_ts_thread, NULL);

	/*while (1)
	{
	ts_key=Scan_TS_Key();
	usleep(300000);
	}*/
	Close_i2c(); //	close ILITEK TS
	close(fd_fb); // close fb
	system("reboot");
	return EXIT_SUCCESS;

}
