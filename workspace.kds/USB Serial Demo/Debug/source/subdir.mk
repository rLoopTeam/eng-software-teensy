################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/usb_device_cdc_acm.c \
../source/usb_device_ch9.c \
../source/usb_device_descriptor.c \
../source/virtual_com.c 

OBJS += \
./source/usb_device_cdc_acm.o \
./source/usb_device_ch9.o \
./source/usb_device_descriptor.o \
./source/virtual_com.o 

C_DEPS += \
./source/usb_device_cdc_acm.d \
./source/usb_device_ch9.d \
./source/usb_device_descriptor.d \
./source/virtual_com.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D"CPU_MK20DX256VLH7" -I../startup -I../board -I../utilities -I../CMSIS -I../drivers -I"C:\Users\David\workspace.kds\USB Serial Demo\source" -I"C:\Users\David\workspace.kds\USB Serial Demo\usb_1.1.0" -I"C:\Users\David\workspace.kds\USB Serial Demo\usb_1.1.0\device" -I"C:\Users\David\workspace.kds\USB Serial Demo\usb_1.1.0\include" -I"C:\Users\David\workspace.kds\USB Serial Demo\usb_1.1.0\osa" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


