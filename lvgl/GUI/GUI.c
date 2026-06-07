#include "GUI.h"
#include "../lvgl.h"
lvgl_parameter_t param ={
    .Screen_Brightness = 15,
	.Joystick_SwitchL = 0,
    .Joystick_SwitchR = 0,
	.Battery_Level = 0,
	.Battery_Voltage = 0,
	.video_transmission_switch = 0
};


static void tileview_event_cb(lv_event_t * e)
{
    // 获取事件对象（Tileview 本身）
    lv_obj_t * tileview = lv_event_get_target(e);
    lv_tileview_get_tile_act(tileview);
    if(lv_tileview_get_tile_act(tileview) == lv_event_get_user_data(e)){
    	param.video_transmission_switch = 1;
    }else{
    	param.video_transmission_switch = 0;
    }

}

/*
 * 函数名：my_gui_test
 * 功能：一键创建5页带分隔线的选项卡UI界面
 * 知识点：Tabview创建、分隔线样式配置、对象尺寸设置、文本标签创建
 */
void my_gui_test(void)
{
        lv_obj_t * tileview = lv_tileview_create( lv_scr_act() );//在屏幕中创建平铺视图
        lv_obj_set_style_bg_color(tileview, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(tileview, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_t * tilevw0 = lv_tileview_add_tile( tileview, 0, 0, LV_DIR_RIGHT );
        lv_obj_t * tilevw1 = lv_tileview_add_tile( tileview, 1, 0, LV_DIR_ALL );
        lv_obj_t * tilevw2 = lv_tileview_add_tile( tileview, 2, 0, LV_DIR_ALL );
        lv_obj_t * tilevw3 = lv_tileview_add_tile( tileview, 3, 0, LV_DIR_ALL );
        lv_obj_t * tilevw4 = lv_tileview_add_tile( tileview, 4, 0, LV_DIR_LEFT );
        //lv_obj_t * tilevw5 = lv_tileview_add_tile( tileview, 5, 0, LV_DIR_LEFT );
        TileView_JoysticSetting(tilevw0);
        TileView_LcdBatterySetting(tilevw1);
        TileView_NRFConnectSetting(tilevw2);
        TileView_PidParamSetting(tilevw3);
        TileView_Ladar_Pointmap(tilevw4);
        //TileView_PID_Parameter(tilevw5);
        lv_obj_add_event_cb(tileview, tileview_event_cb, LV_EVENT_VALUE_CHANGED, tilevw4);

}

lvgl_parameter_t *GUI_GetParameter(void){
	return &param;
}

