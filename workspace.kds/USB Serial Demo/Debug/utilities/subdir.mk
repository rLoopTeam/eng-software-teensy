################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../utilities/fsl_debug_console.c \
../utilities/fsl_notifier.c \
../utilities/fsl_sbrk.c \
../utilities/fsl_shell.c 

OBJS += \
./utilities/fsl_debug_console.o \
./utilities/fsl_notifier.o \
./utilities/fsl_sbrk.o \
./utilities/fsl_shell.o 

C_DEPS += \
./utilities/fsl_debug_console.d \
./utilities/fsl_notifier.d \
./utilities/fsl_sbrk.d \
./utilities/fsl_shell.d 


# Each subdirectory must supply rules for building sources it contributes
utilities/%.o: ../utilities/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D"CPU_MK20DX256VLH7" -I../startup -I../board -I../utilities -I../CMSIS -I../drivers -I"C:\Users\David\Documents\GitHub\eng-software-teensy\workspace.kds\USB Serial Demo\source" -I"C:\Users\David\Documents\GitHub\eng-software-teensy\workspace.kds\USB Serial Demo\usb_1.1.0" -I"C:\Users\David\Documents\GitHub\eng-software-teensy\workspace.kds\USB Serial Demo\usb_1.1.0\device" -I"C:\Users\David\Documents\GitHub\eng-software-teensy\workspace.kds\USB Serial Demo\usb_1.1.0\include" -I"C:\Users\David\Documents\GitHub\eng-software-teensy\workspace.kds\USB Serial Demo\usb_1.1.0\osa" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


