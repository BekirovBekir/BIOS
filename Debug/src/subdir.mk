################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ComPort.c \
../src/Display.c \
../src/FrameBuffer.c \
../src/GPIO.c \
../src/PreAsm.c \
../src/TS.c \
../src/eeprom.c \
../src/i2c.c 

OBJS += \
./src/ComPort.o \
./src/Display.o \
./src/FrameBuffer.o \
./src/GPIO.o \
./src/PreAsm.o \
./src/TS.o \
./src/eeprom.o \
./src/i2c.o 

C_DEPS += \
./src/ComPort.d \
./src/Display.d \
./src/FrameBuffer.d \
./src/GPIO.d \
./src/PreAsm.d \
./src/TS.d \
./src/eeprom.d \
./src/i2c.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


