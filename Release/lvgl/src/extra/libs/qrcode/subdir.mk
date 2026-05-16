################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lvgl/src/extra/libs/qrcode/lv_qrcode.c \
../lvgl/src/extra/libs/qrcode/qrcodegen.c 

OBJS += \
./lvgl/src/extra/libs/qrcode/lv_qrcode.o \
./lvgl/src/extra/libs/qrcode/qrcodegen.o 

C_DEPS += \
./lvgl/src/extra/libs/qrcode/lv_qrcode.d \
./lvgl/src/extra/libs/qrcode/qrcodegen.d 


# Each subdirectory must supply rules for building sources it contributes
lvgl/src/extra/libs/qrcode/%.o lvgl/src/extra/libs/qrcode/%.su lvgl/src/extra/libs/qrcode/%.cyclo: ../lvgl/src/extra/libs/qrcode/%.c lvgl/src/extra/libs/qrcode/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lvgl-2f-src-2f-extra-2f-libs-2f-qrcode

clean-lvgl-2f-src-2f-extra-2f-libs-2f-qrcode:
	-$(RM) ./lvgl/src/extra/libs/qrcode/lv_qrcode.cyclo ./lvgl/src/extra/libs/qrcode/lv_qrcode.d ./lvgl/src/extra/libs/qrcode/lv_qrcode.o ./lvgl/src/extra/libs/qrcode/lv_qrcode.su ./lvgl/src/extra/libs/qrcode/qrcodegen.cyclo ./lvgl/src/extra/libs/qrcode/qrcodegen.d ./lvgl/src/extra/libs/qrcode/qrcodegen.o ./lvgl/src/extra/libs/qrcode/qrcodegen.su

.PHONY: clean-lvgl-2f-src-2f-extra-2f-libs-2f-qrcode

