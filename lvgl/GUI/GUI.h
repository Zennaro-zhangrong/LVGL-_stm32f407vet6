#ifndef MY_GUI_H_
#define MY_GUI_H_
#include "../lvgl.h"
#include "../../Hardware/UART.h"
typedef struct {
	uint8_t Screen_Brightness;//屏幕亮度，1~F
    unsigned char Joystick_SwitchL;//摇杆左侧按下的次数
    unsigned char Joystick_SwitchR;//摇杆右侧按下的次数
    short Joystick_LX;//左摇杆X值，-255 ~ 255，左负右正，下负上正
    short Joystick_LY;
    short Joystick_RX;
    short Joystick_RY;
    char Joystick_SwitchRudder;//右手舵开关
    char Joystick_SwitchLoop;//速度闭环开关
    unsigned char Battery_Level;//电池电量 %0~99%
    float Battery_Voltage;//电池电压 Vol
    unsigned char MAX_SpeedSet;//设置小车的速度增益系数
    unsigned char NRF_ConnectState;//天线的连接状态
    unsigned short NRF_realRate;//实际的秒均传输速率
    unsigned short NRF_realDelay;//实际的通信延迟
    unsigned char PID_Channel;//PID参数通道
    unsigned short *PID_Buffer;//PID参数地址
    unsigned char video_transmission_switch;//图传开关
}lvgl_parameter_t;

extern lvgl_parameter_t param;
void my_gui_test(void);
void TileView_JoysticSetting(lv_obj_t * tileview);
void TileView_PidParamSetting(lv_obj_t * tileview);
void TileView_LcdBatterySetting(lv_obj_t * tileview);
void TileView_NRFConnectSetting(lv_obj_t *tileview);
void lvgl_battery_data_update(unsigned char Battery_level, float Battery_Voltage);
void Joystick_Swich_EXTI_Callback(uint16_t GPIO_Pin);
void TileView_PID_Parameter(lv_obj_t *    tileview );
void TileView_Ladar_Pointmap(lv_obj_t *tileview );
void NRF_ConnectState(uint8_t state);
void NRF_TransmissionStatus_Update(void);
lvgl_parameter_t *GUI_GetParameter(void);
#endif

