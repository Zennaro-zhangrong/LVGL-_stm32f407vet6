/*
 * lcd_nrf_settings.c
 *
 *  Created on: 2026年3月18日
 *      Author: zr186
 *
 *  Description: LCD与无线连接设置界面，包含电池信息显示和屏幕亮度调节。
 */

/*==================== INCLUDE ====================*/
#include "GUI.h"
#include "../lvgl.h"
#include "../../Screen_Driver/LCD.h"

/*==================== EXTERNAL VARIABLES ====================*/
extern lvgl_parameter_t param;  // 全局参数结构体，存储电池信息和屏幕亮度

/*==================== STATIC VARIABLES ====================*/
lv_obj_t *label_battery_info;     // 显示电池详细信息的标签
lv_obj_t *label_brightness;       // 显示当前亮度等级的标签
lv_obj_t *slider;
/*==================== STATIC FUNCTION PROTOTYPES ====================*/
static void timer_update_ui(lv_timer_t *timer);
/*==================== STATIC FUNCTIONS ====================*/

/**
 * @brief 定时器回调函数，周期性更新UI元素。
 *        统一处理电池信息刷新和亮度调节，避免多个回调并发导致的UI冲突。
 * @param timer 定时器指针
 */
static void timer_update_ui(lv_timer_t *timer)
{
    /* ---------- 更新电池信息标签 ---------- */
    char *battery_symbol;
    if (param.Battery_Level >= 80) {
        battery_symbol = LV_SYMBOL_BATTERY_FULL;
    } else if (param.Battery_Level >= 60) {
        battery_symbol = LV_SYMBOL_BATTERY_3;
    } else if (param.Battery_Level >= 40) {
        battery_symbol = LV_SYMBOL_BATTERY_2;
    } else if (param.Battery_Level >= 15) {
        battery_symbol = LV_SYMBOL_BATTERY_1;
    } else {
        battery_symbol = LV_SYMBOL_BATTERY_EMPTY;
    }
    param.Screen_Brightness = lv_slider_get_value(slider);

    lv_obj_invalidate(label_battery_info);
    lv_label_set_text_fmt(label_battery_info,
                          "%s [%d%%]        "LV_SYMBOL_CHARGE" [%d.%04d V]",
						  battery_symbol,
                          param.Battery_Level,
                          (unsigned char)param.Battery_Voltage,
                          (unsigned short)((param.Battery_Voltage - (unsigned short)param.Battery_Voltage) * 10000));
    lv_label_set_text_fmt(label_brightness, " Screen Brightness: %d / 15", param.Screen_Brightness);
    LCD_Brightness(param.Screen_Brightness);   // 应用亮度到硬件
}


/*==================== PUBLIC FUNCTIONS ====================*/

/**
 * @brief 创建LCD与连接设置界面（嵌入TileView）
 * @param tileview 父容器（TileView对象）
 */
void TileView_LcdBatterySetting(lv_obj_t *tileview)
{
    /* ---------- 顶部标题 ---------- */
    lv_obj_t *title_label = lv_label_create(tileview);
    lv_label_set_text(title_label, "< LCD & Battery Settings >");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 0);

    /* ---------- 主列表容器 ---------- */
    lv_obj_t *list = lv_list_create(tileview);
    lv_obj_set_size(list, 270, 205);
    lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, -15);

    /* ---------- 电池信息区域 ---------- */
    lv_obj_t *battery_container = lv_obj_create(list);
    lv_obj_set_size(battery_container, LV_PCT(100), 90);

    /* 静态说明文字 */
    lv_obj_t *label_battery_title = lv_label_create(battery_container);
    lv_obj_set_size(label_battery_title, LV_PCT(100), 20);
    lv_label_set_text(label_battery_title, "     RC Battery Information");
    lv_obj_align(label_battery_title, LV_ALIGN_TOP_MID, 0, 0);

    /* 动态电池信息标签（将在定时器中刷新） */
    label_battery_info = lv_label_create(battery_container);
    lv_obj_align(label_battery_info, LV_ALIGN_TOP_MID, 0, 30);


    /* ---------- 亮度调节区域 ---------- */
    lv_obj_t *brightness_container = lv_obj_create(list);
    lv_obj_set_size(brightness_container, LV_PCT(100), 110);

    /* 亮度显示标签（将在定时器中刷新） */
    label_brightness = lv_label_create(brightness_container);
    lv_label_set_text_fmt(label_brightness, " Screen Brightness: %d / 15", param.Screen_Brightness);
    lv_obj_align(label_brightness, LV_ALIGN_BOTTOM_MID, 0, 0);

    /* 亮度调节滑块 */
    slider = lv_slider_create(brightness_container);
    lv_obj_align(slider, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_size(slider, LV_PCT(85), 30);
    lv_slider_set_range(slider, 1, 15);
    lv_slider_set_value(slider, param.Screen_Brightness, LV_ANIM_ON);

    /* ---------- 启动UI刷新定时器（周期50ms） ---------- */
    lv_timer_create(timer_update_ui, 200, NULL);
}

/**
 * @brief 外部电池数据更新接口（由其他模块调用）
 * @param battery_level  电池电量百分比（0~100）
 * @param battery_voltage 电池电压（浮点数，如3.85V）
 */
void lvgl_battery_data_update(unsigned char battery_level, float battery_voltage)
{
    param.Battery_Level = battery_level;
    param.Battery_Voltage = battery_voltage;
}
