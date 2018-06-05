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

#include "i2c.h"
#include "TS.h"
#include "FrameBuffer.h"
#include "Display.h"
#include "eeprom.h"
#include "PreAsm.h"
#include "CellTest.h"

#define DEBUG 1

#if (DEBUG > 0)
#define DBG(x)	(x)
#else
#define DBG(x)
#endif

pthread_t fsm_ts_thread;	//thread for timer
int id_fsm_ts_thread = 1;

pthread_t test_sel_thread;	// thread for select test from USB port
int id_test_sel_thread = 2;

pthread_t cell_passthrough_thread;	// thread for select test from USB port
int id_cell_passthrough_thread = 3;

pthread_mutex_t mutex;

pthread_cond_t suspend_test_sel_cv;

int timer_tick = 0;
int fd_fb;

extern int preasm_flag;
int test_run_flag = 0;

int suspend_menu_flag = 0;

void* fsm_ts_thread_func(void* thread_data) {

	static ilitek_key_info key = { 0, 0, 0, 0 };

	for (;;) {
		key = Scan_TS_Key();
		FSM_TS(&key);
		timer_tick++;
		if (timer_tick >= 40000) {
			Write_EEPROM("2", 0);// write eeprom 2, after reboot android will be srart
			//pthread_exit(0);
			system("reboot");
		}
		usleep(30000);
	}
	return EXIT_SUCCESS;
}

void* test_sel_thread_func(void* thread_data) {
	char str[10] = { 0 };
	int state;

	for (;;) {
		pthread_mutex_lock(&mutex);
		state = (preasm_flag == 1 && suspend_menu_flag == 0);
		while (suspend_menu_flag) {
			pthread_cond_wait(&suspend_test_sel_cv, &mutex);
		}
		pthread_mutex_unlock(&mutex);
		if (state) {
			//printf("enter to thread\n");

			memset(str, 0, sizeof(str));
			USB_getline(str, 4, 500);

			if (isActiveFullTesetMenu()) {

				//printf("str = %s %i \n", str, str[0]);
				if (test_run_flag == 1)
					TestRun(str);
				if (test_run_flag == 2)
					TestRun_PostAsm(str);
				if (test_run_flag == 3)
					TestRun_Modems(str);
				if (test_run_flag == 4)
					MainTestRun(str);

			} //else {
				if (str[0] == '\n') {
					gotoParentMenu();
				}
			//}

		} else {
			usleep(100000);
		}
	}

	return EXIT_SUCCESS;
}

void* cell_passthrough_func(void* thread_data) {

	int state;

	pthread_mutex_lock(&mutex);
	state = suspend_menu_flag;
	pthread_mutex_unlock(&mutex);
	switch (state) {
	case 0:
		break;
	case 1:
		USB_Module_Passthrough(1);
		break;
	case 2:
		UART_Module_Passthrough(1);
		break;
	default:
		printf("Unexpected state!!!");
		break;
	}

	pthread_mutex_lock(&mutex);
	suspend_menu_flag = 0;
	pthread_cond_signal(&suspend_test_sel_cv);
	pthread_mutex_unlock(&mutex);

	return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
	//ilitek_key_info ts_key={0,0,0,0};

	Open_i2c("/dev/i2c-2", 0x41); //	open ILITEK TS
	fd_fb = open("/dev/tty0", O_RDWR); // open fb
	Fill_Buffer(0x00, 0x00, 0x00);
	MenuInit();

	printf("========== BIOS v1.2.0 ==========");

	int trhread_state = pthread_create(&fsm_ts_thread, NULL, fsm_ts_thread_func,
			&id_fsm_ts_thread);
	if (trhread_state == 0) {
		printf("\nFSM thread started\n");
	} else {
		perror("\nFSM thread fail\n");
	}

	trhread_state = pthread_create(&test_sel_thread, NULL, test_sel_thread_func,
			&id_test_sel_thread);
	if (trhread_state == 0) {
		printf("\nPreasm thread started\n");
	} else {
		perror("\nPreasm thread fail\n");
	}

	//pthread_cancel(fsm_ts_thread);
	pthread_join(fsm_ts_thread, NULL);
	pthread_join(test_sel_thread, NULL);

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
