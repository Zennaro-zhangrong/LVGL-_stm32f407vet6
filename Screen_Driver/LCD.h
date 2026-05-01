/*
 * LCD.h
 *
 *  Created on: Jan 8, 2026
 *      Author: zr186
 */

#ifndef LCD_H_
#define LCD_H_
#include "main.h"
#include "../lvgl/lvgl.h"


#define LCD_CS_Valid	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin,GPIO_PIN_RESET)
#define LCD_CS_Invalid	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin,GPIO_PIN_SET)
#define LCD_RST_Valid	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin,GPIO_PIN_RESET)
#define LCD_RST_Invalid	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin,GPIO_PIN_SET)
#define LCD_Command_Valid	HAL_GPIO_WritePin(LCD_RE_GPIO_Port, LCD_RE_Pin,GPIO_PIN_RESET)
#define LCD_Data_Valid	HAL_GPIO_WritePin(LCD_RE_GPIO_Port, LCD_RE_Pin,GPIO_PIN_SET)



typedef enum
{
  Screen_OK       = 0x00U,
  Screen_ERROR    = 0x01U,
  Screen_BUSY     = 0x02U,
  Screen_TIMEOUT  = 0x03U
} Screen_StatusTypeDef;


typedef struct {
    uint8_t SetX;       // X轴地址设置指令
    uint8_t SetY;       // Y轴地址设置指令
    uint8_t W_Ram;      // 写显存指令
    uint8_t R_Ram;      // 读显存指令
    uint8_t Direction;  // 当前显示方向（0123）
    uint16_t Width;     // 当前显示宽度
    uint16_t Height;    // 当前显示高度
    uint16_t Clear_Color;//清屏颜色
    uint8_t Brightness;  //屏幕背光亮度0到F
    uint8_t LCD_TX_Flag; //判断是否发送完成
} LCD_DevTypeDef;


typedef struct{
	uint8_t Queue_sum;
	uint16_t X1;
	uint16_t Y1;
	uint16_t X2;
	uint16_t Y2;
	lv_color_t *color_p;
	uint16_t size;
	lv_disp_drv_t * disp_drv;
}SPI_send_queue;

void LCD_Init(void);
void LCD_Brightness(uint8_t Brightness);
void LCD_ClearDisplay(uint16_t Color);
void LCD_Set_Page_Cursor(uint16_t XL, uint16_t YL, uint16_t XR, uint16_t YR);
Screen_StatusTypeDef LCD_Fill_Page_Buffer(uint8_t* pData, uint16_t size);
void LCD_ScreenBuffer_Send(lv_color_t * color_p, uint16_t size);
uint8_t SPI_Transmission_ENQueue(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void SPI_Transmission_DEQueue(void);
void LCD_Apply_ScreenFilling(void);
void LCD_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
#endif /* LCD_H_ */
