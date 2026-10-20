################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lvgl/GUI/GUI.c \
../lvgl/GUI/Joystick_Setting.c \
../lvgl/GUI/LCD&Battery_Setting.c \
../lvgl/GUI/Ladar_Point_map.c \
../lvgl/GUI/NRF_Connect_Setting.c \
../lvgl/GUI/PID_Parameter_Setting.c \
../lvgl/GUI/TileView_Parameter_List.c 

OBJS += \
./lvgl/GUI/GUI.o \
./lvgl/GUI/Joystick_Setting.o \
./lvgl/GUI/LCD&Battery_Setting.o \
./lvgl/GUI/Ladar_Point_map.o \
./lvgl/GUI/NRF_Connect_Setting.o \
./lvgl/GUI/PID_Parameter_Setting.o \
./lvgl/GUI/TileView_Parameter_List.o 

C_DEPS += \
./lvgl/GUI/GUI.d \
./lvgl/GUI/Joystick_Setting.d \
./lvgl/GUI/LCD&Battery_Setting.d \
./lvgl/GUI/Ladar_Point_map.d \
./lvgl/GUI/NRF_Connect_Setting.d \
./lvgl/GUI/PID_Parameter_Setting.d \
./lvgl/GUI/TileView_Parameter_List.d 


# Each subdirectory must supply rules for building sources it contributes
lvgl/GUI/%.o lvgl/GUI/%.su lvgl/GUI/%.cyclo: ../lvgl/GUI/%.c lvgl/GUI/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-lvgl-2f-GUI

clean-lvgl-2f-GUI:
	-$(RM) ./lvgl/GUI/GUI.cyclo ./lvgl/GUI/GUI.d ./lvgl/GUI/GUI.o ./lvgl/GUI/GUI.su ./lvgl/GUI/Joystick_Setting.cyclo ./lvgl/GUI/Joystick_Setting.d ./lvgl/GUI/Joystick_Setting.o ./lvgl/GUI/Joystick_Setting.su ./lvgl/GUI/LCD&Battery_Setting.cyclo ./lvgl/GUI/LCD&Battery_Setting.d ./lvgl/GUI/LCD&Battery_Setting.o ./lvgl/GUI/LCD&Battery_Setting.su ./lvgl/GUI/Ladar_Point_map.cyclo ./lvgl/GUI/Ladar_Point_map.d ./lvgl/GUI/Ladar_Point_map.o ./lvgl/GUI/Ladar_Point_map.su ./lvgl/GUI/NRF_Connect_Setting.cyclo ./lvgl/GUI/NRF_Connect_Setting.d ./lvgl/GUI/NRF_Connect_Setting.o ./lvgl/GUI/NRF_Connect_Setting.su ./lvgl/GUI/PID_Parameter_Setting.cyclo ./lvgl/GUI/PID_Parameter_Setting.d ./lvgl/GUI/PID_Parameter_Setting.o ./lvgl/GUI/PID_Parameter_Setting.su ./lvgl/GUI/TileView_Parameter_List.cyclo ./lvgl/GUI/TileView_Parameter_List.d ./lvgl/GUI/TileView_Parameter_List.o ./lvgl/GUI/TileView_Parameter_List.su

.PHONY: clean-lvgl-2f-GUI

