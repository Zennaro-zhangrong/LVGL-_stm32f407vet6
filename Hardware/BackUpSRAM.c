/*
 * BackUpSRAM.c
 *
 *  Created on: 2026年3月23日
 *      Author: zr186
 */
#include "main.h"
#include "../lvgl/GUI/GUI.h"
#include <string.h>

#define BKPSRAM_BASE 0x40024000

/**
 * @brief 备份SRAM初始化
 * @note  这段代码应在启动时，初始化备份域时调用
 */
void BackUpSRAM_Init(void)
{
    // 1. 使能电源接口时钟
    __HAL_RCC_PWR_CLK_ENABLE();

    // 2. 使能对备份域的访问 (DBP位)
    HAL_PWR_EnableBkUpAccess();

    // 3. 使能备份SRAM时钟
    __HAL_RCC_BKPSRAM_CLK_ENABLE();

    // 4. 使能备份调压器 (在VBAT模式下为备份SRAM供电)
    HAL_PWREx_EnableBkUpReg();

    // 5. 等待备份调压器就绪 (必须等待，否则数据可能丢失)
    while (__HAL_PWR_GET_FLAG(PWR_FLAG_BRR) == RESET);

    // 6. (可选) 首次上电或电池掉电后，对备份SRAM初始化一个已知值
    PWR->CSR |= PWR_CSR_BRE; // 确保使能位被置位，以备后用
}

void BackUp_LvglParameter(void){
	lvgl_parameter_t *PdataBkp = (lvgl_parameter_t *)BKPSRAM_BASE;
	lvgl_parameter_t *src_data = GUI_GetParameter();
	memcpy(PdataBkp, src_data, sizeof(lvgl_parameter_t));
}
