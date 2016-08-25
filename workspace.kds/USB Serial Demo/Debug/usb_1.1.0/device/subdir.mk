################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../usb_1.1.0/device/usb_device_dci.c \
../usb_1.1.0/device/usb_device_khci.c 

OBJS += \
./usb_1.1.0/device/usb_device_dci.o \
./usb_1.1.0/device/usb_device_khci.o 

C_DEPS += \
./usb_1.1.0/device/usb_device_dci.d \
./usb_1.1.0/device/usb_device_khci.d 


# Each subdirectory must supply rules for building sources it contributes
usb_1.1.0/device/%.o: ../usb_1.1.0/device/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D"CPU_MK20DX256VLH7" -I../startup -I../board -I../utilities -I../CMSIS -I../drivers -I"C:\Users\David\workspace.kds\USB Serial Demo\source" -I"C:\Users\David\workspace.kds\USB Serial Demo\usb_1.1.0" -I"C:\Users\David\workspace.kds\USB Serial Demo\usb_1.1.0\device" -I"C:\Users\David\workspace.kds\USB Serial Demo\usb_1.1.0\include" -I"C:\Users\David\workspace.kds\USB Serial Demo\usb_1.1.0\osa" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


