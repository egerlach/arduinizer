################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Arduinator.c \
../src/cr_startup_lpc13.c \
../src/program.c \
../src/timer16_int.c 

OBJS += \
./src/Arduinator.o \
./src/cr_startup_lpc13.o \
./src/program.o \
./src/timer16_int.o 

C_DEPS += \
./src/Arduinator.d \
./src/cr_startup_lpc13.d \
./src/program.d \
./src/timer16_int.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DNDEBUG -D__USE_CMSIS=CMSISv1p30_LPC13xx -D__CODE_RED -D__REDLIB__ -I"/home/jlamothe/src/arduinizer/LPC13xx_Lib/inc" -I"/home/jlamothe/src/arduinizer/CMSISv1p30_LPC13xx/inc" -O2 -Os -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


