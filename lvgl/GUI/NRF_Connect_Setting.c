/*
 * NRF_Connect_Setting.c
 *
 *  Created on: 2026年4月24日
 *      Author: zr186
 */

#include "GUI.h"

lv_obj_t * state_Btn;
lv_obj_t * label_state_Btn;
lv_obj_t * RCRD_label;

static void nrf_btm_event_cb(lv_event_t * e){
    lv_obj_t * obj = lv_event_get_target(e);

    // 获取当前被点击的按钮 ID（0、1、2……）
    uint32_t id = lv_btnmatrix_get_selected_btn(obj);
    param.MAX_SpeedSet = (unsigned char)id;
}




void TileView_NRFConnectSetting(lv_obj_t *tileview){
	 lv_obj_t * label1 = lv_label_create(tileview);
	 lv_label_set_text(label1, "<  MAX_Speed & NRFstate  >");
	 lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 0);

	lv_obj_t * list = lv_list_create(tileview);
    lv_obj_set_size(list, 270, 205);
    lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, -15);



    lv_obj_t * label2 = lv_label_create(list);
    lv_label_set_text(label2, "     Set the max speed for car.");
    lv_obj_set_size(label2,lv_pct(100),14);

    lv_obj_t * btm_nrf = lv_btnmatrix_create(list);
    lv_obj_set_size(btm_nrf,lv_pct(100),80);
    static const char *map[] = { "slow","medium","fast", "" };
    lv_btnmatrix_set_map(btm_nrf, map);
    lv_btnmatrix_set_btn_ctrl_all(btm_nrf, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_one_checked(btm_nrf, true);  // 单选模式
    lv_obj_set_style_shadow_width(btm_nrf, 0, LV_PART_ITEMS);
    lv_btnmatrix_set_btn_ctrl(btm_nrf, 0, LV_BTNMATRIX_CTRL_CHECKED); // 默认选中250Kbps

    //  绑定回调（切换选择时触发）
    lv_obj_add_event_cb(btm_nrf, nrf_btm_event_cb, LV_EVENT_VALUE_CHANGED, NULL);


    lv_obj_t * NRF_State_obj = lv_obj_create(list);
    lv_obj_set_size(NRF_State_obj,lv_pct(100),lv_pct(50));


    RCRD_label = lv_label_create(NRF_State_obj);
    lv_label_set_text_fmt(RCRD_label, "R:[ %d kB/s]   D:[ %d ms]", 0, 0);
    lv_obj_align(RCRD_label, LV_ALIGN_TOP_MID, 0, 0);


    state_Btn = lv_btn_create(NRF_State_obj);
    lv_obj_set_size(state_Btn,lv_pct(90),40);
    lv_obj_align(state_Btn, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(state_Btn, LV_OBJ_FLAG_CLICKABLE); //开启点击模式
    lv_obj_set_style_bg_color(state_Btn, lv_color_hex(0x8b0303), LV_STATE_DEFAULT); // 未选中（默认）
    lv_obj_set_style_bg_color(state_Btn, lv_color_hex(0xdcdbdb), LV_STATE_PRESSED); // 按下时的过渡颜色（可选）
    label_state_Btn = lv_label_create(state_Btn);  // 创建标签作为按钮的子对象
    lv_label_set_text(label_state_Btn, "Disconnect! Searching...");                 // 设置显示的文字内容
    lv_obj_center(label_state_Btn);
}


void NRF_ConnectState(uint8_t state){
	if(state == 0){
	    lv_obj_set_style_bg_color(state_Btn, lv_color_hex(0x8b0303), LV_STATE_DEFAULT); // 未选中（默认）
	    lv_label_set_text(label_state_Btn, "Disconnect! Searching...");
	}else{
	    lv_obj_set_style_bg_color(state_Btn, lv_color_hex(0x025d0e), LV_STATE_DEFAULT); // 未选中（默认）
	    lv_label_set_text(label_state_Btn, "Connected~");

	}
}


void NRF_TransmissionStatus_Update(void){
	lv_label_set_text_fmt(RCRD_label, "R:[ %d kB/s]   D:[ %d ms]", param.NRF_realRate, param.NRF_realDelay);
}
