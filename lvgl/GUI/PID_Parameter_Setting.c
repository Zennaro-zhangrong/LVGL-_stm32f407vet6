/*
 * PID_Parameter_Setting.c
 *
 *  Created on: 2026年3月18日
 *      Author: zr186
 */
#include "GUI.h"

lv_obj_t *Switch_btnm;
lv_obj_t * roller_P3;
lv_obj_t * roller_P2;
lv_obj_t * roller_P1;
lv_obj_t * roller_I3;
lv_obj_t * roller_I2;
lv_obj_t * roller_I1;
lv_obj_t * roller_D3;
lv_obj_t * roller_D2;
lv_obj_t * roller_D1;
static uint16_t selected_idx = 0;
static unsigned short PID_Parame[18] = {0};




static void sync_button_event_cb(lv_event_t * e)
{
    // 获取事件类型（这里主要处理点击事件）
    lv_event_code_t code = lv_event_get_code(e);

    // 按钮被点击时执行
    if(code == LV_EVENT_CLICKED) {
    	unsigned short P;
    	unsigned short I;
    	unsigned short D;
    	P = 100 * lv_roller_get_selected(roller_P3) +
    		10 * lv_roller_get_selected(roller_P2) +
    		lv_roller_get_selected(roller_P1);
    	I = 100 * lv_roller_get_selected(roller_I3) +
    		10 * lv_roller_get_selected(roller_I2) +
    		lv_roller_get_selected(roller_I1);
    	D = 100 * lv_roller_get_selected(roller_D3) +
    		10 * lv_roller_get_selected(roller_D2) +
    		lv_roller_get_selected(roller_D1);

    	PID_Parame[selected_idx * 3] = P;
    	PID_Parame[selected_idx * 3 + 1] = I;
    	PID_Parame[selected_idx * 3 + 2] = D;
    	param.PID_Channel |= (0x01 << selected_idx);
    	param.PID_Buffer = PID_Parame;
    }
}


static void Switch_btnm_cb(lv_event_t *e)
{
    // 获取按钮矩阵对象
    lv_obj_t *btnm = lv_event_get_target(e);
    selected_idx = lv_btnmatrix_get_selected_btn(btnm);
    unsigned char data[9] = {0};
    data[0] = PID_Parame[selected_idx * 3]/100;
    data[1] = (PID_Parame[selected_idx * 3]%100)/10;
    data[2] = PID_Parame[selected_idx * 3]%10;

    data[3] = PID_Parame[selected_idx * 3 + 1]/100;
    data[4] = (PID_Parame[selected_idx * 3 + 1]%100)/10;
    data[5] = PID_Parame[selected_idx * 3 + 1]%10;

    data[6] = PID_Parame[selected_idx * 3 + 2]/100;
    data[7] = (PID_Parame[selected_idx * 3 + 2]%100)/10;
    data[8] = PID_Parame[selected_idx * 3 + 2]%10;

    lv_roller_set_selected(roller_P3, data[0], LV_ANIM_OFF);
	lv_roller_set_selected(roller_P2, data[1], LV_ANIM_OFF);
	lv_roller_set_selected(roller_P1, data[2], LV_ANIM_OFF);
	lv_roller_set_selected(roller_I3, data[3], LV_ANIM_OFF);
	lv_roller_set_selected(roller_I2, data[4], LV_ANIM_OFF);
	lv_roller_set_selected(roller_I1, data[5], LV_ANIM_OFF);
	lv_roller_set_selected(roller_D3, data[6], LV_ANIM_OFF);
	lv_roller_set_selected(roller_D2, data[7], LV_ANIM_OFF);
	lv_roller_set_selected(roller_D1, data[8], LV_ANIM_OFF);
}




void TileView_PidParamSetting(lv_obj_t * tileview){
    lv_obj_t * label1 = lv_label_create(tileview);
    lv_label_set_text(label1, "<  PID parameter setting  >");
    lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t * list = lv_list_create(tileview);
    lv_obj_set_size(list, 270, 205);
    lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, -15);



    Switch_btnm = lv_btnmatrix_create(list);
    lv_obj_set_size(Switch_btnm,lv_pct(100),lv_pct(60));
    static const char *map[] = {"PID_A", "PID_B","PID_C","\n","PID_D","PID_E", "PID_F",""};
    lv_btnmatrix_set_map(Switch_btnm, map);
    lv_btnmatrix_set_btn_ctrl_all(Switch_btnm, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_one_checked(Switch_btnm, true);
    lv_obj_set_style_shadow_width(Switch_btnm, 0, LV_PART_ITEMS);
    lv_btnmatrix_set_btn_ctrl(Switch_btnm, 0, LV_BTNMATRIX_CTRL_CHECKED);
    lv_obj_add_event_cb(Switch_btnm, Switch_btnm_cb, LV_EVENT_VALUE_CHANGED, NULL);


    lv_obj_t * obj_KP = lv_obj_create(list);
    lv_obj_set_size(obj_KP, lv_pct(100),  lv_pct(85));

    roller_P3 = lv_roller_create(obj_KP);
    lv_obj_set_style_text_line_space(roller_P3, 25, LV_STATE_DEFAULT);
    lv_roller_set_options(roller_P3, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_P3, 3);
    lv_obj_align(roller_P3, LV_ALIGN_LEFT_MID, 40,0);

    roller_P2 = lv_roller_create(obj_KP);
    lv_obj_set_style_text_line_space(roller_P2, 25, LV_STATE_DEFAULT);
    lv_roller_set_options(roller_P2, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_P2, 3);
    lv_obj_align(roller_P2, LV_ALIGN_LEFT_MID, 110,0);

    roller_P1 = lv_roller_create(obj_KP);
    lv_obj_set_style_text_line_space(roller_P1, 25, LV_STATE_DEFAULT);
    lv_roller_set_options(roller_P1, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_P1, 3);
    lv_obj_align(roller_P1, LV_ALIGN_LEFT_MID, 160,0);

    lv_obj_t * label_P1 = lv_label_create(obj_KP);
    lv_obj_align(label_P1, LV_ALIGN_LEFT_MID, 0,0);
    lv_label_set_text(label_P1,"P : ");

    lv_obj_t * label_P2 = lv_label_create(obj_KP);
    lv_obj_align(label_P2, LV_ALIGN_LEFT_MID, 94,0);
    lv_label_set_text(label_P2,LV_SYMBOL_BULLET);








    lv_obj_t * obj_KI = lv_obj_create(list);
    lv_obj_set_size(obj_KI, lv_pct(100), lv_pct(85));

    roller_I3 = lv_roller_create(obj_KI);
    lv_obj_set_style_text_line_space(roller_I3,25, LV_STATE_DEFAULT);
    lv_roller_set_options(roller_I3, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_I3, 3);
    lv_obj_align(roller_I3, LV_ALIGN_LEFT_MID, 40,0);

    roller_I2 = lv_roller_create(obj_KI);
    lv_obj_set_style_text_line_space(roller_I2, 25, LV_STATE_DEFAULT);
    lv_roller_set_options(roller_I2, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_I2, 3);
    lv_obj_align(roller_I2, LV_ALIGN_LEFT_MID, 110,0);

    roller_I1 = lv_roller_create(obj_KI);
    lv_obj_set_style_text_line_space(roller_I1, 25, LV_STATE_DEFAULT);
    lv_roller_set_options(roller_I1, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_I1, 3);
    lv_obj_align(roller_I1, LV_ALIGN_LEFT_MID, 160,0);

    lv_obj_t * label_I1 = lv_label_create(obj_KI);
    lv_obj_align(label_I1, LV_ALIGN_LEFT_MID, 0,0);
    lv_label_set_text(label_I1,"I : ");

    lv_obj_t * label_I2 = lv_label_create(obj_KI);
    lv_obj_align(label_I2, LV_ALIGN_LEFT_MID, 94,0);
    lv_label_set_text(label_I2,LV_SYMBOL_BULLET);







    lv_obj_t * obj_KD = lv_obj_create(list);
    lv_obj_set_size(obj_KD, lv_pct(100), lv_pct(85));

    roller_D3 = lv_roller_create(obj_KD);
    lv_obj_set_style_text_line_space(roller_D3, 25, LV_STATE_DEFAULT);
    lv_roller_set_options(roller_D3, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_D3, 3);
    lv_obj_align(roller_D3, LV_ALIGN_LEFT_MID, 40,0);

    roller_D2 = lv_roller_create(obj_KD);
    lv_obj_set_style_text_line_space(roller_D2, 25, LV_STATE_DEFAULT);
    lv_roller_set_options(roller_D2, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_D2, 3);
    lv_obj_align(roller_D2, LV_ALIGN_LEFT_MID, 110,0);

    roller_D1 = lv_roller_create(obj_KD);
    lv_obj_set_style_text_line_space(roller_D1,25, LV_STATE_DEFAULT);
    lv_roller_set_options(roller_D1, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_D1, 3);
    lv_obj_align(roller_D1, LV_ALIGN_LEFT_MID, 160,0);

    lv_obj_t * label_D1 = lv_label_create(obj_KD);
    lv_obj_align(label_D1, LV_ALIGN_LEFT_MID, 0,0);
    lv_label_set_text(label_D1,"D : ");

    lv_obj_t * label_D2 = lv_label_create(obj_KD);
    lv_obj_align(label_D2, LV_ALIGN_LEFT_MID, 94,0);
    lv_label_set_text(label_D2,LV_SYMBOL_BULLET);








     lv_obj_t * obj_Button = lv_obj_create(list);
     lv_obj_set_size(obj_Button, lv_pct(100), 60);
     lv_obj_t * list_button2 = lv_btn_create(obj_Button);
     lv_obj_set_size(list_button2, lv_pct(60), 40);
     lv_obj_align(list_button2, LV_ALIGN_CENTER, 0,0);
     lv_obj_set_style_bg_color(list_button2, lv_color_hex(0xf0a548), LV_STATE_DEFAULT);
     lv_obj_set_style_bg_color(list_button2, lv_color_hex(0xf9dbb6), LV_STATE_PRESSED);
     lv_obj_t * label_Butt2 = lv_label_create(list_button2);  // 创建标签作为按钮的子对象
     lv_label_set_text(label_Butt2, "Synchronize");                 // 设置显示的文字内容
     lv_obj_center(label_Butt2);
     lv_obj_add_event_cb(list_button2, sync_button_event_cb, LV_EVENT_ALL, NULL);




}
