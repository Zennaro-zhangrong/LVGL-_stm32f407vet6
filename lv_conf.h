/**
 * @file lv_conf.h
 * LVGL v8.3.11 配置文件
 */

/*
 * 请将此文件复制并重命名为 `lv_conf.h`，然后放置在以下两个位置之一：
 * 1. 直接放在 `lvgl` 文件夹同级目录
 * 2. 放在任意自定义路径，并执行以下两步：
 *    - 定义宏 `LV_CONF_INCLUDE_SIMPLE`
 *    - 将自定义路径添加到编译器的头文件包含路径中
 */

/* clang-format off */
#if 1 /* 设置为1以启用本配置文件的内容 | 【修改建议】必须改为1，否则LVGL不会加载此配置 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   颜色设置
 *====================*/

/* 颜色深度：
 * 1 = 1字节/像素（单色）
 * 8 = RGB332 格式
 * 16 = RGB565 格式
 * 32 = ARGB8888 格式
 | 【修改建议】保留16，STM32F407处理RGB565效率最高，且适配ILI9341屏 */
#define LV_COLOR_DEPTH 16

/* 交换RGB565颜色的两个字节顺序。
 * 当显示屏使用8位接口（如SPI）时，此配置会很有用
 | 【修改建议】SPI接口屏改为1，并行16位接口屏保留0 */
#define LV_COLOR_16_SWAP 1

/* 启用透明背景绘制功能
 * 当使用不透明度（opa）、变换（transform_*）等样式属性时，必须启用此功能
 * 若UI界面需要叠加在其他图层（如OSD菜单、视频画面）之上，也需要启用
 | 【修改建议】改为1，40KB内存足够支撑，且能实现更丰富的UI效果 */
#define LV_COLOR_SCREEN_TRANSP 1

/* 调整颜色混合函数的舍入策略。不同GPU的颜色混合计算方式可能不同
 * 0 = 向下取整
 * 64 = 数值≥x.75时向上取整
 * 128 = 数值≥x.5时向上取整（四舍五入）
 * 192 = 数值≥x.25时向上取整
 * 254 = 强制向上取整
 | 【修改建议】保留0，对性能最优 */
#define LV_COLOR_MIX_ROUND_OFS 0

/* 色度键控颜色：图像中与此颜色相同的像素将不会被绘制
 | 【修改建议】保留纯绿色0x00ff00，是最常用的透明键值 */
#define LV_COLOR_CHROMA_KEY lv_color_hex(0x00ff00)         /* 纯绿色 */

/*=========================
   内存设置
 *=========================*/

/* 内存分配函数选择：
 * 1 = 使用自定义的malloc/free函数
 * 0 = 使用LVGL内置的 `lv_mem_alloc()` 和 `lv_mem_free()`
 | 【修改建议】保留0，嵌入式场景下LVGL内置内存管理更可控 */
#define LV_MEM_CUSTOM 0
#if LV_MEM_CUSTOM == 0
    /* LVGL内置内存池的大小（单位：字节），要求最小值≥2KB
 | 【修改建议】改为(40U * 1024U)，匹配项目40KB内存限制 */
    #define LV_MEM_SIZE (36U * 1024U)          /* 单位：字节 */

    /* 为内存池指定一个固定的物理地址（可以是外部SRAM地址）
     * 0 = 不使用固定地址，由LVGL自动分配
 | 【修改建议】保留0，使用STM32F407内部RAM即可满足需求 */
    #define LV_MEM_ADR 0x10007000     /* 0 = 未使用 */
    /* 替代固定地址的方案：定义一个内存分配函数，用于给LVGL分配内存池
     * 示例：my_malloc
     */
    #if LV_MEM_ADR == 0
        #undef LV_MEM_POOL_INCLUDE
        #undef LV_MEM_POOL_ALLOC
    #endif

#else       /* LV_MEM_CUSTOM == 1 分支 */
    #define LV_MEM_CUSTOM_INCLUDE <stdlib.h>   /* 自定义内存函数的头文件 | 【修改建议】无需修改，仅自定义内存时生效 */
    #define LV_MEM_CUSTOM_ALLOC   malloc
    #define LV_MEM_CUSTOM_FREE    free
    #define LV_MEM_CUSTOM_REALLOC realloc
#endif     /* LV_MEM_CUSTOM */

/* 渲染和内部处理使用的中间内存缓冲区最大数量
 * 若缓冲区数量不足，LVGL会输出错误日志
 | 【修改建议】改为8，减少内存占用，同时满足320*240屏的渲染需求 */
#define LV_MEM_BUF_MAX_NUM 8

/* 内存操作函数选择：
 * 1 = 使用标准库的 `memcpy` 和 `memset` 函数
 * 0 = 使用LVGL内置的内存操作函数
 * 两种方式的性能优劣取决于具体硬件平台
 | 【修改建议】改为1，STM32F4标准库的内存函数经过硬件优化，速度更快 */
#define LV_MEMCPY_MEMSET_STD 1

/*====================
   HAL 层设置
 *====================*/

/* 默认显示屏刷新周期（单位：毫秒）
 * LVGL会按照此周期，重绘屏幕上发生变化的区域
 | 【修改建议】保留30，30ms对应约33帧/秒，平衡流畅度与CPU占用 */
#define LV_DISP_DEF_REFR_PERIOD 50      /* 单位：毫秒 */

/* 输入设备的读取周期（单位：毫秒）
 | 【修改建议】保留30，对电容屏而言，此频率既灵敏又不占用过多CPU */
#define LV_INDEV_DEF_READ_PERIOD 30     /* 单位：毫秒 */

/* 自定义系统滴答时钟源：
 * 1 = 使用自定义的毫秒级时间源，无需手动调用 `lv_tick_inc()`
 * 0 = 禁用自定义滴答源，需在定时器中断中调用 `lv_tick_inc()`
 | 【修改建议】改为1，适配STM32 HAL库的系统滴答 */
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE "stm32f4xx_hal.h"         /* 系统时间函数的头文件 | 【修改建议】改为"stm32f4xx_hal.h" */
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (HAL_GetTick())    /* 获取当前系统时间的表达式 | 【修改建议】改为(HAL_GetTick()) */
    /* 若LVGL作为ESP32组件使用，可启用以下配置
    // #define LV_TICK_CUSTOM_INCLUDE "esp_timer.h"
    // #define LV_TICK_CUSTOM_SYS_TIME_EXPR ((esp_timer_get_time() / 1000LL))
    */
#endif   /* LV_TICK_CUSTOM */

/* 默认屏幕像素密度（单位：像素/英寸）
 * 用于初始化组件的默认尺寸、样式内边距等参数
 * 此配置影响较小，可根据实际屏幕效果微调
 | 【修改建议】改为120，更适配320*240的小尺寸屏幕 */
#define LV_DPI_DEF 142     /* 单位：像素/英寸 */

/*=======================
 * 功能特性配置
 *=======================*/

/*-------------
 * 绘制引擎
 *-----------*/

/* 启用复杂绘制引擎：
 * 启用后才能绘制阴影、渐变、圆角、圆形、圆弧、斜线、图像变换、蒙版等效果
 | 【修改建议】保留1，40KB内存可支撑，且能实现丰富UI效果 */
#define LV_DRAW_COMPLEX 1
#if LV_DRAW_COMPLEX != 0

    /* 启用阴影计算结果缓存
     * LV_SHADOW_CACHE_SIZE = 最大缓存的阴影尺寸（阴影宽度 + 圆角半径）
     * 缓存的内存开销 = 尺寸值的平方
 | 【修改建议】改为4，缓存小尺寸阴影，提升绘制效率且内存占用可忽略 */
    #define LV_SHADOW_CACHE_SIZE 8

    /* 设置缓存的圆形数据最大数量
     * 缓存的是1/4圆的周长数据，用于抗锯齿绘制
     * 每个圆形的内存开销 = 半径 × 4 字节
     * 缓存的是最常用的圆形半径
     * 0 = 禁用圆形缓存
 | 【修改建议】保留4，缓存常用圆形尺寸，提升绘制效率 */
    #define LV_CIRCLE_CACHE_SIZE 16
#endif /* LV_DRAW_COMPLEX */

/**
 * 简单图层配置：
 * 当组件的 `style_opa < 255` 时，会使用简单图层功能
 * 原理是将组件缓冲到一个图层中，再以指定透明度混合到屏幕上
 * 注意：`bg_opa`、`text_opa` 等属性不需要图层缓冲
 * 组件可以分块缓冲，避免占用过大的连续内存
 *
 * - LV_LAYER_SIMPLE_BUF_SIZE: 目标缓冲区大小（字节），LVGL会尝试分配此大小的内存
 * - LV_LAYER_SIMPLE_FALLBACK_BUF_SIZE: 备用缓冲区大小（字节），主缓冲区分配失败时使用
 *
 * 以上两个参数的单位均为字节
 * 注意：变换图层（使用transform_angle/zoom属性）占用更大内存且不能分块缓冲
 * 本配置仅影响带透明度的组件
 | 【修改建议】LV_LAYER_SIMPLE_BUF_SIZE改为(16*1024)，LV_LAYER_SIMPLE_FALLBACK_BUF_SIZE改为(2*1024)，减少内存占用 */
#define LV_LAYER_SIMPLE_BUF_SIZE          (32 * 1024)
#define LV_LAYER_SIMPLE_FALLBACK_BUF_SIZE (32 * 1024)

/* 默认图像缓存大小
 * 图像缓存的作用是保持图像处于打开状态，避免重复解码
 * 若仅使用LVGL内置图像格式，缓存无明显优势
 * 若使用PNG/JPG等复杂解码器，缓存可大幅提升效率，但会占用额外RAM
 * 0 = 禁用图像缓存
 | 【修改建议】改为2，缓存2个常用图像，平衡效率与内存 */
#define LV_IMG_CACHE_DEF_SIZE 2

/* 渐变支持的最大停止点数量
 * 增加此值可以实现更复杂的渐变效果
 * 每个额外的停止点会增加 (sizeof(lv_color_t) + 1) 字节的内存开销
 | 【修改建议】保留2，满足基础渐变需求，无需更高复杂度 */
#define LV_GRADIENT_MAX_STOPS 32

/* 默认渐变缓冲区大小（单位：字节）
 * LVGL计算渐变颜色映射时，可将结果缓存以避免重复计算
 * 若缓存大小不足，渐变映射仅在绘制时临时分配内存
 * 0 = 禁用渐变缓存
 | 【修改建议】改为512，缓存小尺寸渐变映射，提升绘制效率 */
#define LV_GRAD_CACHE_DEF_SIZE 512

/* 启用渐变抖动功能：
 * 用于在低色深屏幕上实现视觉上更平滑的渐变效果
 * 启用后会额外分配1~2行的渲染缓冲区
 * 内存开销 = 32位 × 组件宽度；若启用误差扩散抖动，额外增加 24位 × 组件宽度
 | 【修改建议】保留0，小屏视觉提升有限，且节省内存 */
#define LV_DITHER_GRADIENT 0
#if LV_DITHER_GRADIENT
    /* 启用误差扩散抖动：
     * 视觉效果更好，但会增加CPU占用和内存开销
     * 内存额外开销 = 24位 × 组件宽度
 | 【修改建议】保留0 */
    #define LV_DITHER_ERROR_DIFFUSION 0
#endif

/* 软件旋转的最大缓冲区大小（单位：字节）
 * 仅在显示驱动中启用软件旋转时生效
 | 【修改建议】改为(8*1024)，适配320*240屏旋转需求且节省内存 */
#define LV_DISP_ROT_MAX_BUF (8*1024)

/*-------------
 * GPU 硬件加速
 *-----------*/

/* 启用Arm 2D图形加速库
 | 【修改建议】保留0，STM32F407无专用2D GPU，启用无加速效果 */
#define LV_USE_GPU_ARM2D 0

/* 启用STM32 DMA2D（又称Chrom Art）硬件加速
 | 【修改建议】改为1，STM32F407内置DMA2D，可大幅提升绘制效率 */
#define LV_USE_GPU_STM32_DMA2D 0
#if LV_USE_GPU_STM32_DMA2D
    /* 必须定义目标芯片的CMSIS头文件路径
     * 示例："stm32f7xx.h" 或 "stm32f4xx.h"
 | 【修改建议】改为"stm32f4xx.h" */
    #define LV_GPU_DMA2D_CMSIS_INCLUDE
#endif

/* 启用RA6M3芯片的G2D GPU加速
 | 【修改建议】保留0，非目标芯片 */
#define LV_USE_GPU_RA6M3_G2D 0
#if LV_USE_GPU_RA6M3_G2D
    /* 目标芯片头文件路径
     * 示例："hal_data.h"
     */
    #define LV_GPU_RA6M3_G2D_INCLUDE "hal_data.h"
#endif

/* 启用SWM341芯片的DMA2D GPU加速
 | 【修改建议】保留0，非目标芯片 */
#define LV_USE_GPU_SWM341_DMA2D 0
#if LV_USE_GPU_SWM341_DMA2D
    #define LV_GPU_SWM341_DMA2D_INCLUDE "SWM341.h"
#endif

/* 启用NXP i.MX RT系列芯片的PXP GPU加速
 | 【修改建议】保留0，非目标芯片 */
#define LV_USE_GPU_NXP_PXP 0
#if LV_USE_GPU_NXP_PXP
    /* PXP初始化方式：
     * 1 = 自动添加裸机/FreeRTOS中断处理，在lv_init()时自动初始化
     *     注意：使用FreeRTOS时需定义宏 SDK_OS_FREE_RTOS
     * 0 = 手动调用 lv_gpu_nxp_pxp_init()，且必须在lv_init()之前执行
     */
    #define LV_USE_GPU_NXP_PXP_AUTO_INIT 0
#endif

/* 启用NXP i.MX RT系列芯片的VG-Lite GPU加速
 | 【修改建议】保留0，非目标芯片 */
#define LV_USE_GPU_NXP_VG_LITE 0

/* 启用SDL渲染器API加速
 | 【修改建议】保留0，嵌入式场景无需SDL */
#define LV_USE_GPU_SDL 0
#if LV_USE_GPU_SDL
    #define LV_GPU_SDL_INCLUDE_PATH <SDL2/SDL.h>
    /* 纹理缓存大小，默认8MB */
    #define LV_GPU_SDL_LRU_SIZE (1024 * 1024 * 8)
    /* 蒙版绘制的自定义混合模式，若链接旧版SDL2库，需禁用此功能 */
    #define LV_GPU_SDL_CUSTOM_BLEND_MODE (SDL_VERSION_ATLEAST(2, 0, 6))
#endif

/*-------------
 * 日志功能
 *-----------*/

/* 启用日志模块
 | 【修改建议】调试阶段改为1，发布阶段改回0 */
#define LV_USE_LOG 0
#if LV_USE_LOG

    /* 日志级别筛选：
     * LV_LOG_LEVEL_TRACE       最详细的日志，用于深度调试
     * LV_LOG_LEVEL_INFO        记录重要事件
     * LV_LOG_LEVEL_WARN        记录非致命警告
     * LV_LOG_LEVEL_ERROR       仅记录致命错误（系统可能崩溃）
     * LV_LOG_LEVEL_USER        仅记录用户自定义日志
     * LV_LOG_LEVEL_NONE        禁用所有日志
 | 【修改建议】保留LV_LOG_LEVEL_WARN，平衡日志量与调试需求 */
    #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

    /* 日志输出方式：
     * 1 = 使用标准库的 `printf` 函数输出
     * 0 = 用户需注册自定义回调函数 `lv_log_register_print_cb()`
 | 【修改建议】改为1，STM32可重定向printf到串口，方便调试 */
    #define LV_LOG_PRINTF 0

    /* 启用/禁用特定模块的TRACE级日志
     * 以下模块的TRACE日志输出量较大，可按需关闭
 | 【修改建议】保留1，调试时便于定位问题 */
    #define LV_LOG_TRACE_MEM        1
    #define LV_LOG_TRACE_TIMER      1
    #define LV_LOG_TRACE_INDEV      1
    #define LV_LOG_TRACE_DISP_REFR  1
    #define LV_LOG_TRACE_EVENT      1
    #define LV_LOG_TRACE_OBJ_CREATE 1
    #define LV_LOG_TRACE_LAYOUT     1
    #define LV_LOG_TRACE_ANIM       1

#endif  /* LV_USE_LOG */

/*-------------
 * 断言功能
 *-----------*/

/* 启用断言功能：检测操作失败或无效数据
 * 若启用了日志模块，断言失败时会输出错误日志
 | 【修改建议】保留默认值，关键断言启用，非关键禁用以节省性能 */
#define LV_USE_ASSERT_NULL          1   /* 检查参数是否为NULL（速度快，推荐启用） */
#define LV_USE_ASSERT_MALLOC        1   /* 检查内存分配是否成功（速度快，推荐启用） */
#define LV_USE_ASSERT_STYLE         1   /* 检查样式是否正确初始化（速度快，推荐启用） */
#define LV_USE_ASSERT_MEM_INTEGRITY 0   /* 检查内存池完整性（速度慢） */
#define LV_USE_ASSERT_OBJ           0   /* 检查对象类型和有效性（速度慢） */

/* 断言失败时的自定义处理函数
 * 示例：重启MCU
 | 【修改建议】保留默认的while(1)，触发断言时挂起系统，便于调试 */
#define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
#define LV_ASSERT_HANDLER while(1);   /* 默认挂起系统 */

/*-------------
 * 其他功能
 *-----------*/

/* 启用性能监视器：显示CPU使用率和FPS帧率
 | 【修改建议】调试阶段改为1，发布阶段改回0 */
#define LV_USE_PERF_MONITOR 0
#if LV_USE_PERF_MONITOR
    #define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT
#endif

/* 启用内存监视器：显示已用内存和内存碎片情况
 * 要求：必须使用LVGL内置内存管理（LV_MEM_CUSTOM = 0）
 | 【修改建议】调试阶段改为1，监控内存使用是否超出40KB限制 */
#define LV_USE_MEM_MONITOR 0
#if LV_USE_MEM_MONITOR
    #define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT
#endif

/* 启用重绘调试：在每次重绘的区域绘制随机颜色的矩形
 | 【修改建议】保留0，仅在调试重绘逻辑时启用 */
#define LV_USE_REFR_DEBUG 0

/* 替换LVGL内置的 (v)snprintf 函数
 | 【修改建议】保留0，嵌入式场景下LVGL内置函数更适配 */
#define LV_SPRINTF_CUSTOM 0
#if LV_SPRINTF_CUSTOM
    #define LV_SPRINTF_INCLUDE <stdio.h>
    #define lv_snprintf  snprintf
    #define lv_vsnprintf vsnprintf
#else   /* LV_SPRINTF_CUSTOM == 0 分支 */
    #define LV_SPRINTF_USE_FLOAT 0 /* 启用浮点数支持 | 【修改建议】保留0，节省内存 */
#endif  /* LV_SPRINTF_CUSTOM */

/* 启用用户数据功能：允许给LVGL对象附加自定义数据
 | 【修改建议】保留1，内存开销小，且能扩展组件功能 */
#define LV_USE_USER_DATA 1

/* 垃圾回收（GC）功能配置
 * 仅在LVGL绑定到高级语言（如Micropython）时使用，由高级语言管理内存
 | 【修改建议】保留0，STM32裸机/RTOS场景无需GC */
#define LV_ENABLE_GC 0
#if LV_ENABLE_GC != 0
    #define LV_GC_INCLUDE "gc.h"                           /* 包含GC相关头文件 */
#endif /* LV_ENABLE_GC */

/*=====================
 *  编译器设置
 *====================*/

/* 系统字节序设置：
 * 1 = 大端模式
 * 0 = 小端模式
 | 【修改建议】保留0，STM32F407是小端系统 */
#define LV_BIG_ENDIAN_SYSTEM 0

/* 为 `lv_tick_inc` 函数添加自定义编译器属性
 | 【修改建议】保留空，无需特殊属性 */
#define LV_ATTRIBUTE_TICK_INC

/* 为 `lv_timer_handler` 函数添加自定义编译器属性
 | 【修改建议】改为__attribute__((optimize("O0")))，调试时禁用优化，便于断点调试 */
#define LV_ATTRIBUTE_TIMER_HANDLER __attribute__((optimize("O2")))

/* 为 `lv_disp_flush_ready` 函数添加自定义编译器属性
 | 【修改建议】保留空，无需特殊属性 */
#define LV_ATTRIBUTE_FLUSH_READY

/* 缓冲区的强制对齐大小（单位：字节）
 | 【修改建议】保留1，无需强制对齐 */
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE 1

/* 内存对齐属性宏：
 * 用于给需要对齐的内存添加编译器属性
 * 示例：__attribute__((aligned(4)))
 * 注：在-Os优化级别下，数据可能不会默认按边界对齐
 | 【修改建议】保留默认，或根据需要改为__attribute__((aligned(4))) */
#define LV_ATTRIBUTE_MEM_ALIGN

/* 大型常量数组的属性宏：
 * 用于标记大型常量数组（如字体位图）
 | 【修改建议】改为__attribute__((section(".rodata")))，放入只读数据区，节省RAM */
#define LV_ATTRIBUTE_LARGE_CONST

/* RAM中大型数组的声明前缀：
 * 用于标记RAM中的大型数组
 | 【修改建议】改为__attribute__((section(".ram_data")))，便于内存分区管理 */
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY

/* 高性能函数的内存属性宏：
 * 用于将性能关键的函数放入高速内存（如RAM）
 | 【修改建议】改为__attribute__((section(".ram_func")))，提升执行速度 */
#define LV_ATTRIBUTE_FAST_MEM

/* GPU加速变量的属性宏：
 * 用于标记GPU加速操作中使用的变量
 * 这些变量通常需要放在DMA可访问的RAM区域
 | 【修改建议】改为__attribute__((section(".dma_ram")))，确保DMA2D可访问 */
#define LV_ATTRIBUTE_DMA

/* 常量导出宏：
 * 用于将LVGL常量导出到绑定层（如Micropython）
 * 格式：LV_<常量名>
 | 【修改建议】保留默认，无需导出 */
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning /* 默认值用于消除GCC警告 */

/* 启用大坐标支持：
 * 将坐标范围从默认的 -32768 ~ 32767 扩展到 -4194304 ~ 4194303
 * 实现方式：将坐标类型从int16_t改为int32_t
 | 【修改建议】保留0，320*240屏无需大坐标 */
#define LV_USE_LARGE_COORD 0

/*==================
 *   字体配置
 *===================*/

/* Montserrat字体族：ASCII字符集 + 部分符号，位深度bpp = 4
 * 字体来源：https://fonts.google.com/specimen/Montserrat
 | 【修改建议】仅启用项目需要的字体，其余保持0，节省Flash/RAM */
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 0
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 0
#define LV_FONT_MONTSERRAT_18 0
#define LV_FONT_MONTSERRAT_20 0
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 0
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 0

/* 特殊功能字体示例
 | 【修改建议】保持0，项目无特殊需求时禁用，节省资源 */
#define LV_FONT_MONTSERRAT_12_SUBPX      0 /* 亚像素渲染字体 */
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0  /* 压缩字体，bpp = 3 */
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0  /* 希伯来语、阿拉伯语、波斯语字体 */
#define LV_FONT_SIMSUN_16_CJK            0  /* 1000个常用中日韩汉字 */

/* 像素完美的等宽字体
 | 【修改建议】保持0，无等宽字体需求时禁用 */
#define LV_FONT_UNSCII_8  0
#define LV_FONT_UNSCII_16 0

/* 自定义字体声明：
 * 在此处声明自定义字体，可作为全局默认字体使用
 * 示例：#define LV_FONT_CUSTOM_DECLARE   LV_FONT_DECLARE(my_font_1) LV_FONT_DECLARE(my_font_2)
 | 【修改建议】无自定义字体时保持默认 */
#define LV_FONT_CUSTOM_DECLARE

/* 设置默认字体
 | 【修改建议】与启用的字体保持一致，此处为Montserrat-14 */
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/* 启用大型字体/多字符集字体支持：
 * 当字体尺寸大或字符集多时，需要启用此宏
 * 若编译器报错，说明需要启用此宏
 | 【修改建议】保持0，项目无大型字体需求 */
#define LV_FONT_FMT_TXT_LARGE 0

/* 启用压缩字体支持
 | 【修改建议】保持0，无压缩字体需求时禁用 */
#define LV_USE_FONT_COMPRESSED 0

/* 启用亚像素渲染功能
 | 【修改建议】保持0，小屏视觉提升有限 */
#define LV_USE_FONT_SUBPX 0
#if LV_USE_FONT_SUBPX
    /* 显示屏像素顺序：
     * 0 = RGB 顺序
     * 1 = BGR 顺序
     * 注：普通字体无需关注此配置
     */
    #define LV_FONT_SUBPX_BGR 0
#endif

/* 启用字形缺失时的占位符绘制
 | 【修改建议】保留1，提升用户体验 */
#define LV_USE_FONT_PLACEHOLDER 1

/*=================
 *  文本设置
 *=================*/

/**
 * 字符串编码格式选择：
 * 请确保IDE/编辑器的编码格式与此处一致
 * - LV_TXT_ENC_UTF8      UTF-8 编码（支持中文等多字节字符）
 * - LV_TXT_ENC_ASCII     ASCII 编码（仅支持英文字符）
 | 【修改建议】若需显示中文，改为LV_TXT_ENC_UTF8；否则保留默认 */
#define LV_TXT_ENC LV_TXT_ENC_UTF8

/* 文本换行符：文本遇到以下字符时会自动换行
 | 【修改建议】保留默认即可 */
#define LV_TXT_BREAK_CHARS " ,.;:-_"

/* 长单词强制换行阈值：
 * 当单词长度超过此值时，LVGL会在任意位置换行
 * 设置为≤0的值可禁用此功能
 | 【修改建议】保持0，禁用长单词强制换行 */
#define LV_TXT_LINE_BREAK_LONG_LEN 0

/* 长单词换行的前置最小字符数
 * 依赖于 LV_TXT_LINE_BREAK_LONG_LEN 配置
 | 【修改建议】保持默认 */
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3

/* 长单词换行的后置最小字符数
 * 依赖于 LV_TXT_LINE_BREAK_LONG_LEN 配置
 | 【修改建议】保持默认 */
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

/* 文本颜色控制符：用于在文本中插入颜色切换指令
 | 【修改建议】保留默认的'#' */
#define LV_TXT_COLOR_CMD "#"

/* 启用双向文本支持：
 * 允许混合显示从左到右（LTR）和从右到左（RTL）的文本
 * 遵循Unicode双向文本算法：
 * https://www.w3.org/International/articles/inline-bidi-markup/uba-basics
 | 【修改建议】保持0，无RTL语言需求时禁用 */
#define LV_USE_BIDI 0
#if LV_USE_BIDI
    /* 设置默认文本方向：
     * LV_BASE_DIR_LTR   从左到右
     * LV_BASE_DIR_RTL   从右到左
     * LV_BASE_DIR_AUTO  自动检测
     */
    #define LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_AUTO
#endif

/* 启用阿拉伯语/波斯语字符处理：
 * 这些语言的字符会根据在文本中的位置自动切换形态
 | 【修改建议】保持0，无相关语言需求时禁用 */
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/*==================
 *  组件配置
 *================*/

/* 组件文档：https://docs.lvgl.io/latest/en/html/widgets/index.html
 | 【修改建议】仅启用项目需要的组件，其余保持0，节省Flash/RAM */
#define LV_USE_ARC        1 /* 圆弧组件 */

#define LV_USE_BAR        1 /* 进度条组件 */

#define LV_USE_BTN        1 /* 按钮组件 */

#define LV_USE_BTNMATRIX  1 /* 按钮矩阵组件 */

#define LV_USE_CANVAS     1 /* 画布组件 */

#define LV_USE_CHECKBOX   1 /* 复选框组件 */

#define LV_USE_DROPDOWN   1 /* 下拉列表组件 | 依赖：lv_label */

#define LV_USE_IMG        1 /* 图像组件 | 依赖：lv_label */

#define LV_USE_LABEL      1 /* 标签组件 */
#if LV_USE_LABEL
    #define LV_LABEL_TEXT_SELECTION 1 /* 启用标签文本选择功能 | 【修改建议】保留1 */
    #define LV_LABEL_LONG_TXT_HINT 1  /* 长文本优化：存储额外信息提升绘制速度 | 【修改建议】保留1 */
#endif

#define LV_USE_LINE       1 /* 线条组件 */

#define LV_USE_ROLLER     1 /* 滚轮组件 | 依赖：lv_label */
#if LV_USE_ROLLER
    #define LV_ROLLER_INF_PAGES 7 /* 无限滚轮的额外页面数 | 【修改建议】保留7 */
#endif

#define LV_USE_SLIDER     1 /* 滑动条组件 | 依赖：lv_bar */

#define LV_USE_SWITCH     1 /* 开关组件 */

#define LV_USE_TEXTAREA   1 /* 文本框组件 | 依赖：lv_label */
#if LV_USE_TEXTAREA != 0
    #define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500    /* 密码显示时长（毫秒） | 【修改建议】保留1500 */
#endif

#define LV_USE_TABLE      1 /* 表格组件 */

/*==================
 * 扩展组件配置
 *==================*/

/*-----------
 * 扩展组件
 *----------*/
#define LV_USE_ANIMIMG    1 /* 动画图片组件 | 【修改建议】按需启用，无需时改为0 */

#define LV_USE_CALENDAR   1 /* 日历组件 | 【修改建议】按需启用，无需时改为0 */
#if LV_USE_CALENDAR
    #define LV_CALENDAR_WEEK_STARTS_MONDAY 0 /* 星期一开始一周 | 【修改建议】保留0，周日开始 */
    #if LV_CALENDAR_WEEK_STARTS_MONDAY
        #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"}
    #else
        #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"}
    #endif

    #define LV_CALENDAR_DEFAULT_MONTH_NAMES {"January", "February", "March",  "April", "May",  "June", "July", "August", "September", "October", "November", "December"}
    #define LV_USE_CALENDAR_HEADER_ARROW 1 /* 启用箭头式日历头 | 【修改建议】保留1 */
    #define LV_USE_CALENDAR_HEADER_DROPDOWN 1 /* 启用下拉式日历头 | 【修改建议】保留1 */
#endif  /* LV_USE_CALENDAR */

#define LV_USE_CHART      1 /* 图表组件 | 【修改建议】按需启用，无需时改为0 */

#define LV_USE_COLORWHEEL 1 /* 颜色选择器组件 | 【修改建议】按需启用，无需时改为0 */

#define LV_USE_IMGBTN     1 /* 图像按钮组件 | 【修改建议】按需启用，无需时改为0 */

#define LV_USE_KEYBOARD   1 /* 键盘组件 | 【修改建议】按需启用，无需时改为0 */

#define LV_USE_LED        1 /* LED组件 | 【修改建议】按需启用，无需时改为0 */

#define LV_USE_LIST       1 /* 列表组件 | 【修改建议】按需启用，无需时改为0 */

#define LV_USE_MENU       1 /* 菜单组件 | 【修改建议】按需启用，无需时改为0 */

#define LV_USE_METER      1 /* 仪表盘组件 | 【修改建议】按需启用，无需时改为0 */

#define LV_USE_MSGBOX     1 /* 消息框组件 | 【修改建议】按需启用，无需时改为0 */

#define LV_USE_SPAN       1 /* 富文本组件 | 【修改建议】按需启用，无需时改为0 */
#if LV_USE_SPAN
    /* 单行文本支持的最大span描述符数量
     */
    #define LV_SPAN_SNIPPET_STACK_SIZE 64
#endif

#define LV_USE_SPINBOX    1 /* 数字输入框组件 | 【修改建议】按需启用，无需时改为0 */

#define LV_USE_SPINNER    1 /* 加载动画组件 | 【修改建议】按需启用，无需时改为0 */

#define LV_USE_TABVIEW    1 /* 标签页组件 | 【修改建议】按需启用，无需时改为0 */

#define LV_USE_TILEVIEW   1 /* 瓦片视图组件 | 【修改建议】按需启用，无需时改为0 */

#define LV_USE_WIN        1 /* 窗口组件 | 【修改建议】按需启用，无需时改为0 */

/*-----------
 * 主题配置
 *----------*/

/* 启用默认主题：功能完整、视觉效果好
 | 【修改建议】保留1，快速实现美观UI */
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT

    /* 主题亮度模式：
     * 0 = 亮色模式
     * 1 = 暗色模式
 | 【修改建议】按需选择，小屏亮色模式更清晰 */
    #define LV_THEME_DEFAULT_DARK 0

    /* 启用按钮按下时的放大效果
 | 【修改建议】保留1，提升交互体验 */
    #define LV_THEME_DEFAULT_GROW 1

    /* 默认过渡动画时长（单位：毫秒）
 | 【修改建议】保留80，平衡流畅度与响应速度 */
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif /* LV_USE_THEME_DEFAULT */

/* 启用基础主题：极简风格，适合作为自定义主题的基础
 | 【修改建议】保持0，使用默认主题即可 */
#define LV_USE_THEME_BASIC 0

/* 启用单色主题：专为单色屏设计
 | 【修改建议】保持0，ILI9341是彩色屏 */
#define LV_USE_THEME_MONO 0

/*-----------
 * 布局管理器
 *----------*/

/* 启用Flex布局：类似CSS的Flexbox布局
 | 【修改建议】保留1，灵活的布局方式 */
#define LV_USE_FLEX 1

/* 启用Grid布局：类似CSS的Grid布局
 | 【修改建议】保留1，适合复杂网格布局 */
#define LV_USE_GRID 1

/*---------------------
 * 第三方库集成
 *--------------------*/

/* 文件系统接口配置 */

/* 标准C文件系统接口（fopen/fread等）
 | 【修改建议】无文件系统需求时保持0 */
#define LV_USE_FS_STDIO 0
#if LV_USE_FS_STDIO
    #define LV_FS_STDIO_LETTER '\0'     /* 驱动器盘符（大写字母，如'A'） */
    #define LV_FS_STDIO_PATH ""         /* 工作目录，文件路径会附加在此目录后 */
    #define LV_FS_STDIO_CACHE_SIZE 0    /* 读缓存大小，>0启用缓存 */
#endif

/* POSIX文件系统接口（open/read等）
 | 【修改建议】无文件系统需求时保持0 */
#define LV_USE_FS_POSIX 0
#if LV_USE_FS_POSIX
    #define LV_FS_POSIX_LETTER '\0'     /* 驱动器盘符 */
    #define LV_FS_POSIX_PATH ""         /* 工作目录 */
    #define LV_FS_POSIX_CACHE_SIZE 0    /* 读缓存大小 */
#endif

/* Windows文件系统接口（CreateFile/ReadFile等）
 | 【修改建议】嵌入式场景保持0 */
#define LV_USE_FS_WIN32 0
#if LV_USE_FS_WIN32
    #define LV_FS_WIN32_LETTER '\0'     /* 驱动器盘符 */
    #define LV_FS_WIN32_PATH ""         /* 工作目录 */
    #define LV_FS_WIN32_CACHE_SIZE 0    /* 读缓存大小 */
#endif

/* FATFS文件系统接口（需单独添加FATFS库）
 | 【修改建议】使用SD卡时改为1，否则保持0 */
#define LV_USE_FS_FATFS 0
#if LV_USE_FS_FATFS
    #define LV_FS_FATFS_LETTER '\0'     /* 驱动器盘符 */
    #define LV_FS_FATFS_CACHE_SIZE 0    /* 读缓存大小 */
#endif

/* LittleFS文件系统接口（需单独添加LittleFS库）
 | 【修改建议】无需求时保持0 */
#define LV_USE_FS_LITTLEFS 0
#if LV_USE_FS_LITTLEFS
    #define LV_FS_LITTLEFS_LETTER '\0'     /* 驱动器盘符 */
    #define LV_FS_LITTLEFS_CACHE_SIZE 0    /* 读缓存大小 */
#endif

/* PNG图片解码器
 | 【修改建议】需要显示PNG时改为1，否则保持0 */
#define LV_USE_PNG 0

/* BMP图片解码器
 | 【修改建议】需要显示BMP时改为1，否则保持0 */
#define LV_USE_BMP 0

/* JPG + 分割JPG解码器
 * 分割JPG是LVGL自定义的嵌入式优化格式
 | 【修改建议】需要显示JPG时改为1，否则保持0 */
#define LV_USE_SJPG 0

/* GIF动图解码器
 | 【修改建议】需要显示GIF时改为1，否则保持0 */
#define LV_USE_GIF 0

/* QR码生成库
 | 【修改建议】需要生成QR码时改为1，否则保持0 */
#define LV_USE_QRCODE 0

/* FreeType字体引擎：支持TrueType/OpenType字体
 | 【修改建议】无复杂字体需求时保持0 */
#define LV_USE_FREETYPE 0
#if LV_USE_FREETYPE
    /* FreeType字符缓存大小（字节），-1禁用缓存 */
    #define LV_FREETYPE_CACHE_SIZE (16 * 1024)
    #if LV_FREETYPE_CACHE_SIZE >= 0
        /* 位图缓存类型：
         * 1 = 使用sbit缓存（小字体<256px时更省内存）
         * 0 = 使用图像缓存（字体≥256px时必须用此模式）
         */
        #define LV_FREETYPE_SBIT_CACHE 0
        /* 缓存的FT_Face/FT_Size对象最大数量
         * 0 = 使用系统默认值
         */
        #define LV_FREETYPE_CACHE_FT_FACES 0
        #define LV_FREETYPE_CACHE_FT_SIZES 0
    #endif
#endif

/* Tiny TTF字体引擎：轻量级TrueType字体引擎
 | 【修改建议】无复杂字体需求时保持0 */
#define LV_USE_TINY_TTF 0
#if LV_USE_TINY_TTF
    /* 启用从文件加载TTF字体 */
    #define LV_TINY_TTF_FILE_SUPPORT 0
#endif

/* Rlottie动画库：支持Lottie动画
 | 【修改建议】无动画需求时保持0 */
#define LV_USE_RLOTTIE 0

/* FFmpeg库：支持多种图片解码和视频播放
 * 注意：启用后请禁用其他图片解码器
 | 【修改建议】无视频需求时保持0 */
#define LV_USE_FFMPEG 0
#if LV_USE_FFMPEG
    /* 输出输入信息到标准错误流 */
    #define LV_FFMPEG_DUMP_FORMAT 0
#endif

/*-----------
 * 其他扩展功能
 *----------*/

/* 启用对象截图功能：导出LVGL对象为图像数据
 | 【修改建议】调试阶段改为1，发布阶段改回0 */
#define LV_USE_SNAPSHOT 0

/* 启用猴子测试功能：随机点击屏幕，用于稳定性测试
 | 【修改建议】测试阶段改为1，发布阶段改回0 */
#define LV_USE_MONKEY 0

/* 启用网格导航功能：支持键盘/编码器的网格状导航
 | 【修改建议】有物理按键时改为1，否则保持0 */
#define LV_USE_GRIDNAV 0

/* 启用对象碎片功能：将复杂UI拆分为可复用的碎片
 | 【修改建议】复杂UI项目改为1，否则保持0 */
#define LV_USE_FRAGMENT 0

/* 启用图像字体功能：支持在标签/富文本中使用图像作为字体
 | 【修改建议】无需求时保持0 */
#define LV_USE_IMGFONT 1

/* 启用发布-订阅消息系统
 | 【修改建议】复杂UI项目改为1，否则保持0 */
#define LV_USE_MSG 0

/* 启用拼音输入法
 * 依赖：lv_keyboard组件
 | 【修改建议】需要中文输入时改为1，否则保持0 */
#define LV_USE_IME_PINYIN 0
#if LV_USE_IME_PINYIN
    /* 启用默认词库
     * 若不使用默认词库，需在使用lv_ime_pinyin前手动设置词库
     */
    #define LV_IME_PINYIN_USE_DEFAULT_DICT 1
    /* 候选词显示的最大数量
     * 根据屏幕尺寸调整
     */
    #define LV_IME_PINYIN_CAND_TEXT_NUM 6

    /* 启用9键输入模式
     */
    #define LV_IME_PINYIN_USE_K9_MODE      1
    #if LV_IME_PINYIN_USE_K9_MODE == 1
        #define LV_IME_PINYIN_K9_CAND_TEXT_NUM 3
    #endif // LV_IME_PINYIN_USE_K9_MODE
#endif

/*==================
* 示例代码配置
*==================*/

/* 启用LVGL示例代码编译
 | 【修改建议】学习阶段改为1，项目开发阶段改回0，节省编译时间 */
#define LV_BUILD_EXAMPLES 0

/*===================
 * 演示程序配置
 ====================*/

/* 启用组件演示程序：展示所有LVGL组件
 * 注意：可能需要增大LV_MEM_SIZE
 | 【修改建议】学习阶段改为1，项目开发阶段改回0 */
#define LV_USE_DEMO_WIDGETS 0
#if LV_USE_DEMO_WIDGETS
#define LV_DEMO_WIDGETS_SLIDESHOW 0 /* 启用幻灯片模式 */
#endif

/* 启用键盘和编码器演示程序
 | 【修改建议】学习阶段改为1，项目开发阶段改回0 */
#define LV_USE_DEMO_KEYPAD_AND_ENCODER 0

/* 启用性能基准演示程序：测试系统性能
 | 【修改建议】性能测试阶段改为1，项目开发阶段改回0 */
#define LV_USE_DEMO_BENCHMARK 0
#if LV_USE_DEMO_BENCHMARK
/* 使用RGB565A8格式图片，搭配16位色深 */
#define LV_DEMO_BENCHMARK_RGB565A8 0
#endif

/* 启用压力测试演示程序：测试LVGL稳定性
 | 【修改建议】稳定性测试阶段改为1，项目开发阶段改回0 */
#define LV_USE_DEMO_STRESS 0

/* 启用音乐播放器演示程序
 | 【修改建议】学习阶段改为1，项目开发阶段改回0 */
#define LV_USE_DEMO_MUSIC 0
#if LV_USE_DEMO_MUSIC
    #define LV_DEMO_MUSIC_SQUARE    0 /* 方形界面 */
    #define LV_DEMO_MUSIC_LANDSCAPE 0 /* 横屏界面 */
    #define LV_DEMO_MUSIC_ROUND     0 /* 圆形界面 */
    #define LV_DEMO_MUSIC_LARGE     0 /* 大尺寸界面 */
    #define LV_DEMO_MUSIC_AUTO_PLAY 0 /* 自动播放 */
#endif





/*****           新添加的宏定义         **/


/* ========== 手动追加：Tileview 滑动/吸附 灵敏优化参数 LVGL8.3.11 ========== */
/* 1. 滑动最小触发距离：越小越灵敏 */
#define LV_SCROLL_THROW_THRESHOLD      3
/* 2. 惯性滑动最低速度门槛：轻滑也能触发惯性 */
#define LV_SCROLL_THROW_SPEED          30
/* 3. 滚动吸附动画时长：数值越小 切页越快、不拖沓 */
#define LV_SCROLL_ANIM_TIME            150
/* 4. 分页吸附判定阈值：数值越小 划一点点就自动翻页 */
#define LV_SCROLL_SNAP_THRESHOLD       10
/* 5. 关闭边缘回弹：手感更干净利落 */
#define LV_SCROLL_BOUNCE               0





/*--END OF LV_CONF_H--*/

#endif /*LV_CONF_H*/

#endif /*配置内容启用开关*/
