/**
 * @file lv_port_indev_templ.h
 *
 */

/*将此文件复制并重命名为 "lv_port_indev.h"，并将此值设置为 "1" 以启用内容*/
#if 1

#ifndef LV_PORT_INDEV_TEMPL_H
#define LV_PORT_INDEV_TEMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      头文件包含
 *********************/
#include "../../lvgl/lvgl.h"
#include "../../../Screen_Driver/FT6336G.h"
/*********************
 *      宏定义
 *********************/

/**********************
 *      类型定义
 **********************/
/**********************
 * 全局函数声明
 **********************/
void lv_port_indev_init(void);  // 输入设备驱动初始化函数

/**********************
 *      宏
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_INDEV_TEMPL_H*/

#endif /*禁用/启用文件内容（0=禁用，1=启用）*/
