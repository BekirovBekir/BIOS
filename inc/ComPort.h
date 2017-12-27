/*
 * ComPort.h
 *
 *  Created on: 20 июн. 2017 г.
 *      Author: alex
 */

#ifndef COMPORT_H_
#define COMPORT_H_

int OpenPort(char* portname);
int ClosePort(int fd);
int WritePort(int fd, unsigned char* buf, unsigned int buflen);
int ReadPort(int fd, unsigned char* buf, unsigned int buflen);

int SetPort(int fd, int speed);

int PrintConfigPort(int fd);

#endif /* COMPORT_H_ */
