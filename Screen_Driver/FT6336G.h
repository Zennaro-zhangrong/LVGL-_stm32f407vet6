/*
 * FT6336G.h
 *
 *  Created on: Jan 11, 2026
 *      Author: zr186
 */

#ifndef FT6336G_H_
#define FT6336G_H_


#include "main.h"


//创建触摸屏设备状态的返回值
typedef enum{
	FT6336G_OK = 0,
	FT6336G_BUSY = 1,
	FT6336G_ERROR = 2,
	FT6336G_TOUCH = 3,
	FT6336G_IDLE = 4

}FT6336G_Status;


typedef enum {
    GESTURE_NONE = 0x00,        // 无手势
    GESTURE_MOVE_UP = 0x10,     // 上滑手势
    GESTURE_MOVE_RIGHT = 0x14,  // 右滑手势
    GESTURE_MOVE_DOWN = 0x18,   // 下滑手势
    GESTURE_MOVE_LEFT = 0x1C,   // 左滑手势
    GESTURE_ZOOM_IN = 0x48,     // 放大手势
    GESTURE_ZOOM_OUT = 0x49     // 缩小手势
} FT6336G_Gesture;





// 触摸点结构体
typedef struct {
    uint8_t touchCount;           // 触摸点数量 (0-2)
    uint16_t x[2];                // X坐标
    uint16_t y[2];                // Y坐标
    uint8_t weight[2];            // 触摸重量/压力
    uint8_t area[2];              // 触摸面积
    FT6336G_Gesture gesture;      // 手势
    uint8_t touchStatus;          // 触摸状态
} FT6336G_TouchData;



// 设备结构体
typedef struct {
    I2C_HandleTypeDef *hi2c;      // I2C句柄
    GPIO_TypeDef *intPort;        // INT中断引脚端口
    uint16_t intPin;              // INT中断引脚
    GPIO_TypeDef *rstPort;        // 复位引脚端口
    uint16_t rstPin;              // 复位引脚
    uint16_t width;               // 触摸屏宽度
    uint16_t height;              // 触摸屏高度
    FT6336G_TouchData touchData;  // 触摸数据
    uint8_t initialized;          // 初始化标志
    uint16_t DevAddress;		  // 设备IIC地址
    volatile uint8_t IRQ_Flag;    //中断触发标志位
} FT6336G_Device;



extern FT6336G_Device Touch_Screen;
void FT6336G_EXTI_Callback(uint16_t GPIO_Pin);
void FT6336G_Init(void);
void FT6336G_GetCoordinate(void);
#endif /* FT6336G_H_ */
