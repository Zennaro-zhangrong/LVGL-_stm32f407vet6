################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Screen_Driver/FT6336G.c \
../Screen_Driver/LCD.c \
../Screen_Driver/SPI_cb.c 

OBJS += \
./Screen_Driver/FT6336G.o \
./Screen_Driver/LCD.o \
./Screen_Driver/SPI_cb.o 

C_DEPS += \
./Screen_Driver/FT6336G.d \
./Screen_Driver/LCD.d \
./Screen_Driver/SPI_cb.d 


# Each subdirectory must supply rules for building sources it contributes
Screen_Driver/%.o Screen_Driver/%.su Screen_Driver/%.cyclo: ../Screen_Driver/%.c Screen_Driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Screen_Driver

clean-Screen_Driver:
	-$(RM) ./Screen_Driver/FT6336G.cyclo ./Screen_Driver/FT6336G.d ./Screen_Driver/FT6336G.o ./Screen_Driver/FT6336G.su ./Screen_Driver/LCD.cyclo ./Screen_Driver/LCD.d ./Screen_Driver/LCD.o ./Screen_Driver/LCD.su ./Screen_Driver/SPI_cb.cyclo ./Screen_Driver/SPI_cb.d ./Screen_Driver/SPI_cb.o ./Screen_Driver/SPI_cb.su

.PHONY: clean-Screen_Driver

