################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../startup/system_MK20LH7.c 

S_UPPER_SRCS += \
../startup/startup_MK20LH7.S 

OBJS += \
./startup/startup_MK20LH7.o \
./startup/system_MK20LH7.o 

C_DEPS += \
./startup/system_MK20LH7.d 

S_UPPER_DEPS += \
./startup/startup_MK20LH7.d 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

startup/%.o: ../startup/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D"CPU_MK20DX256VLH7" -I../startup -I../board -I../utilities -I../CMSIS -I../drivers -I"C:\Users\David\Documents\GitHub\eng-software-teensy\workspace.kds\USB Serial Demo\source" -I"C:\Users\David\Documents\GitHub\eng-software-teensy\workspace.kds\USB Serial Demo\usb_1.1.0" -I"C:\Users\David\Documents\GitHub\eng-software-teensy\workspace.kds\USB Serial Demo\usb_1.1.0\device" -I"C:\Users\David\Documents\GitHub\eng-software-teensy\workspace.kds\USB Serial Demo\usb_1.1.0\include" -I"C:\Users\David\Documents\GitHub\eng-software-teensy\workspace.kds\USB Serial Demo\usb_1.1.0\osa" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


