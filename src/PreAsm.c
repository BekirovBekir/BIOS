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


#include "../inc/PreAsm.h"
#include "../inc/eeprom.h"

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
	int fd;
	int i;

	if(!Do) return -1;

	memset(buffer_in, 0, BUFFER_SIZE);
	memset(buffer_out, 0, BUFFER_SIZE);

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
	char userAnsw = ' ', ch=' ';
	FILE *fp;


	if(!Do) return -1;


	fp = fopen( CalibAccel_File_Path, "r" );//fdC = open( CalibAccel_File_Path, O_RDONLY );
	if(fp == NULL){
		printf("Start calibrate\n\n");

	}
	else{
		fscanf (fp, "%i%i%i", &CalibX, &CalibY, &CalibZ);

		printf("Device already calibrate! Calibrate values: x = %i; y = %i; z = %i\n", CalibX, CalibY, CalibZ);
		printf("Press 'y' to calibrate or 'n' to exit: ");
		do{
			ch = getchar();
		}while(ch !='y'&& ch !='n');

		if(ch == 'n'){
			return 0;
		}
		AlreadyCalibrFlag = 1;
		fclose(fp);
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

		printf("CalibX = %i \n", CalibX );
		printf("CalibY = %i \n", CalibY );
		printf("CalibZ = %i \n", CalibZ );

		//printf("To end the test press, press 'y' - when test OK or 'n' - when test failed  \n\n");
		printf("Press 'y' - to write calibrates or other key to repeat: ");
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
