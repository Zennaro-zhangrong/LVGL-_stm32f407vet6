/*
 * TileView_Parameter_List.c
 *
 *  Created on: 2026年4月27日
 *      Author: zr186
 */
#include "GUI.h"
void TileView_PID_Parameter(lv_obj_t *tileview ){

     lv_obj_t * label1 = lv_label_create(tileview);
     lv_label_set_text(label1, "<  Overview of PID Parameters  >");
     lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 0);


    lv_obj_t * list = lv_list_create(tileview);
    lv_obj_set_size(list, 270, 205);
    lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, -15);


     lv_obj_t * label2 = lv_label_create(list);
     lv_label_set_text_fmt(label2, "PID_A: [  %01d.%02d  ] [  %01d.%02d  ] [  %01d.%02d  ]\r\n",2,22,1,11,3,33);
     lv_obj_set_width(label2,lv_pct(100));

     lv_obj_t * label3 = lv_label_create(list);
     lv_label_set_text_fmt(label3, "PID_B: [  %01d.%02d  ] [  %01d.%02d  ] [  %01d.%02d  ]\r\n",2,22,1,11,3,33);
     lv_obj_set_width(label3,lv_pct(100));
     lv_obj_t * label4 = lv_label_create(list);
     lv_label_set_text_fmt(label4, "PID_C: [   %01d.%02d  ] [  %01d.%02d   ] [  %01d.%02d  ]\r\n",2,22,1,11,3,33);
     lv_obj_set_width(label4,lv_pct(100));
     lv_obj_t * label5 = lv_label_create(list);
     lv_label_set_text_fmt(label5, "PID_D: [   %01d.%02d  ] [  %01d.%02d  ] [  %01d.%02d  ]\r\n",2,22,1,11,3,33);
     lv_obj_set_width(label5,lv_pct(100));
     lv_obj_t * label6 = lv_label_create(list);
     lv_label_set_text_fmt(label6, "PID_E: [   %01d.%02d  ] [  %01d.%02d  ] [  %01d.%02d  ]\r\n",2,22,1,11,3,33);
     lv_obj_set_width(label6,lv_pct(100));
    lv_obj_t * label7 = lv_label_create(list);
     lv_label_set_text_fmt(label7, "PID_F: [   %01d.%02d  ] [  %01d.%02d  ] [  %01d.%02d  ]\r\n",2,22,1,11,3,33);
     lv_obj_set_width(label7,lv_pct(100));








}




