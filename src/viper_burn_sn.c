#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <stdarg.h>
#include <time.h>
#include <stdio_ext.h>
#include <termios.h>
#include <fcntl.h>

#define SERIAL_NUMBER_SIZE 8
#define SERIAL_NUMBER_OFFSET 2
#define EEPROM_PATH "/sys/class/i2c-dev/i2c-1/device/1-0054/eeprom"
#define ATTEMPTS 3
#define USB_CON_PATH "/dev/ttyGS0"
#define EEPROM_SIZE 512
#define USB_READ_TIMEOUT 30000

#define MAX_SIZE_SN 100

static unsigned char sernum[SERIAL_NUMBER_SIZE];
static char sernum_str[MAX_SIZE_SN * 2];	//SERIAL_NUMBER_SIZE * 2
//-----------------------------------------------------------------------
//static struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };
static struct pollfd usbpoll = { STDIN_FILENO, POLLIN|POLLPRI };

extern int fd_fb;

static int read_SN_from_eeprom(unsigned char* serial_number);
static int write_SN_to_eeprom(unsigned char* serial_number);
static int validate_input_string(char* str);
static int validate_SN_in_eeprom(unsigned char* bufferA, unsigned char* bufferB);
static int convert_SN(unsigned char* sn_str, unsigned char* sn);

static int to_USB_console(unsigned char* fmt, ...);

static int eeprom_integrity_check(void);
extern int get_line(char* str, int size, int timeout);
//-----------------------------------------------------------------------

int EEPROM_SN_Read(void)
{
	unsigned char sn_valid = 0;
	unsigned char str_valid = 0;
	unsigned char i;
	unsigned char attempts = 0;
	unsigned char prompt_attempts;
	char prompt;

	char buf[200];
	int cnt_byte=0;


	if (eeprom_integrity_check() !=0 )
		return -1;


			printf("\n**Device Serial Number Burn-In**\n");
	to_USB_console("\n**Device Serial Number Burn-In**\n");

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\n\x1b[2C**Device Serial Number Burn-In**\n");
	write(fd_fb, buf, cnt_byte);


			printf("@Device Serial Number: ");
	to_USB_console("@Device Serial Number: ");

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Device Serial Number: ");
	write(fd_fb, buf, cnt_byte);

	if( read_SN_from_eeprom(sernum) == 0 )
	{
		for(i=0; i<SERIAL_NUMBER_SIZE; i++)
		{
					printf("%02x", sernum[i]);
			to_USB_console("%02x", sernum[i]);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "%02x", sernum[i]);
			write(fd_fb, buf, cnt_byte);
		}
				printf("\n");
		to_USB_console("#\n");

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "#\n");
		write(fd_fb, buf, cnt_byte);


		printf("&Test 1B: OK\n");
		to_USB_console("&Test 1B: OK\n");

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\n\x1b[2C&Test 1B: OK\n");
		write(fd_fb, buf, cnt_byte);

	}
	else
	{
		printf("Not Set#\n");
		to_USB_console("Not Set#\n");

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CNot Set#\n");
		write(fd_fb, buf, cnt_byte);

		printf("\n@Format is Incorrect or no value entered#\n");
		to_USB_console("\n@Format is Incorrect or no value entered#\n");

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\n");
		write(fd_fb, buf, cnt_byte);

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Format is Incorrect or no value entered#\n");
		write(fd_fb, buf, cnt_byte);

		printf("\n^Test 1B: FAIL, Format is Incorrect or no value entered\n");
		to_USB_console("\n^Test 1B: FAIL, Format is Incorrect or no value entered\n");

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 1B: FAIL, Format is Incorrect or no value entered\n");
		write(fd_fb, buf, cnt_byte);

		return -1;
	}
}

int EEPROM_SN(void)
{
	unsigned char sn_valid = 0;
	unsigned char str_valid = 0;
	unsigned char i;
	unsigned char attempts = 0;
	unsigned char prompt_attempts;
	char prompt;
	//char tmp_str[100];
	//char *line = NULL;
	//int readed;
	//FILE* usb_con_in;

	char buf[200];
	int cnt_byte=0;


	if (eeprom_integrity_check() !=0 )
		return -1;


			printf("\n**Device Serial Number Burn-In**\n");
	to_USB_console("\n**Device Serial Number Burn-In**\n");

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\n\x1b[2C**Device Serial Number Burn-In**\n");
	write(fd_fb, buf, cnt_byte);


start:

			printf("@Device Serial Number: ");
	to_USB_console("@Device Serial Number: ");

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Device Serial Number: ");
	write(fd_fb, buf, cnt_byte);

	if( read_SN_from_eeprom(sernum) == 0 )
	{
		for(i=0; i<SERIAL_NUMBER_SIZE; i++)
		{
					printf("%02x", sernum[i]);
			to_USB_console("%02x", sernum[i]);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "%02x", sernum[i]);
			write(fd_fb, buf, cnt_byte);
		}
				printf("\n");
		to_USB_console("#\n");

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "#\n");
		write(fd_fb, buf, cnt_byte);

		sn_valid = 1;
	}
	else
	{
				printf("Not Set#\n");
		to_USB_console("Not Set#\n");

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CNot Set#\n");
		write(fd_fb, buf, cnt_byte);

		sn_valid = 0;
	}


edit:

	if(sn_valid == 0)
	{
		attempts = 0;

		do
		{
			str_valid = 0;

			if(attempts != 0)
			{
						printf("\n@Format is Incorrect or no value entered, please retry#\n");
				to_USB_console("\n@Format is Incorrect or no value entered, please retry#\n");

				memset(buf, 0, 200);
				cnt_byte=snprintf(buf, sizeof(buf), "\n");
				write(fd_fb, buf, cnt_byte);

				memset(buf, 0, 200);
				cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C@Format is Incorrect or no value entered, please retry#\n");
				write(fd_fb, buf, cnt_byte);
			}

					printf("\nEnter Device Serial Number (16 character Hex value):");
			to_USB_console("\nEnter Device Serial Number (16 character Hex value):");

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\n");
			write(fd_fb, buf, cnt_byte);

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CEnter Device Serial Number (16 character Hex value):");
			write(fd_fb, buf, cnt_byte);

			memset(sernum_str, 0, MAX_SIZE_SN * 2);	//SERIAL_NUMBER_SIZE*2

			if( get_line(sernum_str, MAX_SIZE_SN*2, USB_READ_TIMEOUT) != 0)	//SERIAL_NUMBER_SIZE*2
			{
						printf("\n");
				to_USB_console("#\n");

				memset(buf, 0, 200);
				cnt_byte=snprintf(buf, sizeof(buf), "#\n");
				write(fd_fb, buf, cnt_byte);
			}


			attempts++;
			str_valid = validate_input_string(sernum_str);

		} while ( (str_valid != 0) && attempts < (ATTEMPTS));


		if(str_valid != 0)
		{
					printf("\n^Test 1B: FAIL, User Failed to Proper Enter Value\n");
			to_USB_console("\n^Test 1B: FAIL, User Failed to Proper Enter Value\n");

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\n\x1b[2C^Test 1B: FAIL, User Failed to Proper Enter Value\n");
			write(fd_fb, buf, cnt_byte);
			return -1;
		}
		//printf("New SN is valid. Writing into EEPROM...");

		convert_SN(sernum_str, sernum);

		if (write_SN_to_eeprom(sernum) == 0)
		{
			//printf("OK\n");
			goto start;
		}
		else
		{
					printf("^Test 1B: FAIL, Failed to write due to EEPROM ERROR\n");
			to_USB_console("^Test 1B: FAIL, Failed to write due to EEPROM ERROR\n");

			memset(buf, 0, 200);
			cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C^Test 1B: FAIL, Failed to write due to EEPROM ERROR\n");
			write(fd_fb, buf, cnt_byte);
		}
	}
	else
	{
				printf("Re-enter Serial Number? (Y/N):");
		to_USB_console("Re-enter Serial Number? (Y/N):");

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CRe-enter Serial Number? (Y/N):");
		write(fd_fb, buf, cnt_byte);

		prompt_attempts = 0;
				while (1)
				{
					//scanf("%s", &prompt);
					get_line(&prompt, 1, USB_READ_TIMEOUT);

					printf("typed %c\n", prompt);

					if(prompt == 'y' || prompt == 'Y')
					{
						prompt = 0;
						sn_valid = 0;
						goto edit;
					}
					else if(prompt == 'n' || prompt == 'N')
					{
						break;
					}
					else
					{
						if(prompt_attempts >= ATTEMPTS-1)
						{
									printf("\n");
							to_USB_console("\n");
							break;
						}

						prompt_attempts++;

								printf("\nRe-enter Serial Number? (Y/N):");
						to_USB_console("\nRe-enter Serial Number? (Y/N):");
					}

				}

	}

			printf("&Test 1B: OK\n");
	to_USB_console("&Test 1B: OK\n");

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\n\x1b[2C&Test 1B: OK\n");
	write(fd_fb, buf, cnt_byte);

	return EXIT_SUCCESS;
};

int read_SN_from_eeprom(unsigned char* serial_number)
{
	FILE* f;
	int ret = 0;
	unsigned char count;
	unsigned char bufferA[SERIAL_NUMBER_SIZE];
	unsigned char bufferB[SERIAL_NUMBER_SIZE];

	f = fopen(EEPROM_PATH, "rb");
	if(f == NULL)
	{
		//printf("Can't open EEPROM for read\n");
		ret = -1;
		goto close;
	}

	fseek(f, SERIAL_NUMBER_OFFSET, SEEK_SET);

	count = fread(&bufferA, 1, SERIAL_NUMBER_SIZE, f);
	if(count != SERIAL_NUMBER_SIZE)
	{
		//printf("Can't read EEPROM to bufferA\n");
		ret = -1;
		goto close;
	}

	count = fread(&bufferB, 1, SERIAL_NUMBER_SIZE, f);
	if(count != SERIAL_NUMBER_SIZE)
	{
		//printf("Can't read EEPROM to bufferB\n");
		ret = -1;
		goto close;
	}

	if( validate_SN_in_eeprom(&bufferA, &bufferB) != 0)
	{
		ret = -1;
		goto close;
	}

	unsigned char i;
	//strncpy(serial_number, bufferA, SERIAL_NUMBER_SIZE);
	for(i=0; i< SERIAL_NUMBER_SIZE; i++)
		{
			serial_number[i] = bufferA[i];
		}

close:
	if(f) fclose(f);

	return ret;
}

int write_SN_to_eeprom(unsigned char* serial_number)
{
	FILE* f;
	int ret = 0;
	int count;

	unsigned char i;

	unsigned char bufferA[SERIAL_NUMBER_SIZE];
	unsigned char bufferB[SERIAL_NUMBER_SIZE];

	memcpy(bufferA, serial_number, SERIAL_NUMBER_SIZE);
	for(i=0; i<SERIAL_NUMBER_SIZE; i++)
	{
		bufferB[i] = (255 - bufferA[i]);
	}

	f = fopen(EEPROM_PATH, "ab");
	if(f == NULL)
	{
		//printf("Can't open EEPROM to write\n");
		ret = -1;
		goto closeW;
	}

	fseek(f, SERIAL_NUMBER_OFFSET, SEEK_SET);

	count = fwrite(&bufferA, 1, SERIAL_NUMBER_SIZE, f);
	if(count != SERIAL_NUMBER_SIZE)
	{
		//printf("Can't write bufferA to EEPROM\n");
		ret = -1;
		goto closeW;
	}

	count = fwrite(&bufferB, 1, SERIAL_NUMBER_SIZE, f);
	if(count != SERIAL_NUMBER_SIZE)
	{
		//printf("Can't write bufferB to EEPROM\n");
		ret = -1;
		goto closeW;
	}

closeW:
	if(f) fclose(f);

	return ret;
}

int validate_input_string(char* str)
{
	unsigned char i;
	int err = 0;

	int cnt=strlen(str)-1;
	if(cnt<=(SERIAL_NUMBER_SIZE*2))
	{
		for(i=0; i< (SERIAL_NUMBER_SIZE*2); i++)
		{
			if( (str[i] >= '0' && str[i] <= '9') ||
				(str[i] >= 'a' && str[i] <= 'f') ||
				(str[i] >= 'A' && str[i] <= 'F'))
			{
				continue;
			}
			err++;
		}
	}
	else err++;
	return err;
}

int convert_SN(unsigned char* sn_str, unsigned char* sn)
{
	unsigned char i;
	unsigned char str[3];
	unsigned char value;

	str[2] = 0;

	for(i=0; i<SERIAL_NUMBER_SIZE*2; i=i+2)
	{
		str[0] = sn_str[i];
		str[1] = sn_str[i+1];
		value = (unsigned char)strtoul(str, NULL, 16);
		sn[(unsigned char)(i/2)] = value;
	}
	return 0;
}

int validate_SN_in_eeprom(unsigned char* bufferA, unsigned char* bufferB)
{
	int ret = 0;
	unsigned char i;

	for(i=0; i<SERIAL_NUMBER_SIZE; i++)
	{
		if((255 - bufferA[i] != bufferB[i]))
			return -1;
	}
	return ret;
}

int to_USB_console(unsigned char* fmt, ...)
{
	struct pollfd usbpoll = { STDIN_FILENO, POLLOUT };
	char buf[200];
	va_list argptr;
	int ret = -1;

	int fd;

	memset((void*)buf, 0 , sizeof(unsigned char) * 200);

	va_start(argptr, fmt);
	vsprintf(buf, fmt, argptr);

	fd = open(USB_CON_PATH, O_WRONLY);
	if(fd > 0)
	{
		usbpoll.fd = fd;
		if( poll(&usbpoll, 1, 100) )
		{
			write(fd, buf, 200);
			ret = 0;
		}
	}

	if(fd > 0)
		close(fd);

	return ret;
}

int eeprom_integrity_check(void)
{
	FILE* f;
	int ret = 0;
	int count = 0;
	int i = 0;
	unsigned char backup_buffer[EEPROM_SIZE];
	unsigned char bufferA[EEPROM_SIZE];
	unsigned char bufferB[EEPROM_SIZE];

	char buf[200];
	int cnt_byte=0;

			printf("\n**EEPROM Integrity Check**\n");
	to_USB_console("\n**EEPROM Integrity Check**\n");

	memset(buf, 0, 200);
	//cnt_byte=snprintf(buf, sizeof(buf), "\x1b[1;3H**EEPROM Integrity Check**\n");
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C**EEPROM Integrity Check**\n");
	write(fd_fb, buf, cnt_byte);

			printf("&Test 1A: ");
	to_USB_console("&Test 1A: ");

	memset(buf, 0, 200);
	cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2C&Test 1A: ");
	write(fd_fb, buf, cnt_byte);

	f = fopen(EEPROM_PATH, "r+");
	if(f == NULL)
	{
		ret = -1;
		goto close;
	}

	fseek(f, 0, SEEK_SET);
	count = fread(backup_buffer, 1, EEPROM_SIZE, f);
	if(count != EEPROM_SIZE)
	{
		ret = -1;
		goto close;
	}

	memset(bufferA, 0, EEPROM_SIZE);
	memset(bufferB, 0, EEPROM_SIZE);

	srand(time(NULL));
	for(i=0; i<EEPROM_SIZE; i++)
	{
		bufferA[i] = rand() % 255;
	}


	fseek(f, 0, SEEK_SET);
	count = fwrite(bufferA, 1, EEPROM_SIZE, f);
	if(count != EEPROM_SIZE)
	{
		ret = -1;
		goto restore;
	}

	fflush(f);
	sleep(1);


	fseek(f, 0, SEEK_SET);
	count = fread(bufferB, 1, EEPROM_SIZE, f);
	if(count != EEPROM_SIZE)
	{
		ret = -1;
		goto restore;
	}


	for(i=0; i<EEPROM_SIZE; i++)
	{
		if(bufferA[i] != bufferB[i])
		{
			ret = -1;
			goto restore;
		}
	}


restore:

	fseek(f, 0, SEEK_SET);
	count = fwrite(backup_buffer, 1, EEPROM_SIZE, f);
	if(count != EEPROM_SIZE)
	{
		ret = -1;
	}

close:

	if(f) fclose(f);


	if(ret != 0)
	{
				printf("Fail, EEPROM Corrupted or Inaccessible\n");
		to_USB_console("Fail, EEPROM Corrupted or Inaccessible\n");

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2CFail, EEPROM Corrupted or Inaccessible\n");
		write(fd_fb, buf, cnt_byte);
	}
	else
	{
				printf("OK\n");
		to_USB_console("OK\n");

		memset(buf, 0, 200);
		cnt_byte=snprintf(buf, sizeof(buf), "\x1b[2COK\n");
		write(fd_fb, buf, cnt_byte);
	}
	return ret;
}


int get_line(char* str, int size, int timeout)
{
	struct pollfd usbpoll = { STDIN_FILENO, POLLIN|POLLPRI };
	FILE *usbcon;
	char *line;
	ssize_t read;
	int ret = 0;
	int len;

	char* fgetsret;

	len = size+1;
	line = (void *)malloc(len);
	memset(line, 0, len);



	usbcon = fopen(USB_CON_PATH, "r");
	if (usbcon == NULL)
	{
		ret = -1;
		printf("Error not opened\n");
		goto close_l;
	}


	tcflush(fileno(usbcon), TCIOFLUSH);

	usbpoll.fd = fileno(usbcon);
	usbpoll.events = POLLIN | POLLPRI;

	if( poll(&usbpoll, 1, timeout) )
	{
		fgetsret = fgets(line, len, usbcon);
	}
	else
	{
		//Timeout
		printf("Error timeout\n");
		ret = -1;
	}

	if(ret == 0 )
	{
		if(fgetsret != NULL)
			strncpy(str, line, size);
		else
			ret = -1;
		printf("Enter - %s\n", str);
	}

	close_l:
	if(usbcon)
		fclose(usbcon);
	if(line)
		free(line);

	return ret;
}
