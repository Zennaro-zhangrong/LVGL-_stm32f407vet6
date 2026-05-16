################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lvgl/src/extra/others/ime/lv_ime_pinyin.c 

OBJS += \
./lvgl/src/extra/others/ime/lv_ime_pinyin.o 

C_DEPS += \
./lvgl/src/extra/others/ime/lv_ime_pinyin.d 


# Each subdirectory must supply rules for building sources it contributes
lvgl/src/extra/others/ime/%.o lvgl/src/extra/others/ime/%.su lvgl/src/extra/others/ime/%.cyclo: ../lvgl/src/extra/others/ime/%.c lvgl/src/extra/others/ime/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lvgl-2f-src-2f-extra-2f-others-2f-ime

clean-lvgl-2f-src-2f-extra-2f-others-2f-ime:
	-$(RM) ./lvgl/src/extra/others/ime/lv_ime_pinyin.cyclo ./lvgl/src/extra/others/ime/lv_ime_pinyin.d ./lvgl/src/extra/others/ime/lv_ime_pinyin.o ./lvgl/src/extra/others/ime/lv_ime_pinyin.su

.PHONY: clean-lvgl-2f-src-2f-extra-2f-others-2f-ime

