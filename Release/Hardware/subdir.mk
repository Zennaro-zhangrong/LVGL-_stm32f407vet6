################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Hardware/ADC.c \
../Hardware/BackUpSRAM.c \
../Hardware/BatteryVoltage.c \
../Hardware/NRF24L01.c \
../Hardware/UART.c 

OBJS += \
./Hardware/ADC.o \
./Hardware/BackUpSRAM.o \
./Hardware/BatteryVoltage.o \
./Hardware/NRF24L01.o \
./Hardware/UART.o 

C_DEPS += \
./Hardware/ADC.d \
./Hardware/BackUpSRAM.d \
./Hardware/BatteryVoltage.d \
./Hardware/NRF24L01.d \
./Hardware/UART.d 


# Each subdirectory must supply rules for building sources it contributes
Hardware/%.o Hardware/%.su Hardware/%.cyclo: ../Hardware/%.c Hardware/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Hardware

clean-Hardware:
	-$(RM) ./Hardware/ADC.cyclo ./Hardware/ADC.d ./Hardware/ADC.o ./Hardware/ADC.su ./Hardware/BackUpSRAM.cyclo ./Hardware/BackUpSRAM.d ./Hardware/BackUpSRAM.o ./Hardware/BackUpSRAM.su ./Hardware/BatteryVoltage.cyclo ./Hardware/BatteryVoltage.d ./Hardware/BatteryVoltage.o ./Hardware/BatteryVoltage.su ./Hardware/NRF24L01.cyclo ./Hardware/NRF24L01.d ./Hardware/NRF24L01.o ./Hardware/NRF24L01.su ./Hardware/UART.cyclo ./Hardware/UART.d ./Hardware/UART.o ./Hardware/UART.su

.PHONY: clean-Hardware

