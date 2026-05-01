/*
 * SPI_cb.c
 *
 *  Created on: 2026年3月15日
 *      Author: zr186
 */
#include "LCD.h"
extern SPI_HandleTypeDef hspi2;


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi){
	LCD_SPI_TxCpltCallback(hspi);
}
