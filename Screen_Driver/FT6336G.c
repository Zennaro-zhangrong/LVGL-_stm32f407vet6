/*
 * FT6336G.c
 *
 *  Created on: Jan 11, 2026
 *      Author: zr186
 */
#include "FT6336G.h"
#include "../Hardware/UART.h"

extern I2C_HandleTypeDef hi2c1;

// FT6336G I2C从机地址（7位地址，数据手册默认未明确，但行业通用0x38，需硬件确认）
#define FT6336G_ADDR                  0x38

// 基础状态寄存器（只读）
#define FT6336G_REG_DEV_MODE          0x00  // 设备模式
#define FT6336G_REG_GESTURE_ID        0x01  // 手势ID
#define FT6336G_REG_TD_STATUS         0x02  // 触摸点状态

// 触摸点1数据寄存器（只读）
#define FT6336G_REG_TOUCH1_XH         0x03  // 触摸点1 X坐标高字节（含事件标志）
#define FT6336G_REG_TOUCH1_XL         0x04  // 触摸点1 X坐标低字节
#define FT6336G_REG_TOUCH1_YH         0x05  // 触摸点1 Y坐标高字节（含触摸ID）
#define FT6336G_REG_TOUCH1_YL         0x06  // 触摸点1 Y坐标低字节
#define FT6336G_REG_TOUCH1_WEIGHT     0x07  // 触摸点1压力值
#define FT6336G_REG_TOUCH1_MISC       0x08  // 触摸点1附加信息（含触摸面积）

// 触摸点2数据寄存器（只读）
#define FT6336G_REG_TOUCH2_XH         0x09  // 触摸点2 X坐标高字节（含事件标志）
#define FT6336G_REG_TOUCH2_XL         0x0A  // 触摸点2 X坐标低字节
#define FT6336G_REG_TOUCH2_YH         0x0B  // 触摸点2 Y坐标高字节（含触摸ID）
#define FT6336G_REG_TOUCH2_YL         0x0C  // 触摸点2 Y坐标低字节
#define FT6336G_REG_TOUCH2_WEIGHT     0x0D  // 触摸点2压力值
#define FT6336G_REG_TOUCH2_MISC       0x0E  // 触摸点2附加信息（含触摸面积）

// 配置寄存器（读写）
#define FT6336G_REG_TH_GROUP          0x80  // 触摸检测阈值
#define FT6336G_REG_TH_DIFF           0x85  // 滤波系数（原宏定义缺漏，补充关键配置寄存器）
#define FT6336G_REG_CTRL              0x86  // 模式切换控制（无触摸时自动切监控模式）
#define FT6336G_REG_TIME_ENTER_MONITOR 0x87 // 无触摸时进入监控模式的延时
#define FT6336G_REG_ACTIVE_MODE_RATE  0x88  // 主动模式报告率
#define FT6336G_REG_MONITOR_MODE_RATE 0x89  // 监控模式报告率

// 芯片信息寄存器（只读）
#define FT6336G_REG_LIB_VER_H         0xA1  // 库版本号高字节
#define FT6336G_REG_LIB_VER_L         0xA2  // 库版本号低字节
#define FT6336G_REG_CIPHER            0xA3  // 芯片选择ID
#define FT6336G_REG_G_MODE            0xA4  // 中断模式配置（轮询/触发）
#define FT6336G_REG_PWR_MODE          0xA5  // 电源模式
#define FT6336G_REG_FIRMWARE_VERSION  0xA6  // 固件版本
#define FT6336G_REG_FOCALTECH_ID      0xA8  // FocalTech厂商ID（固定0x11）
#define FT6336G_REG_RELEASE_CODE      0xAF  // 发布版本号
#define FT6336G_REG_STATE             0xBC  // 当前工作模式状态

// 手势识别相关配置寄存器（读写）
#define FT6336G_REG_RADIAN_VALUE      0x91  // 旋转手势最小允许角度
#define FT6336G_REG_OFFSET_LEFT_RIGHT 0x92  // 左右滑动最大偏移量
#define FT6336G_REG_OFFSET_UP_DOWN    0x93  // 上下滑动最大偏移量
#define FT6336G_REG_DISTANCE_LEFT_RIGHT 0x94 // 左右滑动最小距离
#define FT6336G_REG_DISTANCE_UP_DOWN  0x95  // 上下滑动最小距离
#define FT6336G_REG_DISTANCE_ZOOM     0x96  // 缩放手势最小距离








///////////////////////////////////////文件内函数起始//////////////////////////////////

FT6336G_Device Touch_Screen = {
		.hi2c = &hi2c1,
		.intPort = GPIOB,
		.intPin = GPIO_PIN_5,
		.rstPort = GPIOD,
		.rstPin = GPIO_PIN_7,
		.width = 320,
		.height = 240,
		.touchData = {0},
		.initialized = 0,
		.DevAddress = 0x38 << 1,
		.IRQ_Flag = FT6336G_IDLE
};//创建唯一实体化的触摸屏

/**
 * @brief  向FT6336G指定寄存器写入1字节数据
 * @param  指定写入的寄存器
 * @param  填写要写入的数据，FT6636G的每个寄存器的大小只有8位
 * @retval 写入状态返回
 */
static FT6336G_Status FT6336G_Send_Byte(uint16_t MemAddress,uint8_t Data){
	uint8_t state = HAL_I2C_Mem_Write(Touch_Screen.hi2c,
										  Touch_Screen.DevAddress,
										  MemAddress,
										  I2C_MEMADD_SIZE_8BIT,
										  &Data,
										  1,
										  0xFF);
	if(state == HAL_OK){
		return FT6336G_OK;
	}else{
		log_printf("\r\n\r\n\r\n触摸屏数据发送失败\r\n");
		return FT6336G_ERROR;

	}
}




/**
 * @brief  从FT6336G指定寄存器读取一字节数据
 * @param  指定读取的寄存器
 * @retval 返回读取到的数据
 */
static uint8_t FT6336G_Read_Byte(uint16_t MemAddress){
	uint8_t read_data = 0;
	uint8_t state = HAL_I2C_Mem_Read(Touch_Screen.hi2c,
										 Touch_Screen.DevAddress,
										 MemAddress,
										 I2C_MEMADD_SIZE_8BIT,
										 &read_data,
										 1,
										 0xFF);
	if(state == HAL_OK){
		return read_data;
	}else{
		log_printf("\r\n\r\n\r\n触摸屏数据读取失败\r\n");
		return 0;
	}
}









////////////////////////////////文件对外函数////////////////////////////////////
/*
 * @brief  触摸屏初始化配置
 */
void FT6336G_Init(void){



	if(FT6336G_Read_Byte(FT6336G_REG_FOCALTECH_ID) == 0x11){
		log_printf("Touch screen detection successful, Device ID 0x11\r\n");
		FT6336G_Send_Byte(FT6336G_REG_DEV_MODE,0x00);//设备模式为工作模式
		FT6336G_Send_Byte(FT6336G_REG_G_MODE,0x00);//中断为轮询模式
		FT6336G_Send_Byte(FT6336G_REG_PWR_MODE,0x00);//电源模式为主动模式
		FT6336G_Send_Byte(FT6336G_REG_TH_GROUP,0x18);//触摸灵敏阈值
		FT6336G_Send_Byte(FT6336G_REG_TH_DIFF,0x25);//滤波函数系数
		FT6336G_Send_Byte(FT6336G_REG_CTRL,0x01);//无触摸主动进入监听模式
		FT6336G_Send_Byte(FT6336G_REG_TIME_ENTER_MONITOR,0x20);//进入监听模式延时，单位ms
		FT6336G_Send_Byte(FT6336G_REG_STATE,0x01);//当前工作状态为主动模式
		FT6336G_Send_Byte(FT6336G_REG_ACTIVE_MODE_RATE,0x3C);//主动模式报告率60Hz
		FT6336G_Send_Byte(FT6336G_REG_MONITOR_MODE_RATE,0x28);//监听模式报告率25Hz
		log_printf("Touch screen initialization successful\r\n");
	}else{
		log_printf("Touch screen detection failed. Please try restarting or checking the IIC connection of the touch screen.\r\n");
	}


}




/*
 * @brief  中断引脚对外接口，放到总外部中断回调函数中
 * @param  中断触发引脚，为了适配总中断回调函数
 */
void FT6336G_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == Touch_Screen.intPin){
		if(HAL_GPIO_ReadPin(Touch_Screen.intPort,Touch_Screen.intPin)
				== GPIO_PIN_RESET){
			Touch_Screen.IRQ_Flag = FT6336G_TOUCH;
		}else{
			Touch_Screen.IRQ_Flag = FT6336G_IDLE;
		}
	}
}





void FT6336G_GetCoordinate(void){
    // 读取第一个触摸点的X坐标（高低字节）
	uint8_t Count = FT6336G_Read_Byte(FT6336G_REG_TD_STATUS);
	Count = Count & 0x0F;
	uint16_t x_high = 0;
	uint16_t x_low = 0;
	uint16_t y_high = 0;
	uint16_t y_low = 0;
	uint16_t x_count = 0;
	uint16_t y_count = 0;
	if(Count >= 1){
		for(uint8_t i = 0; i < 2; i++){
			x_high = FT6336G_Read_Byte(FT6336G_REG_TOUCH1_XH);
			x_low = FT6336G_Read_Byte(FT6336G_REG_TOUCH1_XL);
			y_high = FT6336G_Read_Byte(FT6336G_REG_TOUCH1_YH);
			y_low = FT6336G_Read_Byte(FT6336G_REG_TOUCH1_YL);
		    Touch_Screen.touchData.y[0] = 0 + (((x_high & 0x0F) << 8) | x_low);
		    y_count = y_count + Touch_Screen.touchData.y[0];
		    Touch_Screen.touchData.x[0] = 320 - (((y_high & 0x0F) << 8) | y_low);
		    x_count = x_count + Touch_Screen.touchData.x[0];
		}
		Touch_Screen.touchData.y[0] = y_count / 2;
		y_count = 0;
		Touch_Screen.touchData.x[0] = x_count / 2;
		x_count = 0;
	}

	if(Count == 2){
		x_high = FT6336G_Read_Byte(FT6336G_REG_TOUCH2_XH);
		x_low = FT6336G_Read_Byte(FT6336G_REG_TOUCH2_XL);
		y_high = FT6336G_Read_Byte(FT6336G_REG_TOUCH2_YH);
		y_low = FT6336G_Read_Byte(FT6336G_REG_TOUCH2_YL);
		Touch_Screen.touchData.y[1] = ((x_high & 0x0F) << 8) | x_low;
		Touch_Screen.touchData.x[1] = 320 - (((y_high & 0x0F) << 8) | y_low);
	}
/*
	log_printf("触摸点坐标1（%d,%d）,坐标2（%d,%d）\r\n",
				  Touch_Screen.touchData.x[0],
				  Touch_Screen.touchData.y[0],
				  Touch_Screen.touchData.x[1],
				  Touch_Screen.touchData.y[1]);
*/
}
