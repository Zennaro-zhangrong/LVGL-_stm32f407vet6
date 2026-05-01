/*
 * ADC.c
 *
 *  Created on: Jan 8, 2026
 *      Author: zr186
 */
#include "main.h"
#include "UART.h"
#include "ADC.h"
#include "NRF24L01.h"
#include "../lvgl/GUI/GUI.h"
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
static uint16_t ADC_Buff[5] = {0};

Joystick Joystick_LY;
Joystick Joystick_LX;
Joystick Joystick_RY;
Joystick Joystick_RX;




void ADC_Init(void){
	if(HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_Buff, 5) == HAL_OK){
		log_printf("ADC initialization succeeded!\r\n");
	}else{
		log_printf("ADC initialization failed!\r\n");
	}

	Joystick_LY.ADC_BuffChannel = 2;
	Joystick_LX.ADC_BuffChannel = 1;
	Joystick_RY.ADC_BuffChannel = 4;
	Joystick_RX.ADC_BuffChannel = 3;//上面四个通道需要自行确定

	Battery_SamplingValue_Init();
	Joystick_LY.Joystic_Calibrate = (float)ADC_Buff[Joystick_LY.ADC_BuffChannel];
	Joystick_LX.Joystic_Calibrate = (float)ADC_Buff[Joystick_LX.ADC_BuffChannel];
	Joystick_RY.Joystic_Calibrate = (float)ADC_Buff[Joystick_RY.ADC_BuffChannel];
	Joystick_RX.Joystic_Calibrate = (float)ADC_Buff[Joystick_RX.ADC_BuffChannel];
	log_send_loop();
}



void Joystick_ADC_Get(void){
	Joystick_LX.Joystic_Value =
			-1 * (ADC_Buff[Joystick_LX.ADC_BuffChannel] - Joystick_LX.Joystic_Calibrate) / 8;
	Joystick_LY.Joystic_Value =
			1 * (ADC_Buff[Joystick_LY.ADC_BuffChannel] - Joystick_LY.Joystic_Calibrate) / 8;
	Joystick_RX.Joystic_Value =
			1 * (ADC_Buff[Joystick_RX.ADC_BuffChannel] - Joystick_RX.Joystic_Calibrate) / 8;
	Joystick_RY.Joystic_Value =
			-1 * (ADC_Buff[Joystick_RY.ADC_BuffChannel] - Joystick_RY.Joystic_Calibrate) / 8;
	static float K[8] = {
			0.98077, 1.02,//LX左右
			1.003936, 0.9963,//LY上下
			1.045082, 0.9586466,//RX左右
			1.0165,0.984556//RY上下
	};
	//左负右正，下负上正
	if(Joystick_LX.Joystic_Value < 0) Joystick_LX.Joystic_Value = (float)Joystick_LX.Joystic_Value * K[0];
	else Joystick_LX.Joystic_Value = (float)Joystick_LX.Joystic_Value * K[1];

	if(Joystick_LY.Joystic_Value < 0) Joystick_LY.Joystic_Value = (float)Joystick_LY.Joystic_Value * K[3];
	else Joystick_LY.Joystic_Value = (float)Joystick_LY.Joystic_Value * K[2];

	if(Joystick_RX.Joystic_Value < 0) Joystick_RX.Joystic_Value = (float)Joystick_RX.Joystic_Value * K[4];
	else Joystick_RX.Joystic_Value = (float)Joystick_RX.Joystic_Value * K[5];

	if(Joystick_RY.Joystic_Value < 0) Joystick_RY.Joystic_Value = (float)Joystick_RY.Joystic_Value * K[7];
	else Joystick_RY.Joystic_Value = (float)Joystick_RY.Joystic_Value * K[6];

}



void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){
	Battery_Voltage_Get_Callback(ADC_Buff[0]);
}
