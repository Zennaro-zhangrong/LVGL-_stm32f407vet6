/**
 * @file lv_port_disp_templ.c
 *
 */

/*将此文件复制并重命名为 "lv_port_disp.c"，并将此值设置为 "1" 以启用内容*/
#if 1

/*********************
 *      头文件包含
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>
#include "../../../Screen_Driver/LCD.h"
#include "main.h"
/*********************
 *      宏定义
 *********************/
#ifndef MY_DISP_HOR_RES
    #define MY_DISP_HOR_RES    320  // 显示屏水平分辨率（宽度）
#endif

#ifndef MY_DISP_VER_RES
    #define MY_DISP_VER_RES    240  // 显示屏垂直分辨率（高度）
#endif

/**********************
 *      类型定义
 **********************/

/**********************
 *  静态函数声明
 **********************/
static void disp_init(void);  // 显示屏硬件初始化

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);  // 屏幕刷新回调函数


/**********************
 *  静态变量
 **********************/

/**********************
 *      宏
 **********************/

/**********************
 *   全局函数
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * 初始化显示屏硬件
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * 创建绘图缓冲区
     *----------------------------*/

    /**
     * LVGL 需要一个缓冲区来在内部绘制UI组件。
     * 之后该缓冲区的内容会通过显示驱动的 `flush_cb` 回调函数复制到实际的显示屏上。
     * 缓冲区的大小必须大于显示屏的1行像素
     *
     * 有3种缓冲区配置方式：
     * 1. 创建单个缓冲区：
     *      LVGL 会在此缓冲区绘制屏幕内容，然后将其写入显示屏
     *
     * 2. 创建两个缓冲区：
     *      LVGL 会在一个缓冲区绘制屏幕内容并发送到显示屏。
     *      建议使用DMA将缓冲区内容写入显示屏，这样LVGL可以在第一个缓冲区的数据发送过程中，
     *      同时在第二个缓冲区绘制屏幕的下一部分内容，实现渲染和刷新并行处理。
     *
     * 3. 双缓冲模式
     *      设置两个屏幕尺寸大小的缓冲区，并将 disp_drv.full_refresh = 1。
     *      这种模式下LVGL会在 `flush_cb` 中始终提供完整的渲染屏幕数据，
     *      你只需要切换帧缓冲区的地址即可。
     */

    /* 示例1) 单缓冲区配置 */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
    static lv_color_t buf_1[MY_DISP_HOR_RES * 76] __attribute__((section(".lvgl_buf")));                          /*存储48行像素的缓冲区*/
    static lv_color_t buf_2[MY_DISP_HOR_RES * 76] __attribute__((section(".lvgl_buf")));                          /*存储48行像素的缓冲区*/
 	lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, buf_2, MY_DISP_HOR_RES * 76);   /*初始化显示缓冲区*/


    /*-----------------------------------
     * 向LVGL注册显示驱动
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                         /*显示驱动描述符*/
    lv_disp_drv_init(&disp_drv);                    /*基础初始化*/

    /*配置访问显示屏的相关函数*/

    /*设置显示屏分辨率*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*设置用于将缓冲区内容复制到显示屏的回调函数*/
    disp_drv.flush_cb = disp_flush;



    /*设置显示缓冲区*/
    disp_drv.draw_buf = &draw_buf_dsc_1;



    /*最终注册驱动*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   静态函数
 **********************/

/*初始化显示屏及所需的外设*/
static void disp_init(void)
{
    LCD_Init();/*在此处编写你的初始化代码*/
}

volatile bool disp_flush_enabled = true;  // 屏幕刷新使能标志

/* 当LVGL调用disp_flush()时，启用屏幕更新（刷新过程）
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* 当LVGL调用disp_flush()时，禁用屏幕更新（刷新过程）
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*将内部缓冲区中指定区域的内容刷新到显示屏上
 *你可以使用DMA或任何硬件加速来在后台执行此操作，但
 *操作完成后必须调用 'lv_disp_flush_ready()' 通知LVGL。*/;
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    if(disp_flush_enabled) {

    	//将要发送的信息写入队列,并返回写入后的队列数量
    	uint8_t spi_queue_sum = SPI_Transmission_ENQueue(disp_drv, area, color_p);
    	//用于空队列启动，此时队列中的信息为上一行入队的信息
    	if(spi_queue_sum == 1){
    		LCD_Apply_ScreenFilling();//立即申请一次SPI填充
    	}
    }
    //lv_disp_flush_ready(disp_drv);在spi回调时才能使用此函数
    /*重要！！！
     *通知图形库你已完成刷新操作*/
}




#else /*在文件顶部启用此文件*/

/*这个空的类型定义仅用于消除 -Wpedantic 编译警告。*/
typedef int keep_pedantic_happy;
#endif
