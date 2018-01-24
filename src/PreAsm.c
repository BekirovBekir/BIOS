/*
 * PreAsm.c
 *
 *  Created on: 9 янв. 2018 г.
 *      Author: bekir
 */


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


#include "PreAsm.h"
#include "eeprom.h"
#include "i2c.h"
#include "ComPort.h"
#include "GPIO.h"
#include "FrameBuffer.h"


int CX=0;
int CY=0;
int CZ=0;
char LightDataBuffer[100]={0};
float temperature = 0;
float pressure = 0;
char EmmyWiFiBuffer[1024]={0};
char EmmyBTBuffer[1024]={0};
char SaraBuffer[1024]={0};
char LaraBuffer[1024]={0};
unsigned char wifi_first_start_flag=0;

int TestMMC(int Do)
{
	//Performs checksum validation on all memory parts to ensure memory is free of corruption or defect. This includes testing
	#define BUFFER_SIZE 100

	char buffer_out[BUFFER_SIZE];
	char buffer_in[BUFFER_SIZE];

	char buffer_save[BUFFER_SIZE];
	int fd;
	int i;

	if(!Do) return -1;

	memset(buffer_in, 0, BUFFER_SIZE);
	memset(buffer_out, 0, BUFFER_SIZE);
	memset(buffer_save, 0, BUFFER_SIZE);

	printf("Generate output buffer...\n");
	srand(time(NULL));
	for(i=0; i<BUFFER_SIZE; i++)
	{
		buffer_out[i] = rand() % 255;
	}

	for(i=0; i<BUFFER_SIZE; i++)
		{
		if(i==10) printf("\n");
		if(i==20) printf("\n");
		if(i==30) printf("\n");
		if(i==40) printf("\n");
		if(i==50) printf("\n");
		if(i==60) printf("\n");
		if(i==70) printf("\n");
		if(i==80) printf("\n");
		if(i==90) printf("\n");
		printf("0x%02X ",buffer_out[i]);
		}
	printf("\nok");
	printf("\n\n");



	printf("Open eMMC...");
	fd = open("/dev/mmcblk1", O_RDWR);
	if(fd < 0 )
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");

	i=read(fd, buffer_save, 100);
		if(i != 100)
		{
			printf("Read only %d bytes instead %d", i, 100);
		}

	printf("Close eMMC...");
	i = close(fd);
		if(i != 0 )
		{
			printf("Error: %d\n",i);
			return -1;
		}
	printf("ok\n\n");

	printf("Open eMMC...");
	fd = open("/dev/mmcblk1", O_RDWR);
	if(fd < 0 )
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");

	printf("Write output buffer to eMMC...");

	i = write(fd, buffer_out, BUFFER_SIZE);
	if(i != BUFFER_SIZE)
	{
		printf("Write only %d bytes instead %d", i, BUFFER_SIZE);
	}
	printf("ok\n");


	printf("Close eMMC...");
	i = close(fd);
	if(i != 0 )
	{
		printf("Error: %d\n",i);
		return -1;
	}
	printf("ok\n\n");

//----------------------------------------------

	fd = 0;

	printf("Open eMMC...");
	fd = open("/dev/mmcblk1", O_RDWR);
	if(fd < 0)
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");


	printf("Read input buffer from eMMC...");
	i = read(fd, buffer_in, BUFFER_SIZE);
	if(i != BUFFER_SIZE)
	{
		printf("Read only %d bytes instead %d", i, BUFFER_SIZE);
	}
	printf("ok\n\n");


	printf("Input buffer:\n");
	for(i=0; i<BUFFER_SIZE; i++)
		{
		if(i==10) printf("\n");
		if(i==20) printf("\n");
		if(i==30) printf("\n");
		if(i==40) printf("\n");
		if(i==50) printf("\n");
		if(i==60) printf("\n");
		if(i==70) printf("\n");
		if(i==80) printf("\n");
		if(i==90) printf("\n");
		printf("0x%02X ",buffer_in[i]);
		}
	printf("\n\n");

	printf("Close eMMC...");

	printf("Close eMMC...");
	i = close(fd);
	if(i != 0 )
	{
		printf("Error: %d\n",i);
		return -1;
	}
	printf("ok\n\n");

	printf("Open eMMC...");
	fd = open("/dev/mmcblk1", O_RDWR);
	if(fd < 0)
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");

	i = write(fd, buffer_save, BUFFER_SIZE);
		if(i != BUFFER_SIZE)
		{
			printf("Write only %d bytes instead %d", i, 100);
		}

	close(fd);
	printf("ok\n\n");

//------------------------------------------------

	for(i=0; i<BUFFER_SIZE; i++ )
	{
		if(buffer_in[i] != buffer_out[i])
		{
			printf("WARNING: output and input buffer are not equal\n");
			return -1;
		}
	}

	printf("Output and input buffer are equal\n\n");

	return 0;

}

int FuncSPI_32MBit_NOR_Flash(int Do)
{
	//Performs checksum validation on all memory parts to ensure memory is free of corruption or defect. This includes testing
	char buffer_out[BUFFER_SIZE];
	char buffer_in[BUFFER_SIZE];
	char buffer_save[BUFFER_SIZE];
	int fd;
	int i;

	if(!Do) return -1;

	memset(buffer_in, 0, BUFFER_SIZE);
	memset(buffer_out, 0, BUFFER_SIZE);
	memset(buffer_save, 0, BUFFER_SIZE);

	printf("Open SPI NOR...");
	fd = open("/dev/mtdblock0", O_RDWR);
	if(fd < 0 )
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");

	i=read(fd, buffer_save, 100);
		if(i != 100)
		{
			printf("Read only %d bytes instead %d", i, 100);
		}

	printf("Close SPI NOR...");
	i = close(fd);
		if(i != 0 )
		{
			printf("Error: %d\n",i);
			return -1;
		}
	printf("ok\n\n");

	printf("Generate output buffer...\n");
	srand(time(NULL));
	for(i=0; i<BUFFER_SIZE; i++)
	{
		buffer_out[i] = rand() % 255;
	}

	for(i=0; i<BUFFER_SIZE; i++)
	{
		if(i==10) printf("\n");
		if(i==20) printf("\n");
		if(i==30) printf("\n");
		if(i==40) printf("\n");
		if(i==50) printf("\n");
		if(i==60) printf("\n");
		if(i==70) printf("\n");
		if(i==80) printf("\n");
		if(i==90) printf("\n");
		printf("0x%02X ",buffer_out[i]);
	}
	printf("\nok");
	printf("\n\n");



	printf("Open SPI NOR...");
	fd = open("/dev/mtdblock0", O_WRONLY);
	if(fd < 0 )
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");



	printf("Write output buffer to SPI NOR...");
	i = write(fd, buffer_out, BUFFER_SIZE);
	if(i != BUFFER_SIZE)
	{
		printf("Write only %d bytes instead %d", i, BUFFER_SIZE);
	}
	printf("ok\n");


	printf("Close SPI NOR...");
	i = close(fd);
	if(i != 0 )
	{
		printf("Error: %d\n",i);
		return -1;
	}
	printf("ok\n\n");

//----------------------------------------------

	fd = 0;

	printf("Open SPI NOR...");
	fd = open("/dev/mtdblock0", O_RDONLY);
	if(fd < 0)
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");


	printf("Read input buffer from SPI NOR...");
	i = read(fd, buffer_in, BUFFER_SIZE);
	if(i != BUFFER_SIZE)
	{
		printf("Read only %d bytes instead %d", i, BUFFER_SIZE);
	}
	printf("ok\n\n");


	printf("Input buffer:\n");
	for(i=0; i<BUFFER_SIZE; i++)
		{
		if(i==10) printf("\n");
		if(i==20) printf("\n");
		if(i==30) printf("\n");
		if(i==40) printf("\n");
		if(i==50) printf("\n");
		if(i==60) printf("\n");
		if(i==70) printf("\n");
		if(i==80) printf("\n");
		if(i==90) printf("\n");
		printf("0x%02X ",buffer_in[i]);
		}
	printf("\n\n");

	printf("Close SPI NOR...");
	close(fd);
	printf("ok\n\n");

	printf("Open SPI NOR...");
	fd = open("/dev/mtdblock0", O_RDWR);
	if(fd < 0)
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");

	i = write(fd, buffer_save, BUFFER_SIZE);
		if(i != BUFFER_SIZE)
		{
			printf("Write only %d bytes instead %d", i, 100);
		}

	close(fd);
	printf("ok\n\n");

//------------------------------------------------

	for(i=0; i<BUFFER_SIZE; i++ )
	{
		if(buffer_in[i] != buffer_out[i])
		{
			printf("WARNING: output and input buffer are not equal\n");
			return -1;
		}
	}

	printf("Output and input buffer are equal\n\n");
	return 0;
};

int FuncEEPROM(int Do)
{
	//Performs checksum validation on all memory parts to ensure memory is free of corruption or defect. This includes testing
	#define BUFFER_SIZE 100

	char buffer_out[BUFFER_SIZE];
	char buffer_in[BUFFER_SIZE];
	int fd;
	int i;

	if(!Do) return -1;

	memset(buffer_in, 0, BUFFER_SIZE);
	memset(buffer_out, 0, BUFFER_SIZE);

	//system("clear");
	//printf("\n*** EEPROM Test Application ***\n\n");

	printf("\nGenerate output buffer...\n");
	srand(time(NULL));
	for(i=0; i<BUFFER_SIZE; i++)
	{
		buffer_out[i] = rand() % 255;
	}

	for(i=0; i<BUFFER_SIZE; i++)
		{
		if(i==10) printf("\n");
		if(i==20) printf("\n");
		if(i==30) printf("\n");
		if(i==40) printf("\n");
		if(i==50) printf("\n");
		if(i==60) printf("\n");
		if(i==70) printf("\n");
		if(i==80) printf("\n");
		if(i==90) printf("\n");
		printf("0x%02X ",buffer_out[i]);
		}
	printf("\nok");
	printf("\n\n");



	printf("Open EEPROM...");
	fd = open("/sys/class/i2c-dev/i2c-1/device/1-0054/eeprom", O_WRONLY);
	if(fd < 0 )
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");


	printf("Write output buffer to EEPROM...");
	i = write(fd, buffer_out, BUFFER_SIZE);
	if(i != BUFFER_SIZE)
	{
		printf("Write only %d bytes instead %d", i, BUFFER_SIZE);
	}
	printf("ok\n");


	printf("Close EEPROM...");
	i = close(fd);
	if(i != 0 )
	{
		printf("Error: %d\n",i);
		return -1;
	}
	printf("ok\n\n");

//----------------------------------------------

	fd = 0;

	printf("Open EEPROM...");
	fd = open("/sys/class/i2c-dev/i2c-1/device/1-0054/eeprom", O_RDONLY);
	if(fd < 0)
	{
		printf("Error: %d\n", fd);
		return -1;
	}
	printf("ok\n");


	printf("Read input buffer from EEPROM...");
	i = read(fd, buffer_in, BUFFER_SIZE);
	if(i != BUFFER_SIZE)
	{
		printf("Read only %d bytes instead %d", i, BUFFER_SIZE);
	}
	printf("ok\n\n");


	printf("Input buffer:\n");
	for(i=0; i<BUFFER_SIZE; i++)
		{
		if(i==10) printf("\n");
		if(i==20) printf("\n");
		if(i==30) printf("\n");
		if(i==40) printf("\n");
		if(i==50) printf("\n");
		if(i==60) printf("\n");
		if(i==70) printf("\n");
		if(i==80) printf("\n");
		if(i==90) printf("\n");
		printf("0x%02X ",buffer_in[i]);
		}
	printf("\n\n");

	printf("Close EEPROM...");
	close(fd);
	printf("ok\n\n");

//------------------------------------------------

	for(i=0; i<BUFFER_SIZE; i++ )
	{
		if(buffer_in[i] != buffer_out[i])
		{
			printf("WARNING: output and input buffer are not equal\n");
			return -1;
		}
	}

	printf("Output and input buffer are equal\n\n");

	Write_EEPROM("2");
	return 0;
};

int FuncAccelerometer_Calibration(int Do)
{
	//Accelerometer X/Y/Z offet trim needs to be calculate (while board is in a stable flat position). Offset values need to stored in a protected EMMC sector and available for the accelerometer to load on start-up
	#define FiltrTime 32
	#define BUFF_SIZE 100
	#define SN_SIZE 100

	const char *AccelS_Read[3] = {"/sys/bus/iio/devices/iio:device0/in_accel_scale","/sys/bus/iio/devices/iio:device1/in_accel_scale","/sys/bus/iio/devices/iio:device2/in_accel_scale"};
	const char *AccelX_Read[3] = {"/sys/bus/iio/devices/iio:device0/in_accel_x_raw","/sys/bus/iio/devices/iio:device1/in_accel_x_raw","/sys/bus/iio/devices/iio:device2/in_accel_x_raw"};
	const char *AccelY_Read[3] = {"/sys/bus/iio/devices/iio:device0/in_accel_y_raw","/sys/bus/iio/devices/iio:device1/in_accel_y_raw","/sys/bus/iio/devices/iio:device2/in_accel_y_raw"};
	const char *AccelZ_Read[3] = {"/sys/bus/iio/devices/iio:device0/in_accel_z_raw","/sys/bus/iio/devices/iio:device1/in_accel_z_raw","/sys/bus/iio/devices/iio:device2/in_accel_z_raw"};

	const char *CalibX_Read[3] = {"/sys/bus/iio/devices/iio:device0/in_accel_x_calibbias","/sys/bus/iio/devices/iio:device1/in_accel_x_calibbias","/sys/bus/iio/devices/iio:device2/in_accel_x_calibbias"};
	const char *CalibY_Read[3] = {"/sys/bus/iio/devices/iio:device0/in_accel_y_calibbias","/sys/bus/iio/devices/iio:device1/in_accel_y_calibbias","/sys/bus/iio/devices/iio:device2/in_accel_y_calibbias"};
	const char *CalibZ_Read[3] = {"/sys/bus/iio/devices/iio:device0/in_accel_z_calibbias","/sys/bus/iio/devices/iio:device1/in_accel_z_calibbias","/sys/bus/iio/devices/iio:device2/in_accel_z_calibbias"};
	const char *CalibAccel_File_Path = {"CalibAccel"};

	char dataBuffer[BUFF_SIZE];

	int fdS, fdX,fdY,fdZ, fdC, indexFile = 0;
	int ValueX=0,ValueY=0,ValueZ=0;//ValueS=0,
	int CalibX=0,CalibY=0,CalibZ=0, i=0;

	int FounFileFlag = 0, AlreadyCalibrFlag=0;
	//char userAnsw = ' ', ch=' ';
	FILE *fp;


	if(!Do) return -1;


	fp = fopen( CalibAccel_File_Path, "r" );//fdC = open( CalibAccel_File_Path, O_RDONLY );
	if(fp == NULL){
		printf("Start calibrate\n\n");

	}
	else{
		fscanf (fp, "%i%i%i", &CalibX, &CalibY, &CalibZ);
		CX=CalibX; CY=CalibY; CZ=CalibZ;
		printf("Device already calibrate! Calibrate values: x = %i; y = %i; z = %i\n", CalibX, CalibY, CalibZ);
		/*printf("Press 'y' to calibrate or 'n' to exit: ");
		do{
			ch = getchar();
		}while(ch !='y'&& ch !='n');

		if(ch == 'n'){
			return 0;
		}*/
		AlreadyCalibrFlag = 1;
		fclose(fp);
		return 0;
	}


	for(int i=0; i<=2; i++){
		printf("Open: %s\n", AccelS_Read[i]);
		fdS = open( AccelS_Read[i], O_RDONLY );
		if(fdS == -1){
			printf("%s - not found\n", AccelS_Read[i]);
		}
		else{
			FounFileFlag=1;
			indexFile= i;
			break;
		}
	}

	if(FounFileFlag==0)
	{
		printf( "ERROR: Unable to open device file! \n" );
		return -1;
	}
	printf("Open: %s  - OK\n" ,AccelS_Read[indexFile]);
	FounFileFlag = 0;

	//*******//
	printf("Open: %s\n", AccelX_Read[indexFile]);
	fdX = open( AccelX_Read[indexFile], O_RDONLY );
	if(fdX == -1)
	{
		printf( "ERROR: Unable to open device file! \n" );
		return -1;
	}
	printf("Open: %s  - OK\n" ,AccelX_Read[indexFile]);

	//*******//
	printf("Open: %s\n", AccelY_Read[indexFile]);
	fdY = open( AccelY_Read[indexFile], O_RDONLY );
	if(fdY == -1)
	{
		printf( "ERROR: Unable to open device file! \n" );
		return -1;
	}
	printf("Open: %s  - OK\n" ,AccelY_Read[indexFile]);

	//*******//
	printf("Open: %s\n", AccelZ_Read[indexFile]);
	fdZ = open( AccelZ_Read[indexFile], O_RDONLY );
	if(fdZ == -1)
	{
		printf( "ERROR: Unable to open device file! \n" );
		return -1;
	}
	printf("Open: %s  - OK\n" ,AccelZ_Read[indexFile]);

	if(AlreadyCalibrFlag){
		//Clear calibrate
		memset(dataBuffer, 0, sizeof( dataBuffer ));
		sprintf(dataBuffer, "%i",0);
		fdC = open(CalibX_Read[indexFile], O_WRONLY);
		if(fdC == -1)
		{
			printf( "ERROR: Unable to open device file! \n" );
			return -1;
		}
		write(fdC, dataBuffer, sizeof(dataBuffer));
		close(fdC);

		memset(dataBuffer, 0, sizeof( dataBuffer ));
		sprintf(dataBuffer, "%i",0);
		fdC = open(CalibY_Read[indexFile], O_WRONLY);
		if(fdC == -1)
		{
			printf( "ERROR: Unable to open device file! \n" );
			return -1;
		}
		write(fdC, dataBuffer, sizeof(dataBuffer));
		close(fdC);

		memset(dataBuffer, 0, sizeof( dataBuffer ));
		sprintf(dataBuffer, "%i",0);
		fdC = open(CalibZ_Read[indexFile], O_WRONLY);
		if(fdC == -1)
		{
			printf( "ERROR: Unable to open device file! \n" );
			return -1;
		}
		write(fdC, dataBuffer, sizeof(dataBuffer));
		close(fdC);
	}
	AlreadyCalibrFlag = 0;


	//do{

		memset(dataBuffer, 0, sizeof( dataBuffer ));
		fdS = open(AccelS_Read[indexFile], O_RDONLY);
		read(fdS, dataBuffer, sizeof(dataBuffer));
		close(fdS);
		printf("\n------------------------\n");
		printf("Scale = %s \n", dataBuffer );

		ValueX = 0;
		ValueY = 0;
		ValueZ = 0;

		for(int i=0; i<FiltrTime; i++){
			//X
			memset(dataBuffer, 0, sizeof( dataBuffer ));
			fdX = open(AccelX_Read[indexFile], O_RDONLY);
			read(fdX, dataBuffer, sizeof(dataBuffer));
			close(fdX);
			ValueX +=  atoi(dataBuffer);
			//Y
			memset(dataBuffer, 0, sizeof( dataBuffer ));
			fdY = open(AccelY_Read[indexFile], O_RDONLY);
			read(fdY, dataBuffer, sizeof(dataBuffer));
			close(fdY);
			ValueY +=  atoi(dataBuffer);
			//Z
			memset(dataBuffer, 0, sizeof( dataBuffer ));
			fdZ = open(AccelZ_Read[indexFile], O_RDONLY);
			read(fdZ, dataBuffer, sizeof(dataBuffer));
			close(fdZ);
			ValueZ +=  atoi(dataBuffer);
			usleep(10);
		};

		ValueX = ValueX/FiltrTime;
		ValueY = ValueY/FiltrTime;
		ValueZ = ValueZ/FiltrTime;
		printf("Accel_X = %i \n", ValueX );
		printf("Accel_Y = %i \n", ValueY );
		printf("Accel_Z = %i \n", ValueZ );

		CalibX=(ValueX*(-1))/2;
		CalibY=(ValueY*(-1))/2;
		CalibZ=(1024-ValueZ)/2;
		CX=CalibX; CY=CalibY; CY=CalibZ;
		printf("CalibX = %i \n", CalibX );
		printf("CalibY = %i \n", CalibY );
		printf("CalibZ = %i \n", CalibZ );

		//printf("To end the test press, press 'y' - when test OK or 'n' - when test failed  \n\n");
		//printf("Press 'y' - to write calibrates or other key to repeat: ");
		//userAnsw = getchar();

		//usleep(1000000);
		//userAnsw = read_console();

	//}while(userAnsw != 'y');

	close(fdX);
	close(fdY);
	close(fdZ);

	memset(dataBuffer, 0, sizeof( dataBuffer ));
	sprintf(dataBuffer, "%i",CalibX);
	fdC = open(CalibX_Read[indexFile], O_WRONLY);
	if(fdC == -1)
	{
		printf( "ERROR: Unable to open device file! \n" );
		return -1;
	}
	write(fdC, dataBuffer, sizeof(dataBuffer));
	close(fdC);

	memset(dataBuffer, 0, sizeof( dataBuffer ));
	sprintf(dataBuffer, "%i",CalibY);
	fdC = open(CalibY_Read[indexFile], O_WRONLY);
	if(fdC == -1)
	{
		printf( "ERROR: Unable to open device file! \n" );
		return -1;
	}
	write(fdC, dataBuffer, sizeof(dataBuffer));
	close(fdC);

	memset(dataBuffer, 0, sizeof( dataBuffer ));
	sprintf(dataBuffer, "%i",CalibZ);
	fdC = open(CalibZ_Read[indexFile], O_WRONLY);
	if(fdC == -1)
	{
		printf( "ERROR: Unable to open device file! \n" );
		return -1;
	}
	write(fdC, dataBuffer, sizeof(dataBuffer));
	close(fdC);

	//Store Calibrate in file
	sprintf(dataBuffer, "%i %i %i",CalibX,CalibY,CalibZ);
	fdC = open(CalibAccel_File_Path, O_WRONLY | O_CREAT);
	if(fdC < 0 )
	{
		printf("Error: %d\n", fdC);
		return -1;
	}
	i = write(fdC, dataBuffer, sizeof(dataBuffer));
	if(i != SN_SIZE)
	{
		printf("Write only %d bytes instead %d\n", i, SN_SIZE);
	}
	i = close(fdC);


	return 0;
};

int FuncConfirm_Battery_Charger_Communication(int Do)
{
	//Confirm detection and communication with battery charger
	FILE * hiddenConsole;
	#define ANSWER_L 1024
	char Answer[ANSWER_L] ="";
	int lastchar;

	const char* ChipInfo = "0x0001";

	if(!Do) return -1;

	hiddenConsole = popen("lsmod | grep ltc4015_charger", "r"); //lsmod | grep pfuze100_regulator
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("hiddenConsole answer: \n%s", Answer);
	if(lastchar){
		hiddenConsole = popen("rmmod ltc4015_charger", "r"); //rmmod pfuze100_regulator
		lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
		Answer[lastchar] = '\0';
		pclose(hiddenConsole);
		printf("hiddenConsole answer: \n%s", Answer);
	}
	hiddenConsole = popen("i2cget -y 2 0x68 0x4A w", "r");
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("hiddenConsole answer: \n%s", Answer);

	if(strncmp(Answer, ChipInfo,6) == 0){
		return 0;
	}
	else{
		return -1;
	};
	return 0;
};

int FuncConfirm_PMIC_Communication(int Do)
{
	//Validate that PMIC is present and responsive via I2C
	FILE * hiddenConsole;
	char Answer[ANSWER_L] ="";
	int lastchar;

	const char* ChipInfo = "0x10";

	if(!Do) return -1;

	hiddenConsole = popen("lsmod | grep pfuze100_regulator", "r"); //lsmod | grep pfuze100_regulator
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("hiddenConsole answer: \n%s", Answer);
	if(lastchar){
		hiddenConsole = popen("rmmod pfuze100_regulator", "r"); //rmmod pfuze100_regulator
		lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
		Answer[lastchar] = '\0';
		pclose(hiddenConsole);
		printf("hiddenConsole answer: \n%s", Answer);
	}
	hiddenConsole = popen("i2cget -y 2 0x08 0x00 b", "r"); //i2cget -y 2 0x08 0x00 b
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("hiddenConsole answer: \n%s", Answer);

	if(strncmp(Answer, ChipInfo,4) == 0){
		return 0;
	}
	else{
		return -1;
	};

	//modprobe pfuze100_regulator
};

int FuncEMMY_163_Connectivity_Check(int Do)
{
	//Confirm EMMY module is responsive
	//Validate Wi-Fi antenna by detecting and printing available Wifi Networks

	FILE * hiddenConsole;
	char Answer[ANSWER_L];
	int lastchar;
	int result = 0;


	if(!Do) return -1;

		if (wifi_first_start_flag!=1)
		{
			hiddenConsole = popen("/root/wifi_on.sh", "r");
			lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
			Answer[lastchar] = '\0';
			pclose(hiddenConsole);
			wifi_first_start_flag=1;
			sleep(5);
		}

	hiddenConsole = popen("lsmod | grep sd8xxx", "r");
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("hiddenConsole answer: \n%s", Answer);
	if(lastchar == 0){
		printf("\n Kernel module 'sd8xxx' not loaded!!! Load module and try again\n");
		return -1;
	}
	hiddenConsole = popen("lsmod | grep mlan", "r");
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("hiddenConsole answer: \n%s", Answer);
	if(lastchar == 0){
		printf("\n Kernel module 'mlan' not loaded!!! Load module and try again\n");
		sprintf (EmmyWiFiBuffer, "Module 'mlan' not loaded");
		result=-1;
	}
	//print networks
	printf("\n\nScanning WiFi networks... Wait 10 sec...\n\n");
	hiddenConsole = popen("iw dev mlan0 scan | grep SSID", "r");

	sleep(10);

	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("\nFound networks: \n%s", Answer);

	char* ptr_1;
	char* ptr_2;
	int cnt=0;
	ptr_1=strstr(Answer, "SSID");
	memset(EmmyWiFiBuffer, 0, 1024);
	if (ptr_1!=NULL)
	{
		ptr_2=strstr(ptr_1, "\n");
		cnt=ptr_2-ptr_1;
		strncpy(EmmyWiFiBuffer, ptr_1, cnt);
		//sprintf (EmmyWiFiBuffer, "Wi-Fi networks are available");
		printf("\nWiFi Ok\n");
		result=0;
	}
	else
	{
		sprintf (EmmyWiFiBuffer, "Wi-Fi networks aren't available");
		printf("\nWiFi Failed\n");
		result=0;
	}

	//Validate Bluetooth Antenna by detecting and printing available Bluetooth devices to pair

	hiddenConsole = popen("lsmod | grep bt8xxx", "r");
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);
	printf("hiddenConsole answer: \n%s", Answer);
	if(lastchar == 0){
		printf("\n Kernel module 'bt8xxx' not loaded!!! Load module and try again");
		sprintf (EmmyBTBuffer, "Module 'bt8xxx' not loaded");
		result=-1;
	}

	hiddenConsole = popen("/root/bt_scan.sh", "r");
	printf("\n\nScanning bluetooth devices networks... Wait 10 sec...\n\n");
	sleep(10);
	lastchar = fread(Answer, 1, ANSWER_L, hiddenConsole);
	Answer[lastchar] = '\0';
	pclose(hiddenConsole);

	char* ptr_3;
	ptr_3=strstr(Answer, ":");
	memset(EmmyBTBuffer, 0, 1024);
		if (ptr_3!=NULL)
		{
			//sprintf (EmmyBTBuffer, "BT devices are available");
			strcat(EmmyBTBuffer, "MAC: ");
			strncpy((EmmyBTBuffer+5), (ptr_3-2), 11);
			printf("\nBT Ok\n");
			result=0;
		}
		else
		{
			sprintf (EmmyBTBuffer, "BT devices aren't available");
			printf("\nBT Failed\n");
			result=0;
		}

	printf("\nFound bluetooth devices: \n%s", Answer);

	if(result == 0){
		return 0;
	}
	else{
		return -1;
	}

}

int FuncAmbient_Light_Sensor_Functionality(int Do)
{
	//Provide live printed results of light sensor brightness detection (user can hit enter to exit)
	char F=0;
	const char *devName[3] = {"/sys/bus/iio/devices/iio:device0/in_illuminance_raw","/sys/bus/iio/devices/iio:device1/in_illuminance_raw","/sys/bus/iio/devices/iio:device2/in_illuminance_raw"};
	int file, i_t=0;

	if(!Do) return -1;

	for(int i=0; i<=2; i++){
		printf("Open: %s\n", devName[i]);
		file = open( devName[i], O_RDONLY );
		if(file == -1){
			printf("%s - not found\n", devName[i]);
		}
		else{
			F=1;
			i_t= i;
			break;
		}
	}

	if(F==0)
	{
		printf( "ERROR: Unable to open device file! \n" );
		return -1;
	}
	printf("Open OK\n");

		memset(LightDataBuffer, 0, sizeof( LightDataBuffer ) );
		file = open(devName[i_t], O_RDONLY);
		read( file, LightDataBuffer, sizeof(LightDataBuffer) );
		close(file);
		printf("Light sensor value = %s \n", LightDataBuffer );

	close(file);
	printf("Close OK\n");

	return 0;
}

int FuncBarometer_Functionality(int Do)
{
	//Provide live printed results of pressure readings and temperature readings (user can hit enter to exit)
	char F=0;
	char dataBuffer[100];
	const char *pressureRead[3] = {"/sys/bus/iio/devices/iio:device0/in_pressure_raw","/sys/bus/iio/devices/iio:device1/in_pressure_raw","/sys/bus/iio/devices/iio:device2/in_pressure_raw"};
	const char *tempRead[3] =     {"/sys/bus/iio/devices/iio:device0/in_temp_raw","/sys/bus/iio/devices/iio:device1/in_temp_raw","/sys/bus/iio/devices/iio:device2/in_temp_raw"};
	int fileTemp,filePressure, i_t=0, i_p=0;

	if(!Do) return -1;

	for(int i=0; i<=2; i++){
		printf("Open: %s\n", tempRead[i]);
		fileTemp = open( tempRead[i], O_RDONLY );
		if(fileTemp == -1){
			printf("%s - not found\n", tempRead[i]);
		}
		else{
			F=1;
			i_t= i;
			break;//continue;
		}
	}

	if(F==0)
	{
		printf( "ERROR: Unable to open device file! \n" );
		return -1;
	}
	printf("Open OK\n");

	F=0;
	for(int i=0; i<=2; i++){
		printf("Open: %s\n", pressureRead[i]);
		filePressure = open( pressureRead[i], O_RDONLY );
		if(filePressure == -1){
			printf("%s - not found\n", pressureRead[i]);
		}
		else{
			F=1;
			i_p = i;
			break;
		}
	}

	if(F==0)
	{
		printf( "ERROR: Unable to open device file! \n" );
		return -1;
	}
	printf("Open OK\n");

	// Temperature data reading
		memset( dataBuffer, 0, sizeof( dataBuffer ) );
		fileTemp = open( tempRead[i_t], O_RDONLY );
		read( fileTemp, dataBuffer, sizeof(dataBuffer) );
		close( fileTemp );

		temperature = atof( dataBuffer );
		temperature *= 0.0625;

		printf( "Temperature Celsius value = %f \n", temperature );
		// 100us delay for sensor
		usleep(100000);

	// Pressure data reading
		memset( dataBuffer, 0, sizeof( dataBuffer ) );
		filePressure = open( pressureRead[i_p], O_RDONLY );//O_RDWR
		read( filePressure, dataBuffer, sizeof( dataBuffer) );
		close( filePressure );
		pressure = atof( dataBuffer );
		pressure *= 0.25;
		pressure /= 100;
		printf( "Pressure hPa value = %i \n", (int)pressure);

	close( fileTemp );
	close( filePressure );

	return 0;
}

int FuncLARA_Module_Testing_Power_Antenna_Permission(int Do)
{
	//Test sending power on to device on UART port
	//Test sending power on to device on USB port
	//Test toggling GPIO signal for RF antenna selection for UART modem versus USB modem

	#define BUFF_SIZE 100
	int LaraErr=0;

		//setup GPIO
	printf( "Setup GPIO for LARA \n" );

		if (Init_GPIO("50", "out")!=1)
		{
			sprintf(LaraBuffer, "Error export pins");
			return -1;
		}
		if (Init_GPIO("52", "out")!=1)
		{
			sprintf(LaraBuffer, "Error export pins");
			return -1;
		}

	//----------------------Power test modem via UART-------------------------------------------
	printf( "LARA power-on\n" );

		if (Write_GPIO("50", "1")!=1)
		{
			sprintf(LaraBuffer, "Error write pin 50");
			return -1;
		}
	usleep(500000);//sleep(1);

	if (Write_GPIO("52", "0")!=1)
	{
		sprintf(LaraBuffer, "Error write pin 52");
		return -1;
	}
	usleep(100000);//sleep(1);

	if (Write_GPIO("52", "1")!=1)
	{
		sprintf(LaraBuffer, "Error write pin 52");
		return -1;
	}
	usleep(200000);//sleep(1);

	if (Write_GPIO("52", "0")!=1)
	{
		sprintf(LaraBuffer, "Error write pin 52");
		return -1;
	}
	usleep(3000000);
	//ttymxc1
	printf( "Send 'ATE0' to LARA\n" );

	if(Init_LARA_SARA("/dev/ttymxc1", 115200) == 1){
		printf( "ERROR init LARA! \n" );
		LaraErr = 1;//return testFailed;
	}
	else{
		printf( "LARA answer 'OK' \n" );
	}
	if(LaraErr){
		return -1;
	}

	printf( "Unexport GPIO for LARA \n" );

		if (DeInit_GPIO("50")!=1)
		{
			sprintf(LaraBuffer, "Error unexport pin 50");
			return -1;
		}
		if (DeInit_GPIO("52")!=1)
		{
			sprintf(LaraBuffer, "Error unexport pin 52");
			return -1;
		}

	sprintf(LaraBuffer, "LARA module recieve AT-command");
	return 0;
}

int FuncSARA_Module_Testing_Power_Antenna_Permission(int Do)
{
	//Test sending power on to device on UART port
	//Test sending power on to device on USB port
	//Test toggling GPIO signal for RF antenna selection for UART modem versus USB modem

	#define BUFF_SIZE 100
	int SaraErr=0;

		//setup GPIO
	printf( "Setup GPIO for SARA \n" );

		if (Init_GPIO("48", "out")!=1)
		{
			sprintf(SaraBuffer, "Error export pins");
			return -1;
		}
		if (Init_GPIO("49", "out")!=1)
		{
			sprintf(SaraBuffer, "Error export pins");
			return -1;
		}

	//----------------------Power test modem via USB------------------------------------------
	usleep(1000000);

	if (Write_GPIO("49", "1")!=1)
	{
		sprintf(SaraBuffer, "Error write pin 49");
		return -1;
	}
	//TODO: проверить появился ли порт ttyACM0
	usleep(12000000);//sleep(12);
	printf( "Send 'ATE0' to SARA\n" );

	if(Init_LARA_SARA("/dev/ttyACM0", 115200) == 1){
		printf( "ERROR init SARA! \n" );
		SaraErr=1;//return testFailed;
	}
	else{
		printf( "SARA answer 'OK' \n" );
	}
	if(SaraErr){
		return -1;
	}

	if (Write_GPIO("48", "1")!=1)
	{
		sprintf(SaraBuffer, "Error write pin 48");
		return -1;
	}

	sleep(1);

	if (Write_GPIO("48", "0")!=1)
	{
		sprintf(SaraBuffer, "Error write pin 48");
		return -1;
	}

	printf( "Unexport GPIO for SARA \n" );

		if (DeInit_GPIO("48")!=1)
		{
			sprintf(SaraBuffer, "Error unexport pin 48");
			return -1;
		}
		if (DeInit_GPIO("49")!=1)
		{
			sprintf(SaraBuffer, "Error unexport pin 49");
			return -1;
		}

	sprintf(SaraBuffer, "SARA module recieve AT-command");
	return 0;
}

int Cameras_Test(int Do, CAMPARAM* camptr1, CAMPARAM* camptr2)
{
	int state1=-1;
	int state2=-1;

	printf("Read camera parameters...\n");
	sleep(1);
	state1=(Read_Cam_Param("/dev/video0", camptr1) ? -1 : 0);
	state2=(Read_Cam_Param("/dev/video1", camptr2) ? -1 : 0);
	//printf("%s, %i, %i, %i", camptr1->description, camptr1->height, camptr1->widht, camptr1->pixel_format);

	if ((state1==0)&&(state2==0)) return 0;
	else return -1;
}

int Init_LARA_SARA(char* port_name, int port_speed){
	char buf_tx[1000]={0};
	char buf_rx[1000]={0};
	int cnt_byte;
	//static
	int port_id;

	port_id=OpenPort(port_name);
	SetPort(port_id, port_speed);
	usleep(1000000);

	ClosePort(port_id);
	usleep(500000);


	///----***----***---***---***---***---***---***---***--
	port_id=OpenPort(port_name);
	printf( "Port ID: %i\n", port_id);
	SetPort(port_id, port_speed);

	sleep(5);

	cnt_byte=sprintf(buf_tx, "ATE0\r");	 // echo off
	puts (buf_tx);
	WritePort(port_id, (unsigned char*)buf_tx, cnt_byte);
	usleep(500000);
	cnt_byte=ReadPort(port_id, (unsigned char*)buf_rx, sizeof(buf_rx));
	puts(buf_rx);
	if (strstr(buf_rx, "OK")==NULL) //SARA-U201-03B-00 //LARA-R204-02B-00
	{
		perror("\r\nError while reading\r\n");
		return -1;
	}

	memset(buf_tx, 0, 1000);
	memset(buf_rx, 0, 1000);
	return 0;

}

void power_init(void)
{
	int fd;
	int n,m;
	char buf[20] = {0};

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if(fd < 0)
	{
		printf("Error while opening - %d\n", fd);
		return;
	}

	n = snprintf(buf, sizeof(buf), "63");
	m = write(fd, buf, n);
	if(m != n)
	{
		printf("Error, not writing\n");
		return;
	}

	m = close(fd);
	if (m != 0)
	{
		printf("Error while closing\n");
		return;
	}
}


void power_set_direction(void)
{
	int fd;
	int n,m;
	char buf[20] = {0};


	fd = open("/sys/class/gpio/gpio63/direction", O_WRONLY);
	if(fd < 0)
	{
		printf("Error while opening - %d\n", fd);
		return;
	}

	n = snprintf(buf, sizeof(buf), "out");
	m = write(fd, buf, n);
	if(m != n)
	{
		printf("Error, not writing\n");
		return;
	}

	m = close(fd);
	if (m != 0)
	{
		printf("Error while closing\n");
		return;
	}
}


void power_set_on(void)
{
	int fd;
	int n,m;
	char buf[20] = {0};


	fd = open("/sys/class/gpio/gpio63/value", O_WRONLY);
	if(fd < 0)
	{
		printf("Error while opening - %d\n", fd);
		return;
	}

	n = snprintf(buf, sizeof(buf), "1");
	m = write(fd, buf, n);
	if(m != n)
	{
		printf("Error, not writing\n");
		return;
	}

	m = close(fd);
	if (m != 0)
	{
		printf("Error while closing\n");
		return;
	}
}


void power_set_off(void)
{
	int fd;
	int n,m;
	char buf[20] = {0};


	fd = open("/sys/class/gpio/gpio63/value", O_WRONLY);
	if(fd < 0)
	{
		printf("Error while opening - %d\n", fd);
		return;
	}

	n = snprintf(buf, sizeof(buf), "0");
	m = write(fd, buf, n);
	if(m != n)
	{
		printf("Error, not writing\n");
		return;
	}

	m = close(fd);
	if (m != 0)
	{
		printf("Error while closing\n");
		return;
	}
}
