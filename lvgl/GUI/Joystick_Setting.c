/*
 * Joystick_Setting.c
 *
 *  Created on: 2026年3月18日
 *      Author: zr186
 */

#include "GUI.h"
#include "main.h"
#include "../../lv_conf.h"
#include "../../Hardware/ADC.h"
lv_obj_t * Handle_bgl;
lv_obj_t * Handle_bgr;
lv_obj_t * Handle_mbl;
lv_obj_t * Handle_mbr;
lv_point_t base_left, base_right;//摇杆的起始位置（绝对位置）

Switch SW_L = {0, 11, 0};
Switch SW_R = {0, 11, 0};

extern lvgl_parameter_t param;
extern Joystick Joystick_LY;
extern Joystick Joystick_RY;
extern Joystick Joystick_LX;
extern Joystick Joystick_RX;


/*
 * @功能：执行后刷新摇杆的位置
 * @参数：摇杆偏置量
 * */
static void Update_Joystick_Pos(int16_t LX, int16_t LY, int16_t RX, int16_t RY){
    lv_obj_set_pos(Handle_mbl, base_left.x + LX / 11, base_left.y - LY / 11);
    lv_obj_set_pos(Handle_mbr, base_right.x + RX / 11, base_right.y - RY / 11);
    param.Joystick_LX = LX;
    param.Joystick_LY = LY;
    param.Joystick_RX = RX;
    param.Joystick_RY = RY;
    //log_printf("LX[%d]       LY[%d]       RX[%d]        RY[%d]\r\n",LX,LY,RX,RY);
}

/*
 * @功能：执行后显示摇杆有无被按下
 * @参数：左摇杆触发中断的总次数
 * @参数：右摇杆触发中断的总次数
 */
static void Updata_Joystick_Color(unsigned char SW_LpressNum, unsigned char SW_RpressNum)
{
	if((SW_LpressNum & 0x01) == 1) {
		lv_obj_set_style_bg_color(Handle_bgl, lv_color_hex(0xff0000), LV_STATE_DEFAULT);
	}else{
		lv_obj_set_style_bg_color(Handle_bgl, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
	}
	if((SW_RpressNum & 0x01) == 1) {
		lv_obj_set_style_bg_color(Handle_bgr, lv_color_hex(0xff0000), LV_STATE_DEFAULT);
	}else{
		lv_obj_set_style_bg_color(Handle_bgr, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
	}
}

/*
 * @功能：摇杆更新定时器回调函数
 * @参数：定时器句柄
 * */
static void Joystick_Update_TIM_CallBack(lv_timer_t *timer){
	Joystick_ADC_Get();
	Update_Joystick_Pos(Joystick_LX.Joystic_Value,
			Joystick_LY.Joystic_Value,
			Joystick_RX.Joystic_Value,
			Joystick_RY.Joystic_Value);
	Updata_Joystick_Color(param.Joystick_SwitchL, param.Joystick_SwitchR);
}



static void switchA_event_cb(lv_event_t * e){
   // 从事件中获取触发事件的开关对象
	lv_obj_t * sw = lv_event_get_target(e);

	// 🔑 获取开关当前状态：true=打开，false=关闭
	bool switch_state = lv_obj_has_state(sw, LV_STATE_CHECKED);

	// 打印状态（根据你的需求替换逻辑）
	if(switch_state) {
		param.Joystick_SwitchRudder = 1;
		//log_printf("SwitchA_open\r\n");
	} else {
		param.Joystick_SwitchRudder = 0;
		//log_printf("SwitchA_close\r\n");
	}

}

static void switchB_event_cb(lv_event_t * e){
	// 从事件中获取触发事件的开关对象
	lv_obj_t * sw = lv_event_get_target(e);

	// 🔑 获取开关当前状态：true=打开，false=关闭
	bool switch_state = lv_obj_has_state(sw, LV_STATE_CHECKED);

	// 打印状态（根据你的需求替换逻辑）
	if(switch_state) {
		param.Joystick_SwitchLoop = 1;
		//log_printf("SwitchB_open\r\n");
	} else {
		param.Joystick_SwitchLoop = 0;
		//log_printf("SwitchB_close\r\n");
	}
}


void TileView_JoysticSetting(lv_obj_t *tileview){

	Handle_bgl = lv_obj_create(tileview);
	lv_obj_set_size(Handle_bgl, 80, 80);
	lv_obj_align(Handle_bgl, LV_ALIGN_CENTER, -80, -60);
	lv_obj_set_style_radius(Handle_bgl, LV_RADIUS_CIRCLE, LV_STATE_DEFAULT | LV_PART_MAIN);
	lv_obj_set_style_border_width(Handle_bgl, 2, LV_STATE_DEFAULT | LV_PART_MAIN);
	lv_obj_set_style_border_color(Handle_bgl, lv_color_hex(0xcfcfcf), LV_STATE_DEFAULT | LV_PART_MAIN);
	lv_obj_set_style_bg_color(Handle_bgl, lv_color_hex(0xffffff), LV_STATE_DEFAULT);


	Handle_bgr = lv_obj_create(tileview);
	lv_obj_set_size(Handle_bgr, 80, 80);
	lv_obj_align(Handle_bgr, LV_ALIGN_CENTER, 80, -60);
	lv_obj_set_style_radius(Handle_bgr, LV_RADIUS_CIRCLE, LV_STATE_DEFAULT | LV_PART_MAIN);
	lv_obj_set_style_border_width(Handle_bgr, 2, LV_STATE_DEFAULT | LV_PART_MAIN);
	lv_obj_set_style_border_color(Handle_bgr, lv_color_hex(0xcfcfcf), LV_STATE_DEFAULT | LV_PART_MAIN);
	lv_obj_set_style_bg_color(Handle_bgr, lv_color_hex(0xffffff), LV_STATE_DEFAULT);


	Handle_mbl = lv_obj_create(tileview);
	lv_obj_set_size(Handle_mbl,40, 40);
	lv_obj_align(Handle_mbl, LV_ALIGN_CENTER, -80, -60);
	lv_obj_set_style_radius(Handle_mbl, LV_RADIUS_CIRCLE, LV_STATE_DEFAULT | LV_PART_MAIN);
	lv_obj_set_style_border_width(Handle_mbl, 2, LV_STATE_DEFAULT | LV_PART_MAIN);
	lv_obj_set_style_border_color(Handle_mbl, lv_color_hex(0x070707), LV_STATE_DEFAULT | LV_PART_MAIN);
	lv_obj_set_style_bg_color(Handle_mbl,lv_color_hex(0x2094f0), LV_PART_MAIN | LV_STATE_DEFAULT);



	Handle_mbr = lv_obj_create(tileview);
	lv_obj_set_size(Handle_mbr, 40, 40);
	lv_obj_align(Handle_mbr, LV_ALIGN_CENTER, 80, -60);
	lv_obj_set_style_radius(Handle_mbr, LV_RADIUS_CIRCLE, LV_STATE_DEFAULT | LV_PART_MAIN);
	lv_obj_set_style_border_width(Handle_mbr, 2, LV_STATE_DEFAULT | LV_PART_MAIN);
	lv_obj_set_style_border_color(Handle_mbr, lv_color_hex(0x070707), LV_STATE_DEFAULT | LV_PART_MAIN);
	lv_obj_set_style_bg_color(Handle_mbr,lv_color_hex(0x2094f0), LV_PART_MAIN | LV_STATE_DEFAULT);


	lv_obj_t * label1 = lv_label_create(tileview);
	lv_label_set_text(label1, "Joystick function settings  >");
	lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 0);



	lv_obj_t * list = lv_list_create(tileview);
	lv_obj_set_size(list, 260, 120);
	lv_obj_align(list, LV_ALIGN_CENTER, 0, 45);



	lv_obj_t * list_option1 = lv_list_add_btn(list, NULL, "Right_hand rudder");
	lv_obj_set_size(list_option1, lv_pct(100), lv_pct(42));
	lv_obj_t * list_switch1 = lv_switch_create(list_option1);
	lv_obj_align(list_switch1, LV_ALIGN_RIGHT_MID, 0,0);
	lv_obj_add_event_cb(list_switch1, switchA_event_cb, LV_EVENT_VALUE_CHANGED, NULL);



	lv_obj_t * list_option2 = lv_list_add_btn(list, NULL, "Speed Close-Loop");
	lv_obj_set_size(list_option2, lv_pct(100), lv_pct(42));
	lv_obj_t * list_switch2 = lv_switch_create(list_option2);
	lv_obj_align(list_switch2, LV_ALIGN_RIGHT_MID, 0,0);
	lv_obj_add_event_cb(list_switch2, switchB_event_cb, LV_EVENT_VALUE_CHANGED, NULL);


    lv_obj_t * list_obj1 =  lv_obj_create(list);
    lv_obj_set_size(list_obj1, lv_pct(100), 120);
    lv_obj_t* list_label3= lv_label_create(list_obj1);
	lv_obj_set_width(list_label3, lv_pct(100));
	lv_obj_align(list_label3, LV_ALIGN_CENTER, 0,0);
	lv_label_set_text(list_label3,"[!!!]:    Press the left button to take control, press the right button to release control.");

//获取摇杆的绝对位置
    base_left.x = -80;
    base_left.y = -60;
    base_right.x = 80;
    base_right.y = -60;

    lv_timer_create(Joystick_Update_TIM_CallBack, 100, NULL);
}



/*
 * @功能：按键中断回调函数，放在外部中断回调函数中即可
 * */
void Joystick_Swich_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == SW_L_Pin
	&& (SW_L.swTimeStart + (uint32_t)SW_L.SW_debounce) <= LV_TICK_CUSTOM_SYS_TIME_EXPR){
		param.Joystick_SwitchL++;
		SW_L.swTimeStart = LV_TICK_CUSTOM_SYS_TIME_EXPR;
	}

	if(GPIO_Pin == SW_R_Pin
	&& (SW_R.swTimeStart + (uint32_t)SW_R.SW_debounce) <= LV_TICK_CUSTOM_SYS_TIME_EXPR){
		param.Joystick_SwitchR++;
		SW_R.swTimeStart = LV_TICK_CUSTOM_SYS_TIME_EXPR;
	}
}


