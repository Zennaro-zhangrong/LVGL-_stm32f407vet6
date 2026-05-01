/*
 * BatteryVoltage.c
 *
 *  Created on: 2026年4月20日
 *      Author: zr186
 */
#include "ADC.h"
#include "main.h"
#include <stdlib.h>
#include <string.h>
#include "UART.h"
#include "../lvgl/GUI/GUI.h"
#include "../Screen_Driver/LCD.h"

#define ADC_SIZE 11//这是计算缓冲区的大小，务必设为奇数且小于256
static CCM unsigned short Battery_ADC_Value_BUFF[ADC_SIZE]= {0};//计算缓冲区本体
static CCM unsigned short Battery_Output_averageBUFF[ADC_SIZE] = {0};
static CCM unsigned short Battery_Real_averageBUFF[ADC_SIZE] = {0};
extern LCD_DevTypeDef LCD_Device;//主要使用其亮度值

/*
 * @功能：放在ADC采集回调函数用来从不同通道的数据中分离电池电压信号
 * 并自动刷新到计算缓冲区
 * @参数：每次采集实际的电池电压采样值
 */
void Battery_Voltage_Get_Callback(unsigned short ADC_VAL){
	static CCM unsigned char i = 0;
	Battery_ADC_Value_BUFF[i] = ADC_VAL;
	i++;
	if(i == ADC_SIZE) i = 0;
}




/*
 * @功能：比较函数，仅为下文的median_ushort_qsort()函数服务
 */
static int cmp_ushort(const void *a, const void *b) {
    unsigned short ua = *(const unsigned short*)a;
    unsigned short ub = *(const unsigned short*)b;
    if (ua < ub) return -1;
    if (ua > ub) return 1;
    return 0;
}

/*
 * @功能：中值滤波，将计算缓冲区的中值求出返回
 * */
 static unsigned short median_ushort_qsort(void) {
	static CCM unsigned short dataCopy[ADC_SIZE];
	memcpy(dataCopy, Battery_ADC_Value_BUFF, sizeof(Battery_ADC_Value_BUFF));
	qsort(dataCopy, ADC_SIZE, sizeof(unsigned short), cmp_ushort);
	return dataCopy[(ADC_SIZE - 1) / 2];
}


 /*
  * @功能：滑动窗口均值滤波
  * @参数：窗口缓冲区指针
  * @参数：窗口大小
  * @参数：要填入的值
  * @返回：窗口的平均值
  * @注意：函数在原地址操作
  */
 static uint16_t average_filter(uint16_t *Pbuff, uint8_t size, uint16_t new_val)
 {
	 char average_window = size;
     uint32_t sum = 0;
     for(int i = 0; i < average_window -1; i++){
    	 Pbuff[i] = Pbuff[i + 1];
    	 sum = sum + (uint32_t)Pbuff[i];
     }
     sum = sum + (uint32_t)new_val;
     Pbuff[average_window - 1] = new_val;

     return (uint16_t)(sum / average_window);
 }




/*
 * @功能：计算电池压降，用于拟合未放电时电池电压
 * @返回：压降值
 * @注意：该模型电流变化与屏幕亮度相关
 * */
static float Battery_Voltage_dropGet(void){
	static float Fixed_voltage_drop = 0.00f;//电池电压输出后的固定压降，PMOS引起
	static float Battery_internal_resistance = 0.7f;//电池的固定内阻
	static float Battery_Output_current;
	static float Battery_voltage_drop;

	Battery_Output_current = //实时计算电流
			150.0f + (float)LCD_Device.Brightness * 7.14f;
	Battery_voltage_drop = //实时计算电池压降
			Fixed_voltage_drop + Battery_internal_resistance * Battery_Output_current * 0.001f;

	return Battery_voltage_drop;
}


/*
 *  @功能：计算电池输出电压值(存在压降)
 *  @返回：电池电压值
 *  */
static float Battery_Output_Get(void){
	//将单片机该ADC引脚的实际电压计算出来
	float measure_vol = (float)(average_filter(Battery_Output_averageBUFF, ADC_SIZE, median_ushort_qsort())) / 4096.0f;
	float reference_vol = 3.3f;  //参考电压
	float magnification = 3.4f; //放大倍数
	float calculate_vol = (measure_vol * 3.3f / magnification) + reference_vol;
	return calculate_vol;
}


/*
 *  @功能：计算电池电压值，拟合压降反推静态电压
 *  @返回：电池电压值
 *  */
static float Battery_volatge_Get(void){
	//将电池实际电压计算出来
	float Voltage_Drop = Battery_Voltage_dropGet();
	float measure_vol = (float)median_ushort_qsort() *0.0008056f;
	float reference_vol = 3.3f;  //参考电压
	float magnification = 3.4f; //放大倍数
	float calculate_vol = (measure_vol / magnification) + reference_vol;
	calculate_vol = (Voltage_Drop + calculate_vol) * 10000.0f;
	calculate_vol = (float)(average_filter(Battery_Real_averageBUFF, ADC_SIZE, (uint16_t)calculate_vol)) / 10000.0f;
	return calculate_vol;
}


/*
 * @功能：初始化缓冲区，避免刚上电时电压数据异常
 * @注意：此函数放在ADC初始化函数之后
 * */
void Battery_SamplingValue_Init(void){
	uint16_t median_value = 0;
	 for(int i = 0; i < ADC_SIZE; i++){
		 HAL_Delay(5);
		 Battery_Output_Get();
		 Battery_volatge_Get();
	 }
}




/*
 * @功能：将电池电压换算为电池电量
 * @返回：电池电量
 * */
unsigned char Battery_Percentage_Get(void){
	static unsigned long long timestart = -20000;
	static unsigned char battery_percentage = 0;
	float calculate_vol = 0;//赋初值

	if(HAL_GetTick() - timestart >= 2000){
		timestart = HAL_GetTick();

		calculate_vol = Battery_Output_Get();
		battery_percentage = (unsigned char)(((Battery_volatge_Get() - 3.7f) *100) /0.5f);
		if(battery_percentage >= 99) battery_percentage = 99;
		lvgl_battery_data_update(battery_percentage,calculate_vol);
	}
	return battery_percentage;
}




