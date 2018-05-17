/*
 * PreAsm.h
 *
 *      Author: PerederiyS
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "CellTest.h"
#include "PreAsm.h"
#include "GPIO.h"
#include "ComPort.h"

#define BUFF_SIZE 100

extern int fd_fb;

#define USB_PATH "/dev/ttyGS0"
#define USB_PORT_NAME "/dev/ttyACM0"
#define UART_PORT_NAME "/dev/ttymxc1"
#define PORT_SPEED 115200

pthread_t test_passthrough;	// thread for select test from USB port
int id_test_passthrough = 3;

extern pthread_mutex_t mutex;

int USB_Module_Passthrought(int Do) {
	//Sending power on to device on USB port
	//Toggling GPIO signal for RF antenna selection for USB modem versus UART modem
	//Resume terminal task, route terminal to the USB device

	if (!Do)
		return -1;

	char buf[2000];
	char bufRx[2000];
	char bufTx[2000];
	int cntByte = 0;
	int portId;

	pthread_mutex_lock(&mutex);
	termState = true;
	pthread_mutex_unlock(&mutex);

	//setup GPIO
	printf("Setup GPIO for USB modem \n");

	if (Init_GPIO("49", "out") != 1) {

		USB_printf("@Error export pins#\n^Test 8A: Fail, error export pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error export pins#\n\x1b[2C^Test 8A: Fail, error export pins\n");
		write(fd_fb, buf, cntByte);

		return -1;
	}
	if (Init_GPIO("48", "out") != 1) {

		USB_printf("@Error export pins#\n^Test 8A: Fail, error export pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error export pins#\n\x1b[2C^Test 8A: Fail, error export pins\n");
		write(fd_fb, buf, cntByte);

		return -1;
	}

	//----------------------Power test modem via USB------------------------------------------
	usleep(1000000);

	if (Write_GPIO("48", "1") != 1) {

		USB_printf("@Error write pins#\n^Test 8A: Fail, error write pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error write pins#\n\x1b[2C^Test 8A: Fail, error write pins\n");
		write(fd_fb, buf, cntByte);

		return -1;
	}

	usleep(1000000);

	if (Write_GPIO("49", "1") != 1) {

		USB_printf("@Error write pins#\n^Test 8A: Fail, error write pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error write pins#\n\x1b[2C^Test 8A: Fail, error write pins\n");
		write(fd_fb, buf, cntByte);

		return -1;
	}

	usleep(12000000);

	portId = OpenPort(USB_PORT_NAME);
	SetPort(portId, PORT_SPEED);
	usleep(1000000);
	ClosePort(portId);
	usleep(500000);

	///----***----***---***---***---***---***---***---***--
	portId = OpenPort(USB_PORT_NAME);
	printf("Port ID: %i\n", portId);
	SetPort(portId, PORT_SPEED);

	USB_printf("**USB Modem Command Passthrough ON**\n", 1000);

	memset(buf, 0, 200);
	cntByte = snprintf(buf, sizeof(buf),
			"\x1b[2C**USB Modem Command Passthrough ON**\n");
	write(fd_fb, buf, cntByte);

	get_line(bufTx, sizeof(bufTx), 100);

	do {
		pthread_mutex_lock(&mutex);
		if (!termState) {
			pthread_mutex_unlock(&mutex);
			break;
		}
		pthread_mutex_unlock(&mutex);
		memset(bufRx, 0, sizeof(bufRx));
		memset(bufTx, 0, sizeof(bufTx));

		if (get_line_wo_clean(bufTx, sizeof(bufTx), 100) != 0)
			continue;
		snprintf(buf, strlen(bufTx), "%s", bufTx);
		strcat(buf, "\r");
		WritePort(portId, (unsigned char*) buf, strlen(bufTx) + 1);
		usleep(200000);
		ReadPort(portId, (unsigned char*) bufRx, sizeof(bufRx));
		USB_printf(bufRx, sizeof(bufRx));

	} while (true);

	ClosePort(portId);

	USB_printf("**USB Modem Command Passthrough OFF**\n", 1000);

	memset(buf, 0, 200);
	cntByte = snprintf(buf, sizeof(buf),
			"\x1b[2C**USB Modem Command Passthrough OFF**\n");
	write(fd_fb, buf, cntByte);

	if (Write_GPIO("49", "0") != 1) {

		USB_printf("@Error write pins#\n^Test 8A: Fail, error write pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error write pins#\n\x1b[2C^Test 8A: Fail, error write pins\n");
		write(fd_fb, buf, cntByte);

		return -1;
	}

	printf("Unexport GPIO for USB modem \n");

	if (DeInit_GPIO("48") != 1) {

		USB_printf(
				"@Error unexport pins#\n^Test 8A: Fail, error unexport pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error unexport pins#\n\x1b[2C^Test 8A: Fail, error unexport pins\n");
		write(fd_fb, buf, cntByte);

		return -1;
	}
	if (DeInit_GPIO("49") != 1) {

		USB_printf(
				"@Error unexport pins#\n^Test 8A: Fail, error unexport pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error unexport pins#\n\x1b[2C^Test 8A: Fail, error unexport pins\n");
		write(fd_fb, buf, cntByte);

		return -1;
	}

	return 0;
}

int UART_Module_Passthrought(int Do) {

	//Sending power on to device on USB port
	//Toggling GPIO signal for RF antenna selection for USB modem versus UART modem
	//Resume terminal task, route terminal to the USB device

	if (!Do)
		return -1;

	char buf[2000];
	char bufRx[2000];
	char bufTx[2000];
	int cntByte = 0;
	int portId;

	pthread_mutex_lock(&mutex);
	termState = true;
	pthread_mutex_unlock(&mutex);

	//setup GPIO
	printf("Setup GPIO for UART modem \n");

	if (Init_GPIO("50", "out") != 1) // init UART modem power enable
			{

		USB_printf("@Error export pins#\n^Test 8B: Fail, error export pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error export pins#\n\x1b[2C^Test 8B: Fail, error export pins\n");
		write(fd_fb, buf, cntByte);

		return -1;
	}
	if (Init_GPIO("48", "out") != 1) // init modem antenna/SIM selector
			{

		USB_printf("@Error export pins#\n^Test 8B: Fail, error export pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error export pins#\n\x1b[2C^Test 8B: Fail, error export pins\n");
		write(fd_fb, buf, cntByte);

		return -1;
	}
	if (Init_GPIO("52", "out") != 1) // init modem ON pin
			{

		USB_printf("@Error export pins#\n^Test 8B: Fail, error export pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error export pins#\n\x1b[2C^Test 8B: Fail, error export pins\n");
		write(fd_fb, buf, cntByte);

		return -1;
	}

//----------------------Power test modem via UART-------------------------------------------
	printf("UART modem power-on\n");

	if (Write_GPIO("48", "0") != 1) {

		USB_printf("@Error write pins#\n^Test 8B: Fail, error write pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error write pins#\n\x1b[2C^Test 8B: Fail, error write pins\n");
		write(fd_fb, buf, cntByte);

		return -1;
	}

	if (Write_GPIO("50", "1") != 1) {

		USB_printf("@Error write pins#\n^Test 8B: Fail, error write pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error write pins#\n\x1b[2C^Test 8B: Fail, error write pins\n");
		write(fd_fb, buf, cntByte);

		return -1;

	}

	usleep(200000);

	if (Write_GPIO("52", "1") != 1) {

		USB_printf("@Error write pins#\n^Test 8B: Fail, error write pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error write pins#\n\x1b[2C^Test 8B: Fail, error write pins\n");
		write(fd_fb, buf, cntByte);

		return -1;

	}

	usleep(500000);

	if (Write_GPIO("52", "0") != 1) {

		USB_printf("@Error write pins#\n^Test 8B: Fail, error write pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error write pins#\n\x1b[2C^Test 8B: Fail, error write pins\n");
		write(fd_fb, buf, cntByte);

		return -1;
	}

	usleep(1000000);

	portId = OpenPort(UART_PORT_NAME);
	SetPort(portId, PORT_SPEED);
	usleep(1000000);
	ClosePort(portId);
	usleep(500000);

	///----***----***---***---***---***---***---***---***--
	portId = OpenPort(UART_PORT_NAME);
	printf("Port ID: %i\n", portId);
	SetPort(portId, PORT_SPEED);

	USB_printf("**UART Modem Command Passthrough ON**\n", 1000);

	memset(buf, 0, 200);
	cntByte = snprintf(buf, sizeof(buf),
			"\x1b[2C**UART Modem Command Passthrough ON**\n");
	write(fd_fb, buf, cntByte);

	get_line(bufTx, sizeof(bufTx), 100);

	do {
		pthread_mutex_lock(&mutex);
		if (!termState) {
			pthread_mutex_unlock(&mutex);
			break;
		}
		pthread_mutex_unlock(&mutex);
		memset(bufRx, 0, sizeof(bufRx));
		memset(bufTx, 0, sizeof(bufTx));

		if (get_line_wo_clean(bufTx, sizeof(bufTx), 100) != 0)
			continue;
		snprintf(buf, strlen(bufTx), "%s", bufTx);
		strcat(buf, "\r");
		WritePort(portId, (unsigned char*) buf, strlen(bufTx) + 1);
		usleep(200000);
		ReadPort(portId, (unsigned char*) bufRx, sizeof(bufRx));
		USB_printf(bufRx, sizeof(bufRx));

	} while (true);

	ClosePort(portId);

	USB_printf("**UART Modem Command Passthrough OFF**\n", 1000);

	memset(buf, 0, 200);
	cntByte = snprintf(buf, sizeof(buf),
			"\x1b[2C**UART Modem Command Passthrough OFF**\n");
	write(fd_fb, buf, cntByte);

	if (Write_GPIO("50", "0") != 1) {

		USB_printf("@Error write pins#\n^Test 8B: Fail, error write pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error write pins#\n\x1b[2C^Test 8B: Fail, error write pins\n");
		write(fd_fb, buf, cntByte);

		return -1;

	}

	printf("Unexport GPIO for UART \n");

	if (DeInit_GPIO("50") != 1) {

		USB_printf(
				"@Error unexport pins#\n^Test 8B: Fail, error unexport pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error unexport pins#\n\x1b[2C^Test 8B: Fail, error unexport pins\n");
		write(fd_fb, buf, cntByte);

		return -1;
	}

	if (DeInit_GPIO("48") != 1) {

		USB_printf(
				"@Error unexport pins#\n^Test 8B: Fail, error unexport pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error unexport pins#\n\x1b[2C^Test 8B: Fail, error unexport pins\n");
		write(fd_fb, buf, cntByte);

		return -1;
	}

	if (DeInit_GPIO("52") != 1) {

		USB_printf(
				"@Error unexport pins#\n^Test 8B: Fail, error unexport pins\n",
				1000);

		memset(buf, 0, 200);
		cntByte =
				snprintf(buf, sizeof(buf),
						"\x1b[2C@Error unexport pins#\n\x1b[2C^Test 8B: Fail, error unexport pins\n");
		write(fd_fb, buf, cntByte);

		return -1;
	}

	return 0;
}
