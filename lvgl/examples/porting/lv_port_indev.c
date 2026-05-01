/**
 * @file lv_port_indev_templ.c
 *
 */

/*将此文件复制并重命名为 "lv_port_indev.c"，并将此值设置为 "1" 以启用内容*/
#if 1

/*********************
 *      头文件包含
 *********************/
#include "lv_port_indev.h"
#include "../../lvgl.h"
#include "main.h"
/*********************
 *      宏定义
 *********************/

/**********************
 *      类型定义
 **********************/
extern FT6336G_Device Touch_Screen;
/**********************
 *  静态函数声明
 **********************/

static void touchpad_init(void);                // 触摸屏初始化
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);  // 触摸屏数据读取回调
static bool touchpad_is_pressed(void);          // 判断触摸屏是否被按下
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y);  // 获取触摸屏按下位置的坐标


/**********************
 *  静态变量
 **********************/
lv_indev_t * indev_touchpad;    // 触摸屏输入设备句柄

/**********************
 *      宏
 **********************/

/**********************
 *   全局函数
 **********************/

void lv_port_indev_init(void)
{
    /**
     * 这里提供了 LVGL 支持的输入设备示例实现：
     *  - 触摸屏（Touchpad）
     *  - 鼠标（Mouse，支持光标显示）
     *  - 键盘（Keypad，仅通过按键操作GUI）
     *  - 编码器（Encoder，仅支持：左、右、按下 操作GUI）
     *  - 按键（Button，外部按键映射到屏幕指定坐标）
     *
     *  以下的 `..._read()` 函数仅为示例，
     *  你需要根据实际硬件适配修改
     */

    static lv_indev_drv_t indev_drv;  // 输入设备驱动描述符

    /*------------------
     * 触摸屏
     * -----------------*/

    /*初始化触摸屏（如果有）*/
    touchpad_init();

    /*注册触摸屏输入设备*/
    lv_indev_drv_init(&indev_drv);                // 初始化输入设备驱动
    indev_drv.type = LV_INDEV_TYPE_POINTER;       // 设备类型：指针类（触摸屏/鼠标）
    indev_drv.read_cb = touchpad_read;            // 设置数据读取回调函数
    indev_touchpad = lv_indev_drv_register(&indev_drv);  // 注册驱动并获取句柄


}

/**********************
 *   静态函数
 **********************/

/*------------------
 * 触摸屏
 * -----------------*/

/*初始化触摸屏硬件*/
static void touchpad_init(void)
{
	FT6336G_Init();/*在此处编写你的触摸屏初始化代码（如I2C/SPI配置、复位等）*/
}

/*LVGL 会调用此函数读取触摸屏数据*/
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static lv_coord_t last_x = 0;  // 上一次按下的X坐标
    static lv_coord_t last_y = 0;  // 上一次按下的Y坐标

    /*保存按下的坐标和状态*/
    if(touchpad_is_pressed()) {
        touchpad_get_xy(&last_x, &last_y);         // 获取当前按下坐标
        data->state = LV_INDEV_STATE_PR;           // 设置状态：按下
    }
    else {
        data->state = LV_INDEV_STATE_REL;           // 设置状态：释放
    }

    /*设置最后一次按下的坐标（即使释放也保留，用于LVGL跟踪）*/
    data->point.x = last_x;
    data->point.y = last_y;
}

/*判断触摸屏是否被按下，返回true表示按下*/
static bool touchpad_is_pressed(void)
{
    if(Touch_Screen.IRQ_Flag == FT6336G_TOUCH){
    	return true;/*在此处编写检测触摸屏按下的代码（如读取INT引脚电平、读取触摸点数量等）*/
    }

    return false;  // 默认返回未按下，需修改为实际检测逻辑
}

/*获取触摸屏按下位置的X/Y坐标（仅在按下时调用）*/
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y)
{
	FT6336G_GetCoordinate();
	(*x) = Touch_Screen.touchData.x[0];  // 默认X坐标，需修改为实际值
    (*y) = Touch_Screen.touchData.y[0];  // 默认Y坐标，需修改为实际值
}

#else /*在文件顶部启用此文件（将#if 0改为#if 1）*/

/*这个空的类型定义仅用于消除 -Wpedantic 编译警告。*/
typedef int keep_pedantic_happy;
#endif
