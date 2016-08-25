################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../board/board.c \
../board/clock_config.c \
../board/pin_mux.c 

OBJS += \
./board/board.o \
./board/clock_config.o \
./board/pin_mux.o 

C_DEPS += \
./board/board.d \
./board/clock_config.d \
./board/pin_mux.d 


# Each subdirectory must supply rules for building sources it contributes
board/%.o: ../board/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D"CPU_MK20DX256VLH7" -I../startup -I../board -I../utilities -I../CMSIS -I../drivers -I"C:\Users\David\workspace.kds\USB Serial Demo\source" -I"C:\Users\David\workspace.kds\USB Serial Demo\usb_1.1.0" -I"C:\Users\David\workspace.kds\USB Serial Demo\usb_1.1.0\device" -I"C:\Users\David\workspace.kds\USB Serial Demo\usb_1.1.0\include" -I"C:\Users\David\workspace.kds\USB Serial Demo\usb_1.1.0\osa" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


