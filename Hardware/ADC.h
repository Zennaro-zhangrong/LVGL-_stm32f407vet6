/*
 * ADC.h
 *
 *  Created on: Jan 8, 2026
 *      Author: zr186
 */

#ifndef ADC_H_
#define ADC_H_
#include "main.h"

void ADC_Init(void);
void ADC_Updata(void);

typedef struct{
	uint16_t Joystic_Calibrate;//某个摇杆的校准值
	int16_t Joystic_Value;//当前摇杆的去除偏置的数值
	int8_t Count_Direction;//取值只有-1和1
	uint8_t ADC_BuffChannel;//标记来自ADC_Buff的数据通道
}Joystick;


typedef struct{
	uint8_t SW_PressNum;
	uint8_t SW_debounce;
	uint32_t swTimeStart;
}Switch;


extern Joystick Joystick_LY;
extern Joystick Joystick_LX;
extern Joystick Joystick_RY;
extern Joystick Joystick_RX;


void Joystick_ADC_Get(void);
unsigned char Battery_Percentage_Get(void);


void Battery_Voltage_Get_Callback(unsigned short ADC_VAL);
unsigned short Battery_SamplingValue_Get(void);
void Battery_SamplingValue_Init(void);
#endif /* ADC_H_ */
