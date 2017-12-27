################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ComPort.c \
../src/FrameBuffer.c \
../src/GPIO.c \
../src/TS.c \
../src/i2c.c 

OBJS += \
./src/ComPort.o \
./src/FrameBuffer.o \
./src/GPIO.o \
./src/TS.o \
./src/i2c.o 

C_DEPS += \
./src/ComPort.d \
./src/FrameBuffer.d \
./src/GPIO.d \
./src/TS.d \
./src/i2c.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


