/*
 * NRF_Connect_Setting.c
 *
 *  Created on: 2026年4月24日
 *      Author: zr186
 */

#include "GUI.h"

extern lvgl_parameter_t param;
static void nrf_btm_event_cb(lv_event_t * e){
    lv_obj_t * obj = lv_event_get_target(e);

    // 获取当前被点击的按钮 ID（0、1、2……）
    uint32_t id = lv_btnmatrix_get_selected_btn(obj);
    param.NRF_RateBtn = (unsigned char)id;
    switch(id) {
        case 0:
        	log_printf("当前速率：250Kbps\r\n");
            break;
        case 1:
        	log_printf("当前速率：1Mbps\r\n");
            break;
        case 2:
        	log_printf("当前速率：2Mbps\r\n");
            break;
        default: break;
    }
}



static void state_btn_event_cb(lv_event_t * e)
{
    // 获取按钮对象
    lv_obj_t * btn = lv_event_get_target(e);
    // 获取按钮上的标签
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    //获取当前双态状态：true=选中(绿色)，false=未选中(红色)
    bool is_checked = lv_obj_has_state(btn, LV_STATE_CHECKED);

    // 根据状态切换文字 + 执行逻辑
    if(is_checked) {
    	param.NRF_Connect = 1;
        lv_label_set_text(label, "Close Video");   // 选中 → 绿色
        // 这里写连接逻辑
    } else {
    	param.NRF_Connect = 0;
        lv_label_set_text(label, "Try to Connect");// 未选中 → 红色
        // 这里写断开逻辑
    }
}



void TileView_NRFConnectSetting(lv_obj_t *tileview){
	 lv_obj_t * label1 = lv_label_create(tileview);
	 lv_label_set_text(label1, "<  NRF24L01 Connect Setting  >");
	 lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 0);

	lv_obj_t * list = lv_list_create(tileview);
    lv_obj_set_size(list, 270, 205);
    lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, -15);



    lv_obj_t * label2 = lv_label_create(list);
    lv_label_set_text(label2, "     Select communication rate.");
    lv_obj_set_size(label2,lv_pct(100),14);

    lv_obj_t * btm_nrf = lv_btnmatrix_create(list);
    lv_obj_set_size(btm_nrf,lv_pct(100),80);
    static const char *map[] = { "250Kbps","1Mbps","2Mbps", "" };
    lv_btnmatrix_set_map(btm_nrf, map);
    lv_btnmatrix_set_btn_ctrl_all(btm_nrf, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_one_checked(btm_nrf, true);  // 单选模式
    lv_obj_set_style_shadow_width(btm_nrf, 0, LV_PART_ITEMS);
    lv_btnmatrix_set_btn_ctrl(btm_nrf, 0, LV_BTNMATRIX_CTRL_CHECKED); // 默认选中250Kbps

    //  绑定回调（切换选择时触发）
    lv_obj_add_event_cb(btm_nrf, nrf_btm_event_cb, LV_EVENT_VALUE_CHANGED, NULL);


    lv_obj_t * NRF_State_obj = lv_obj_create(list);
    lv_obj_set_size(NRF_State_obj,lv_pct(100),lv_pct(50));


    lv_obj_t * RCRD_label = lv_label_create(NRF_State_obj);
    lv_label_set_text_fmt(RCRD_label, "R:[ %d kB/s]   D:[ %d ms]", 0, 0);
    lv_obj_align(RCRD_label, LV_ALIGN_TOP_MID, 0, 0);


    lv_obj_t * state_Btn = lv_btn_create(NRF_State_obj);
    lv_obj_set_size(state_Btn,lv_pct(80),40);
    lv_obj_align(state_Btn, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(state_Btn, LV_OBJ_FLAG_CHECKABLE); // 2. 开启【切换模式】→ 点击会在 未选中/选中 之间切换（关键！）
    lv_obj_set_style_bg_color(state_Btn, lv_color_hex(0x8b0303), LV_STATE_DEFAULT); // 未选中（默认）
    lv_obj_set_style_bg_color(state_Btn, lv_color_hex(0x025d0e), LV_STATE_CHECKED); // 选中态（双态核心）
    lv_obj_set_style_bg_color(state_Btn, lv_color_hex(0xdcdbdb), LV_STATE_PRESSED); // 按下时的过渡颜色（可选）
    lv_obj_t * label_Butt2 = lv_label_create(state_Btn);  // 创建标签作为按钮的子对象
    lv_label_set_text(label_Butt2, "Try to Connect");                 // 设置显示的文字内容
    lv_obj_center(label_Butt2);
    lv_obj_add_event_cb(state_Btn, state_btn_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
}

