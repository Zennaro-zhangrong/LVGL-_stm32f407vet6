/*
 * Ladar_Point_map.c
 *
 *  Created on: 2026年4月28日
 *      Author: zr186
 */
#include "GUI.h"


lv_coord_t * x_arr;
lv_coord_t * y_arr;
void TileView_Ladar_Pointmap(lv_obj_t *tileview ){
    lv_obj_t * chart = lv_chart_create(tileview);//创建图表
    lv_obj_set_size(chart, 260,260);//设置图表大小
    lv_obj_align(chart, LV_ALIGN_CENTER, 0, 0);//图表居中
    lv_obj_set_style_line_width(chart, 0, LV_PART_ITEMS);/* 去掉点间连线 */
    lv_obj_t * rect = lv_obj_create(chart);//在图表上创建矩形
    lv_obj_set_size(rect, 7, 9); //设置矩形大小宽7(70cm)，高9(90cm)
    lv_obj_set_style_bg_color(rect, lv_color_hex(0xa00e0e), 0);//红色
    lv_obj_set_style_bg_opa(rect, LV_OPA_80, 0);       // 850% 透明度
    lv_obj_set_style_border_width(rect, 0, 0);         // 无边框
    lv_obj_set_style_radius(rect, 0, 0);               // 直角
    lv_obj_center(rect);//居中
    lv_obj_move_foreground(rect);//设为顶层


    lv_obj_set_style_width(chart, 2, LV_PART_INDICATOR);//把散点的大小设置为最小2*2
    lv_obj_set_style_height(chart, 2, LV_PART_INDICATOR);
    lv_obj_set_style_radius(chart, 2, LV_PART_INDICATOR);//设置半径为 0，变成方形

    lv_chart_set_type(chart, LV_CHART_TYPE_SCATTER);//图表的模式为：散点图模式

    lv_chart_set_div_line_count(chart, 3,3);//6条线分成四宫格
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_X, 0, 255);//表格长256
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 255);//表格宽256

    lv_chart_set_point_count(chart, 666);//表格上的点数为1024点，对应最外围的大正方形

    //创建唯一序列点，黑色，坐标范围(0,0)~(255,255)
    lv_chart_series_t * ser = lv_chart_add_series(chart,
    		lv_color_hex(0x0739a3),
			LV_CHART_AXIS_PRIMARY_Y);
    //创建散点缓冲区，会在lvgl的内存池创建1024点的坐标，占用内存4KB
    x_arr = lv_chart_get_x_array(chart, ser);
    y_arr = lv_chart_get_y_array(chart, ser);



		// 填充数据（示例为50个点，沿 y=x 直线）
		for(uint32_t i = 0; i < 256; i++) {
			x_arr[i] = 0;
			y_arr[i] = i;
		}
		// 填充数据（示例为50个点，沿 y=x 直线）
		for(uint32_t i = 256; i < 512; i++) {
			x_arr[i] = i - 256;
			y_arr[i] = 255;
		}
		// 填充数据（示例为50个点，沿 y=x 直线）
		for(uint32_t i = 512; i < 666; i++) {
			x_arr[i] = 255;
			y_arr[i] = 768 - i;
		}

	lv_chart_refresh(chart);
}
