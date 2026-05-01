/*
 * LCD.c
 *
 *  Created on: Jan 8, 2026
 *      Author: zr186
 */

#include "LCD.h"

#include "spi.h"
#include "i2c.h"
#include "tim.h"
#include "main.h"
#include "gpio.h"
#include "../Hardware/UART.h"


extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim4;

LCD_DevTypeDef LCD_Device;//创建唯一屏幕实例

static void LCD_Display_Command(uint8_t command);
static void LCD_Display_Data(uint8_t data);
static void LCD_Reset(void);
static void LCD_Direction(uint8_t direction);
static SPI_send_queue SPI_QueueA = {0};
static SPI_send_queue SPI_QueueB = {0};

/*
 * @功能：LCD初始化
 * 流程：开启背光 → 硬件复位 → 设置显示方向 → ILI9341寄存器配置 → 退出睡眠 → 开启显示
 * */
void LCD_Init(void){
    /********** 第一步：开启LCD背光（已实现，保留并优化） **********/
    if (HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1) != HAL_OK){
    	log_printf("LCD backlight failed to turn on\r\n");
    }
    LCD_Device.Brightness = 0x0F;// 设置最大亮度
    LCD_Brightness(0x00);
    log_printf("LCD backlight turned on successfully\r\n");

    /********** 第二步：LCD硬件复位（调用已实现的LCD_Reset） **********/
    LCD_Reset();
    HAL_Delay(400); // 复位后延时，确保芯片电源稳定

    /********** 第三步：ILI9341寄存器初始化序列（核心） **********/
    // 电源控制相关寄存器（稳定电源供应）
    LCD_Display_Command(0xCF);
    LCD_Display_Data(0x00);
    LCD_Display_Data(0xC1);
    LCD_Display_Data(0x30);

    LCD_Display_Command(0xED);
    LCD_Display_Data(0x64);
    LCD_Display_Data(0x03);
    LCD_Display_Data(0X12);
    LCD_Display_Data(0X81);

    LCD_Display_Command(0xE8);
    LCD_Display_Data(0x85);
    LCD_Display_Data(0x00);
    LCD_Display_Data(0x78);

    LCD_Display_Command(0xCB);
    LCD_Display_Data(0x39);
    LCD_Display_Data(0x2C);
    LCD_Display_Data(0x00);
    LCD_Display_Data(0x34);
    LCD_Display_Data(0x02);

    LCD_Display_Command(0xF7);
    LCD_Display_Data(0x20);

    LCD_Display_Command(0xEA);
    LCD_Display_Data(0x00);
    LCD_Display_Data(0x00);

    LCD_Display_Command(0xC0);       //电源控制
    LCD_Display_Data(0x13);     //VRH[5:0] 电压调节高字节位段

    LCD_Display_Command(0xC1);       //电源控制
    LCD_Display_Data(0x13);     //SAP[2:0]辅助电源位段；BT[3:0]亮度调节位段

    LCD_Display_Command(0xC5);       //VCM电压控制
    LCD_Display_Data(0x22);   //22
    LCD_Display_Data(0x35);   //35

    LCD_Display_Command(0xC7);       //VCM电压控制2
    LCD_Display_Data(0xBD);  //AF

    LCD_Display_Command(0x21);

    LCD_Display_Command(0x36);       //存储器访问控制
    LCD_Display_Data(0x08);

    LCD_Display_Command(0xB6);
    LCD_Display_Data(0x0A);
    LCD_Display_Data(0xA2);

    LCD_Display_Command(0x3A);
    LCD_Display_Data(0x55);

    LCD_Display_Command(0xF6);  //接口控制
    LCD_Display_Data(0x01);
    LCD_Display_Data(0x30);  //MCU接口配置

    LCD_Display_Command(0xB1);       //VCM控制
    LCD_Display_Data(0x00);
    LCD_Display_Data(0x1B);

    LCD_Display_Command(0xF2);       //三阶伽马功能禁用
    LCD_Display_Data(0x00);

    LCD_Display_Command(0x26);       //选择伽马曲线
    LCD_Display_Data(0x01);

    LCD_Display_Command(0xE0);       //设置伽马参数
    LCD_Display_Data(0x0F);
    LCD_Display_Data(0x35);
    LCD_Display_Data(0x31);
    LCD_Display_Data(0x0B);
    LCD_Display_Data(0x0E);
    LCD_Display_Data(0x06);
    LCD_Display_Data(0x49);
    LCD_Display_Data(0xA7);
    LCD_Display_Data(0x33);
    LCD_Display_Data(0x07);
    LCD_Display_Data(0x0F);
    LCD_Display_Data(0x03);
    LCD_Display_Data(0x0C);
    LCD_Display_Data(0x0A);
    LCD_Display_Data(0x00);

    LCD_Display_Command(0XE1);       //设置伽马参数
    LCD_Display_Data(0x00);
    LCD_Display_Data(0x0A);
    LCD_Display_Data(0x0F);
    LCD_Display_Data(0x04);
    LCD_Display_Data(0x11);
    LCD_Display_Data(0x08);
    LCD_Display_Data(0x36);
    LCD_Display_Data(0x58);
    LCD_Display_Data(0x4D);
    LCD_Display_Data(0x07);
    LCD_Display_Data(0x10);
    LCD_Display_Data(0x0C);
    LCD_Display_Data(0x32);
    LCD_Display_Data(0x34);
    LCD_Display_Data(0x0F);

    LCD_Display_Command(0x11);       //退出睡眠模式
    HAL_Delay(120);
    LCD_Display_Command(0x29);       //开启显示


    /********** 第四步：设置显示方向 **********/
    LCD_Direction(3);
    /***********第五步：配置操作命令**************/
    LCD_Device.W_Ram = 0x2C;
    LCD_Device.SetX = 0x2A;
    LCD_Device.SetY = 0x2B;
    LCD_Device.LCD_TX_Flag = Screen_OK;
    SPI_QueueA.Queue_sum = 0;//初始化SPI发送队列
    SPI_QueueB.Queue_sum = 0;//初始化SPI发送队列
    log_printf("LCD initialization completed!\r\n");
}


/*
 * @功能：控制LCD屏幕亮度
 * @参数：亮度值，范围0~0xF
 * 	*/
void LCD_Brightness(uint8_t Brightness){
	if(Brightness >= 0xF){
		Brightness = 0xF;
	}
	LCD_Device.Brightness = Brightness;
	 __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, LCD_Device.Brightness);
}


/*
 * @功能：SPI阻塞发送命令字节
 * @参数：要发送的字节内容
 * */
static void LCD_Display_Command(uint8_t command){
	if(LCD_Device.LCD_TX_Flag == Screen_OK){
		LCD_Command_Valid;
		LCD_CS_Valid;
		HAL_SPI_Transmit(&hspi2,&command,1,10);
		LCD_CS_Invalid;
	}
}
/*
 * @功能：PSPI阻塞发送数据字节
 * @参数：要发送的字节内容
 * */
static void LCD_Display_Data(uint8_t data){
	if(LCD_Device.LCD_TX_Flag == Screen_OK){
		LCD_Data_Valid;
		LCD_CS_Valid;
		HAL_SPI_Transmit(&hspi2,&data,1,10);
		LCD_CS_Invalid;
	}
}


/*
 * @功能：LCD复位
 * */
static void LCD_Reset(void){
    LCD_RST_Valid;  // RST拉低（复位开始）
    HAL_Delay(100);  // 复位延时
    LCD_RST_Invalid;// RST拉高（复位结束，芯片唤醒）
    HAL_Delay(50);  // 等待芯片稳定
    log_printf("LCD Reset\r\n");
}



/**
 * @brief  设置LCD显示方向（支持4种旋转模式）
 * @param  direction: 显示方向（0-3，超出范围自动取模）
 *         0: 0°旋转（竖屏，原始方向）
 *         1: 90°顺时针旋转（横屏）
 *         2: 180°旋转（竖屏倒置）
 *         3: 270°顺时针旋转（横屏倒置）
 * @retval 无
 * @note   核心配置ILI9341的0x36寄存器（内存访问控制），同步更新设备结构体参数
 */
static void LCD_Direction(uint8_t direction){
    // 1. 初始化LCD核心指令（ILI9341固定指令，与方向无关）
    LCD_Device.SetX = 0x2A;    // X轴地址设置指令（固定0x2A）
    LCD_Device.SetY = 0x2B;    // Y轴地址设置指令（固定0x2B）
    LCD_Device.W_Ram = 0x2C;   // 写显存指令（固定0x2C）
    LCD_Device.R_Ram = 0x2E;   // 读显存指令（固定0x2E）

    // 2. 限制方向范围（0-3），避免非法值
    LCD_Device.Direction = direction;

    // 3. 根据方向配置寄存器和宽高参数
    switch (LCD_Device.Direction)
    {
        case 0:  // 0°旋转（竖屏，原始方向）
            LCD_Device.Width = 240;  // 竖屏宽度：240像素
            LCD_Device.Height = 320; // 竖屏高度：320像素
            // 0x36寄存器配置：BGR=1(0x08) | MY=0 | MX=0 | MV=0 → 0x08
            LCD_Display_Command(0x36);
            LCD_Display_Data((1 << 3) | (0 << 6) | (0 << 7));
            log_printf("Set the LCD orientation to: 0° portrait screen\r\n");
            break;

        case 1:  // 90°顺时针旋转（横屏）
            LCD_Device.Width = 320;  // 横屏宽度：320像素（原高度）
            LCD_Device.Height = 240; // 横屏高度：240像素（原宽度）
            // 0x36寄存器配置：BGR=1(0x08) | MX=1(0x40) | MV=1(0x20) → 0x68
            LCD_Display_Command(0x36);
            LCD_Display_Data((1 << 3) | (0 << 7) | (1 << 6) | (1 << 5));
            log_printf("Set the LCD orientation to: 90° landscape\r\n");
            break;

        case 2:  // 180°旋转（竖屏倒置）
            LCD_Device.Width = 240;  // 宽高不变，仍为竖屏分辨率
            LCD_Device.Height = 320;
            // 0x36寄存器配置：BGR=1(0x08) | MY=1(0x80) | MX=1(0x40) → 0xC8
            LCD_Display_Command(0x36);
            LCD_Display_Data((1 << 3) | (1 << 6) | (1 << 7));
            log_printf("Set the LCD orientation to: 180° vertical screen inversion\r\n");
            break;

        case 3:  // 270°顺时针旋转（横屏倒置）
            LCD_Device.Width = 320;  // 宽高仍为横屏分辨率
            LCD_Device.Height = 240;
            // 0x36寄存器配置：BGR=1(0x08) | MY=1(0x80) | MV=1(0x20) → 0xA8
            LCD_Display_Command(0x36);
            LCD_Display_Data((1 << 3) | (1 << 7) | (1 << 5));
            log_printf("Set the LCD orientation to: 270° landscape inverted\r\n");
            break;

        default:  // 冗余保护（实际已通过%4限制，不会进入）
            LCD_Device.Direction = 0;
            LCD_Device.Width = 240;
            LCD_Device.Height = 320;
            LCD_Display_Command(0x36);
            LCD_Display_Data(0x08);
            log_printf("Invalid LCD orientation parameter, defaulting to 0° vertical screen\r\n");
            break;
    }
}














/**
 * @功能：设置刷屏窗口
 * @参数：矩形对角线坐标
 * @返回：无
 * @注意：坐标超出屏幕分辨率时会自动截断，避免越界，在填充屏幕数据前调用
 */
void LCD_Set_Page_Cursor(uint16_t XL, uint16_t YL, uint16_t XR, uint16_t YR)
{
    //坐标边界检查（避免超出LCD当前分辨率）

    if (XL >= LCD_Device.Width)  XL = LCD_Device.Width - 1;
    if (YL >= LCD_Device.Height) YL = LCD_Device.Height - 1;
    if (XR >= LCD_Device.Width)  XR = LCD_Device.Width - 1;
    if (YR >= LCD_Device.Height) YR = LCD_Device.Height - 1;

    //设置X轴地址窗口（ILI9341指令0x2A）
    LCD_Display_Command(LCD_Device.SetX);  // 0x2A
    // 起始X高8位、低8位,结束X高8位、低8位
    uint8_t X_Data[4] = {
    		(uint8_t)((XL >> 8) & 0xFF),
			(uint8_t)(XL & 0xFF),
			(uint8_t)((XR >> 8) & 0xFF),
			(uint8_t)(XR & 0xFF)
    };

	LCD_Data_Valid;
	LCD_CS_Valid;
	HAL_SPI_Transmit(&hspi2,X_Data,4,10);
	LCD_CS_Invalid;




    //设置Y轴地址窗口（ILI9341指令0x2B）
    LCD_Display_Command(LCD_Device.SetY);  // 0x2B
    // 起始Y高8位、低8位,结束Y高8位、低8位
    uint8_t Y_Data[4] = {
    		(uint8_t)((YL >> 8) & 0xFF),
			(uint8_t)(YL & 0xFF),
			(uint8_t)((YR >> 8) & 0xFF),
			(uint8_t)(YR & 0xFF)
    };
	LCD_Data_Valid;
	LCD_CS_Valid;
	HAL_SPI_Transmit(&hspi2,Y_Data,4,10);
	LCD_CS_Invalid;

    // 调试日志
    //UART_Printf("页光标已设置：起始坐标(%d,%d)，结束坐标(%d,%d)\r\n", XL, YL, XR, YR);
}




/*
 * @功能：发送指定缓冲区的数据流
 * @参数：缓冲区的地址
 * @参数：要发送字节数量，不可超过65535
 * */
void LCD_ScreenBuffer_Send(lv_color_t * color_p, uint16_t size){
	LCD_Display_Command(LCD_Device.W_Ram);  //发送写显存指令（ILI9341指令0x2C）
	LCD_CS_Valid;
	LCD_Data_Valid;
	HAL_SPI_Transmit_DMA(&hspi2, (uint8_t *)color_p, size);
	//LCD_CS_Invalid;回调函数再执行，否则发送会失败
	//回调函数仅再此句执行结束被调用
}



/*
 * @功能：入队SPI发送硬件，最多缓冲2个，专门面向lvgl双缓冲设计
 * @参数：lvgl句柄，详见“lvgl\examples\porting\lv_port_disp.c”
 * @参数：disp_flush()的输入参数，其包含刷新窗口的坐标信息
 * @参数：disp_flush()的输入参数，是显示缓冲区的物理地址
 * @返回：入队操作后后队列的总数量，最大为2，最小为1
 * */
uint8_t SPI_Transmission_ENQueue(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p){
	SPI_QueueA.Queue_sum++;
	SPI_QueueB.Queue_sum = SPI_QueueA.Queue_sum;
	SPI_send_queue *Queue;
	if(SPI_QueueA.Queue_sum == 1){
		Queue = &SPI_QueueA;
	}else if(SPI_QueueA.Queue_sum == 2){
		Queue = &SPI_QueueB;
	}else{
		while(1);//队列仅两个位置，避免指向未知地址
	}
	uint16_t pixel_w = area->x2 - area->x1 + 1;  // 区域宽度
	uint16_t pixel_h = area->y2 - area->y1 + 1;  // 区域高度
	uint16_t pixel_num = pixel_w * pixel_h;      // 总像素数
	uint16_t data_len = pixel_num * 2;           // RGB565每个像素2字节


	//执行入队操作
	Queue->X1 = area->x1;
	Queue->X2 = area->x2;
	Queue->Y1 = area->y1;
	Queue->Y2 = area->y2;
	Queue->color_p = color_p;
	Queue->size = data_len;
	Queue->disp_drv = disp_drv;


	return Queue->Queue_sum;
}


/*
 * @功能：出队SPI发送硬件，一般在缓冲区数据流发送结束后执行
 * @警告：执行此函数前一定要确保队列内至少有一个可用数据，
 * 也就是出队前一定有一个对应的入队操作
 */
void SPI_Transmission_DEQueue(void){
	if(SPI_QueueA.Queue_sum == 0) while(1);//无队列可出，循环阻塞供调试定位
	//出队操作
	SPI_QueueA.Queue_sum--;
	SPI_QueueB.Queue_sum = SPI_QueueA.Queue_sum;
	if(SPI_QueueA.Queue_sum == 1){
		SPI_QueueA.X1 = SPI_QueueB.X1;
		SPI_QueueA.X2 = SPI_QueueB.X2;
		SPI_QueueA.Y1 = SPI_QueueB.Y1;
		SPI_QueueA.Y2 = SPI_QueueB.Y2;
		SPI_QueueA.color_p = SPI_QueueB.color_p;
		SPI_QueueA.size = SPI_QueueB.size;
		SPI_QueueA.disp_drv = SPI_QueueB.disp_drv;
	}
}

/*
 * @功能：申请一次SPI填充，会自动从队列里选择可发数据
 * @注意：发送完成后会触发LCD_SPI_TxCpltCallback回调函数，在其内部必须完成出队操作
 * @注意：申请情况仅两种：
 * 1，disp_flush()中用于启动空队列；
 * 2，LCD_SPI_TxCpltCallback()中;
 * */
void LCD_Apply_ScreenFilling(void){
	LCD_Set_Page_Cursor(SPI_QueueA.X1, SPI_QueueA.Y1, SPI_QueueA.X2, SPI_QueueA.Y2);
	LCD_ScreenBuffer_Send(SPI_QueueA.color_p, SPI_QueueA.size);
}

void LCD_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi){
	if(hspi == &hspi2){
		LCD_CS_Invalid;//发送结束挂起CS
		SPI_Transmission_DEQueue();//发送结束执行出队操作
		lv_disp_flush_ready(SPI_QueueA.disp_drv);//有空余缓冲区，通知LVGL后台

		//如果还有待发送数据，再次申请一次SPI填充
		if(SPI_QueueA.Queue_sum == 1){
			LCD_Apply_ScreenFilling();
		}

	}
}

