/**
  ******************************************************************************
  * @file    NRF24L01.c
  * @brief   NRF24L01无线收发器驱动程序
  *          支持主从模式通信，带自动重发和ACK确认机制
  ******************************************************************************
  */

#include "NRF24L01.h"
#include "UART.h"
#include <stdio.h>

/* 全局变量定义 --------------------------------------------------------------*/
volatile uint8_t nrf_rx_ready = 1;  // 接收数据就绪标志，1表示可以接受数据
volatile uint8_t nrf_tx_ready = 1;  // 发送就绪标志，1表示可以发送新数据
volatile uint8_t nrf_error_flag = 0; // 错误标志：0=正常，1=重发超限，2=超时
NRF_FrameTypeDef nrf_rx_frame;      // 接收帧缓冲区
NRF_FrameTypeDef nrf_tx_frame;      // 发送帧缓冲区

/* 内部变量 ------------------------------------------------------------------*/
static uint8_t nrf_tx_seq = 0;      // 发送序列号（用于区分不同数据包）

/* 外部变量声明 --------------------------------------------------------------*/
extern SPI_HandleTypeDef hspi3;     // SPI3句柄（由CubeMX生成）

/* 引脚操作宏 ----------------------------------------------------------------*/
#define NRF_CS_LOW()   HAL_GPIO_WritePin(NRF_CS_GPIO_Port, NRF_CS_Pin, GPIO_PIN_RESET)   // 选中NRF24L01
#define NRF_CS_HIGH()  HAL_GPIO_WritePin(NRF_CS_GPIO_Port, NRF_CS_Pin, GPIO_PIN_SET)     // 释放NRF24L01
#define NRF_CE_LOW()   HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_RESET)   // 禁用芯片（待机模式）
#define NRF_CE_HIGH()  HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_SET)     // 使能芯片（接收/发送）




/**
  * @brief  SPI单字节读写函数
  * @param  data: 要发送的数据
  * @retval 接收到的数据
  * @note   NRF24L01使用SPI模式0，MSB优先
  */
static uint8_t SPI_ReadWriteByte(uint8_t data) {
    uint8_t rx_data = 0;
    HAL_SPI_TransmitReceive(&hspi3, &data, &rx_data, 1, 10);
    return rx_data;
}

/**
  * @brief  读取NRF24L01寄存器
  * @param  reg: 寄存器地址
  * @retval 寄存器值
  * @note   1. 拉低CS选中芯片
  *         2. 发送读命令（0x00 | 寄存器地址）
  *         3. 发送NOP命令（0xFF）读取数据
  *         4. 拉高CS释放芯片
  */
uint8_t NRF24L01_ReadReg(uint8_t reg) {
    uint8_t value;
    NRF_CS_LOW();  // 选中芯片，开始SPI通信
    SPI_ReadWriteByte(NRF_CMD_R_REGISTER | (reg & 0x1F));  // 发送读命令
    value = SPI_ReadWriteByte(NRF_CMD_NOP);                // 读取寄存器值
    NRF_CS_HIGH(); // 释放芯片，结束SPI通信
    return value;
}

/**
  * @brief  写入NRF24L01寄存器
  * @param  reg: 寄存器地址
  * @param  value: 要写入的值
  * @retval 无
  * @note   1. 拉低CS选中芯片
  *         2. 发送写命令（0x20 | 寄存器地址）
  *         3. 发送要写入的数据
  *         4. 拉高CS释放芯片
  */
void NRF24L01_WriteReg(uint8_t reg, uint8_t value) {
    NRF_CS_LOW();
    SPI_ReadWriteByte(NRF_CMD_W_REGISTER | (reg & 0x1F));  // 发送写命令
    SPI_ReadWriteByte(value);                              // 发送数据
    NRF_CS_HIGH();
}

/**
  * @brief  读取NRF24L01缓冲区（多字节）
  * @param  reg: 寄存器起始地址
  * @param  buf: 读取缓冲区
  * @param  len: 读取长度
  * @retval 无
  * @note   用于读取地址、数据等连续区域
  */
void NRF24L01_ReadBuf(uint8_t reg, uint8_t *buf, uint8_t len) {
    NRF_CS_LOW();
    SPI_ReadWriteByte(NRF_CMD_R_REGISTER | (reg & 0x1F));  // 发送读命令
    HAL_SPI_Receive(&hspi3, buf, len, 10);
    NRF_CS_HIGH();
}

/**
  * @brief  写入NRF24L01缓冲区（多字节）
  * @param  reg: 寄存器起始地址
  * @param  buf: 写入缓冲区
  * @param  len: 写入长度
  * @retval 无
  * @note   用于写入地址、数据等连续区域
  */
void NRF24L01_WriteBuf(uint8_t reg, uint8_t *buf, uint8_t len) {
    NRF_CS_LOW();
    SPI_ReadWriteByte(NRF_CMD_W_REGISTER | (reg & 0x1F));  // 发送写命令
    HAL_SPI_Transmit(&hspi3, buf, len, 10);
    NRF_CS_HIGH();
}

/**
  * @brief  清空TX FIFO缓冲区
  * @param  无
  * @retval 无
  * @note   在发送失败或重新发送前必须清空TX FIFO
  */
void NRF24L01_FlushTX(void) {
    NRF_CS_LOW();
    SPI_ReadWriteByte(NRF_CMD_FLUSH_TX);  // 发送清空TX FIFO命令
    NRF_CS_HIGH();
}

/**
  * @brief  清空RX FIFO缓冲区
  * @param  无
  * @retval 无
  * @note   在接收缓冲区溢出或重新接收前可以清空RX FIFO
  */
void NRF24L01_FlushRX(void) {
    NRF_CS_LOW();
    SPI_ReadWriteByte(NRF_CMD_FLUSH_RX);  // 发送清空RX FIFO命令
    NRF_CS_HIGH();
}

/**
  * @brief  读取状态寄存器
  * @param  无
  * @retval 状态寄存器值
  * @note   通过发送NOP命令读取状态寄存器，不会影响其他寄存器
  */
uint8_t NRF24L01_ReadStatus(void) {
    NRF_CS_LOW();
    uint8_t status = SPI_ReadWriteByte(NRF_CMD_NOP);  // 发送NOP命令读取状态
    NRF_CS_HIGH();
    return status;
}

/**
  * @brief  检测NRF24L01是否存在
  * @param  无
  * @retval 0=存在，1=不存在
  * @note   使用双重验证：CONFIG寄存器 + 地址寄存器
  */
uint8_t NRF24L01_Check(void) {
	log_printf("检测NRF24L01...\r\n");

    // 验证1：读写CONFIG寄存器
    uint8_t config_original = NRF24L01_ReadReg(NRF_REG_CONFIG);
    // 测试写入和读取（使用非0值，避免与上电默认值冲突）
    NRF24L01_WriteReg(NRF_REG_CONFIG, 0x0C);  // 测试值
    NRF24L01_ReadReg(NRF_REG_CONFIG);
    HAL_Delay(1);
    uint8_t config_read = NRF24L01_ReadReg(NRF_REG_CONFIG);
    if (config_read != 0x0C) {
    	log_printf("配置寄存器测试失败: 写入0x0C，读取0x%02X\r\n", config_read);
        // 尝试恢复初始值
        NRF24L01_WriteReg(NRF_REG_CONFIG, config_original);
        return 1;
    }
    // 恢复原始配置
    NRF24L01_WriteReg(NRF_REG_CONFIG, config_original);
    log_printf("配置寄存器测试: 通过\r\n");




    // 验证2：读写地址寄存器
    uint8_t test_addr[5] = {0x12, 0x34, 0x56, 0x78, 0x90};
    uint8_t read_addr[5];
    // 保存原始地址
    uint8_t original_addr[5];
    NRF24L01_ReadBuf(NRF_REG_TX_ADDR, original_addr, 5);
    // 写入测试地址
    NRF24L01_WriteBuf(NRF_REG_TX_ADDR, test_addr, 5);
    // 读取验证
    NRF24L01_ReadBuf(NRF_REG_TX_ADDR, read_addr, 5);
    // 比较
    for (uint8_t i = 0; i < 5; i++) {
        if (read_addr[i] != test_addr[i]) {
        	log_printf("地址寄存器测试失败:NRF24L01_ReadBuf[%d]。 写入0x%02X，读取0x%02X\r\n",
                       i, test_addr[i], read_addr[i]);
            // 恢复原始地址
            NRF24L01_WriteBuf(NRF_REG_TX_ADDR, original_addr, 5);
            return 1;
        }
    }
    // 恢复原始地址
    NRF24L01_WriteBuf(NRF_REG_TX_ADDR, original_addr, 5);
    log_printf("地址寄存器测试: 通过\r\n");









    return 0;
}

/**
  * @brief  NRF24L01初始化
  * @param  无
  * @retval 0=成功，1=SPI失败，2=芯片未检测到，3=配置错误
  * @note   初始化顺序：
  *         1. 硬件复位
  *         2. SPI通信测试
  *         3. 进入掉电模式
  *         4. 配置寄存器
  *         5. 设置地址
  *         6. 进入接收模式
  */
uint8_t NRF24L01_Init(void) {
	log_printf("开始初始化NRF24L01...\r\n");

    /* 1. 硬件复位 */
    NRF_CS_HIGH();  // CS置高，未选中
    NRF_CE_LOW();   // CE置低，进入待机模式
    HAL_Delay(100); // 等待芯片完全复位（手册要求>100ms）
    uint8_t test_val = 0;
    HAL_SPI_Transmit(&hspi3, &test_val, 1, 100); // 预发送，使SPI进入正确工作模式，不可缺少！！！

    /* 2. SPI通信测试（关键步骤） */
    if (NRF24L01_Check() == 1) {
    	log_printf("SPI通信失败\r\n");
        return 1;
    }
    log_printf("SPI通信正常\r\n");

    /* 3. 进入掉电模式进行配置 */
    NRF24L01_WriteReg(NRF_REG_CONFIG, 0x00);
    HAL_Delay(5);

    /* 4. 检查芯片是否存在 */
    if (NRF24L01_Check()) {
    	log_printf("NRF24L01未检测到!\r\n");
        return 2;
    }

    /* 5. 清空所有状态 */
    NRF24L01_WriteReg(NRF_REG_STATUS, 0x70); // 清除所有中断标志
    NRF24L01_FlushTX();  // 清空发送缓冲区
    NRF24L01_FlushRX();  // 清空接收缓冲区

    /* 6. 配置自动应答 */
    NRF24L01_WriteReg(NRF_REG_EN_AA, 0x01);      // 使能通道0自动应答
    NRF24L01_WriteReg(NRF_REG_EN_RXADDR, 0x01);  // 使能通道0接收

    /* 7. 配置地址宽度（5字节） */
    NRF24L01_WriteReg(NRF_REG_SETUP_AW, 0x03);

    /* 8. 配置自动重发：500us延迟，10次重发 */
    NRF24L01_WriteReg(NRF_REG_SETUP_RETR, 0x1A); // 00101010: 500us, 10次

    /* 9. 配置射频通道（2.476GHz） */
    NRF24L01_WriteReg(NRF_REG_RF_CH, 76);

    /* 10. 配置射频参数*/
    NRF24L01_WriteReg(NRF_REG_RF_SETUP, NRF_RF_2MBPS_0DBM);   // 1Mbps, 0dBm

    /* 11. 配置接收地址和发送地址（两个板子相同才能互相通信） */
    uint8_t addr[5] = {0x34, 0x43, 0x56, 0x65, 0x78};
    NRF24L01_WriteBuf(NRF_REG_RX_ADDR_P0, addr, 5);  // 接收地址（管道0）
    NRF24L01_WriteBuf(NRF_REG_TX_ADDR, addr, 5);     // 发送地址

    /* 12. 配置接收有效载荷长度（32字节） */
    NRF24L01_WriteReg(NRF_REG_RX_PW_P0, NRF_MAX_PAYLOAD_LEN);

    /* 13. 再次清空FIFO和状态寄存器 */
    NRF24L01_FlushTX();
    NRF24L01_FlushRX();
    NRF24L01_WriteReg(NRF_REG_STATUS, 0x70);//清除所有中断标志

    /* 14. 配置为接收模式并上电 */
    // CONFIG配置：0x0F = 00001111
    // bit0=1: PRIM_RX=1（接收模式）
    // bit3=1: EN_CRC=1（CRC使能）
    // bit2=1: CRCO=1（2字节CRC）
    // bit1=1: PWR_UP=1（上电）
    NRF24L01_WriteReg(NRF_REG_CONFIG, CONFIG_PWR_UP | CONFIG_EN_CRC | CONFIG_CRC0 | CONFIG_PRIM_RX);
    HAL_Delay(5);  // 等待稳定

    /* 15. 启动接收（CE置高） */
    NRF_CE_HIGH();
    HAL_Delay(5);

    /* 16. 验证配置 */
    uint8_t config = NRF24L01_ReadReg(NRF_REG_CONFIG);
    log_printf("最终CONFIG: 0x%02X (bit0=PRIM_RX=%d)\r\n",
               config, (config & CONFIG_PRIM_RX) ? 1 : 0);

    if (!(config & CONFIG_PRIM_RX)) {
    	log_printf("警告：NRF24L01未处于接收模式！\r\n");
        return 3;
    }


    return 0;
}


void NRF24L01_Test(void){
	log_printf("\r\n\r\n\r\n\r\n");
	log_printf("NRF24L01主从通信测试开始...\r\n");
	if (NRF24L01_Init() != 0) {
		log_printf("NRF24L01初始化失败，系统暂停！\r\n");
	    Error_Handler();
	  }
	// 打印寄存器状态
	NRF24L01_PrintRegisters();
	// 等待5秒，确保两个板子都准备就绪
	log_printf("等待3秒，确保两个板子都准备就绪...\r\n");
	for (int i = 3; i > 0; i--) {
		log_printf("%d... ", i);
	    HAL_Delay(1000);
	}
	  // 主机主动发送第一条消息
	  uint8_t test_data[] = "Hello from Remote!";
	  log_printf("\r\n主机发送第一条测试消息...\r\n");
	  NRF24L01_SendFrame(FRAME_TYPE_CMD, test_data, strlen((char*)test_data));
}


/**
  * @brief  进入发送模式（内部函数）
  * @param  无
  * @retval 无
  * @note   发送数据前必须切换到发送模式
  */
static void NRF24L01_EnterTxMode(void) {
    NRF_CE_LOW();  // 先拉低CE进入待机模式
    HAL_Delay(1);

    /* 配置为发送模式（清除PRIM_RX位） */
    uint8_t config = NRF24L01_ReadReg(NRF_REG_CONFIG);
    config &= ~CONFIG_PRIM_RX;  // bit0=0（发送模式）
    config |= CONFIG_PWR_UP;    // bit1=1（保持上电）
    config |= CONFIG_EN_CRC;    // bit3=1（CRC使能）
    NRF24L01_WriteReg(NRF_REG_CONFIG, config);

    HAL_Delay(2);  // 等待模式切换稳定

    /* 清除发送相关中断标志 */
    NRF24L01_WriteReg(NRF_REG_STATUS, STATUS_TX_DS | STATUS_MAX_RT);

    log_printf("进入发送模式，CONFIG=0x%02X\r\n", NRF24L01_ReadReg(NRF_REG_CONFIG));
}

/**
  * @brief  进入接收模式（内部函数）
  * @param  无
  * @retval 无
  * @note   发送完成后必须切换回接收模式
  */
static void NRF24L01_EnterRxMode(void) {
    NRF_CE_LOW();  // 先拉低CE进入待机模式
    HAL_Delay(1);

    /* 配置为接收模式（设置PRIM_RX位） */
    uint8_t config = NRF24L01_ReadReg(NRF_REG_CONFIG);
    config |= CONFIG_PWR_UP;    // bit1=1（保持上电）
    config |= CONFIG_PRIM_RX;   // bit0=1（接收模式）
    config |= CONFIG_EN_CRC;    // bit3=1（CRC使能）
    NRF24L01_WriteReg(NRF_REG_CONFIG, config);

    HAL_Delay(2);  // 等待模式切换稳定

    /* 清除接收中断标志 */
    NRF24L01_WriteReg(NRF_REG_STATUS, STATUS_RX_DR);

    /* 启动接收（CE置高） */
    NRF_CE_HIGH();
    HAL_Delay(2);

    log_printf("进入接收模式，CONFIG=0x%02X\r\n", NRF24L01_ReadReg(NRF_REG_CONFIG));
}

/* @brief  等待SPI DMA传输完成（修正超时逻辑）
 * @note   超时时间为10ms，避免死等
 * @retval 无
 */

/**
  * @brief  发送数据帧
  * @param  frame_type: 帧类型（FRAME_TYPE_CMD/FRAME_TYPE_VIDEO）
  * @param  data: 要发送的数据指针
  * @param  len: 数据长度（最大30字节，因为包含2字节头部）
  * @retval 0=成功，1=长度超限，2=发送忙，3=重发超限，4=超时
  * @note   发送流程：
  *         1. 检查参数和状态
  *         2. 准备发送数据
  *         3. 进入发送模式
  *         4. 写入TX FIFO
  *         5. 触发发送
  *         6. 等待ACK或超时
  *         7. 返回接收模式
  */
uint8_t NRF24L01_SendFrame(uint8_t frame_type, uint8_t *data, uint8_t len) {
    /* 1. 参数检查 */
    if (len > (CMD_DATA_LEN - 1)) { // 减1因为包含seq字节
    	log_printf("错误：数据长度超过限制(%d>%d)\r\n", len, CMD_DATA_LEN - 1);
        return 1;
    }

    if (!nrf_tx_ready) {
    	log_printf("警告：上一次发送还未完成\r\n");
        return 2;
    }

    /* 2. 准备发送数据 */
    memset(&nrf_tx_frame, 0, sizeof(NRF_FrameTypeDef));
    nrf_tx_frame.type = frame_type;    // 帧类型
    nrf_tx_frame.seq = nrf_tx_seq++;   // 序列号（自动递增）
    if (len > 0) {
        memcpy(nrf_tx_frame.data, data, len);  // 复制数据
    }

    log_printf("准备发送数据[%d]：类型=0x%02X，长度=%d字节\r\n",
               nrf_tx_frame.seq, frame_type, len);

    /* 3. 进入发送模式 */
    NRF24L01_EnterTxMode();

    /* 4. 清空TX FIFO（避免旧数据干扰） */
    NRF24L01_FlushTX();
    HAL_Delay(1);
    /* 5. 写入发送数据到TX FIFO */
    NRF_CS_LOW();
    SPI_ReadWriteByte(NRF_CMD_W_TX_PAYLOAD);     // 写TX数据命令
    SPI_ReadWriteByte(nrf_tx_frame.type);        // 帧类型（1字节）
    SPI_ReadWriteByte(nrf_tx_frame.seq);         // 序列号（1字节）

    for (uint8_t i = 0; i < len; i++) {
        SPI_ReadWriteByte(data[i]);              // 数据部分
    }

    // 填充剩余字节为0（可选）
    for (uint8_t i = len; i < (CMD_DATA_LEN - 1); i++) {
        SPI_ReadWriteByte(0x00);
    }
    NRF_CS_HIGH();

    /* 6. 触发发送（CE高脉冲） */
    nrf_tx_ready = 0;//发送未完成，非就绪态
    NRF_CE_HIGH();   // 开始发送

    HAL_Delay(1);    // 保持至少10us（手册要求）
    NRF_CE_LOW();    // 发送完成，进入待机模式
    while(nrf_tx_ready == 0){

    }

    NRF24L01_EnterRxMode();//发送完成后切换为接收模式
    return 0;  // 正常返回
}

/**
  * @brief  IRQ中断处理函数
  * @param  GPIO_Pin: 触发中断的引脚
  * @retval 无
  * @note   在STM32的GPIO外部中断回调函数中调用
  *         IRQ引脚为低电平触发，需要手动清除中断标志
  */
void NRF_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == NRF_IRQ_Pin) {
        /* 1. 读取状态寄存器，确定中断类型 */
        uint8_t status = NRF24L01_ReadStatus();

        /* 2. 接收完成中断（RX_DR） */
        if (status & STATUS_RX_DR) {
        	log_printf("[IRQ] 收到数据包 (STATUS=0x%02X)\r\n", status);

            /* 2.1 读取RX FIFO中的数据 */
            NRF_CS_LOW();
            SPI_ReadWriteByte(NRF_CMD_R_RX_PAYLOAD);  // 读RX数据命令
            nrf_rx_frame.type = SPI_ReadWriteByte(NRF_CMD_NOP);  // 帧类型
            nrf_rx_frame.seq = SPI_ReadWriteByte(NRF_CMD_NOP);   // 序列号

            for (uint8_t i = 0; i < (CMD_DATA_LEN - 1); i++) {
                nrf_rx_frame.data[i] = SPI_ReadWriteByte(NRF_CMD_NOP);  // 数据
            }
            NRF_CS_HIGH();

            /* 2.2 标记接收就绪 */
            nrf_rx_ready = 0;
            NRF24L01_FlushRX();//清除接收缓冲区
        }
        /* 3. 发送完成中断（TX_DS） */
        else if (status & STATUS_TX_DS) {
        	log_printf("[IRQ] 发送完成 (STATUS=0x%02X)\r\n", status);
            //NRF24L01_WriteReg(NRF_REG_STATUS, STATUS_TX_DS);  // 清除中断标志
            nrf_tx_ready = 1;  // 标记发送完成
            NRF24L01_FlushTX();  // 清空TX FIFO（必须）
        }
        /* 4. 重发超限中断（MAX_RT） */
        else if (status & STATUS_MAX_RT) {
        	log_printf("[IRQ] 重发超限，发送失败！接收方未应答(STATUS=0x%02X)\r\n", status);
            //NRF24L01_WriteReg(NRF_REG_STATUS, STATUS_MAX_RT);  // 清除中断标志
            nrf_tx_ready = 1;      // 标记发送完成（失败）
            NRF24L01_FlushTX();  // 清空TX FIFO（必须）
        }
        /* 5. 未知中断 */
        else {
        	log_printf("[IRQ] 未知中断 (STATUS=0x%02X)\r\n", status);
            /* 清除所有中断标志 */
        }
        NRF24L01_WriteReg(NRF_REG_STATUS, 0x70); // 清除所有中断标志
        NRF24L01_FlushRX();//清除接收缓冲区
        NRF24L01_FlushTX();  // 清空TX FIFO（必须）
        /* 6. 清除GPIO中断标志（防止重复触发） */
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
    }
}

/**
  * @brief  处理接收数据
  * @param  无
  * @retval 无
  * @note   在主循环中调用，处理接收到的数据
  */
void NRF24L01_ProcessRxData(void) {
    /* 1. 检查是否有数据待处理 */
    if (nrf_rx_ready == 0) {
    	log_printf("处理接收数据[%d]：类型=0x%02X\r\n",
                   nrf_rx_frame.seq, nrf_rx_frame.type);

        /* 2. 根据帧类型处理数据 */
        if (nrf_rx_frame.type == FRAME_TYPE_CMD) {
            /* 2.1 查找字符串结束符 */
            uint8_t str_len = 0;
            for (uint8_t i = 0; i < (CMD_DATA_LEN - 1); i++) {
                if (nrf_rx_frame.data[i] == 0) {
                    str_len = i;
                    break;
                }
                if (i == (CMD_DATA_LEN - 2)) {
                    str_len = CMD_DATA_LEN - 1;
                }
            }

            /* 2.2 检查是否为可打印ASCII */
            uint8_t is_ascii = 1;
            for (uint8_t i = 0; i < str_len; i++) {
                if (nrf_rx_frame.data[i] < 32 || nrf_rx_frame.data[i] > 126) {
                    is_ascii = 0;
                    break;
                }
            }

            /* 2.3 打印数据 */
            if (is_ascii && str_len > 0) {
            	log_printf("收到指令: \"");
                for (uint8_t i = 0; i < str_len; i++) {
                	log_printf("%c", nrf_rx_frame.data[i]);
                }
                log_printf("\"\r\n");


            } else {
                /* 2.5 非ASCII数据，以HEX格式显示 */
            	log_printf("收到数据(HEX): ");
                uint8_t display_len = str_len > 16 ? 16 : str_len;
                for (uint8_t i = 0; i < display_len; i++) {
                	log_printf("%02X ", nrf_rx_frame.data[i]);
                }
                if (str_len > 16) UART_Printf("...");
                log_printf("\r\n");
            }
        }
        /* 3. 视频帧处理（可扩展） */
        else if (nrf_rx_frame.type == FRAME_TYPE_VIDEO) {
        	log_printf("收到视频帧[%d]，前2字节: 0x%02X 0x%02X\r\n",
                       nrf_rx_frame.seq, nrf_rx_frame.data[0], nrf_rx_frame.data[1]);
        }
        /* 4. 未知帧类型 */
        else {
        	log_printf("收到未知帧类型: 0x%02X\r\n", nrf_rx_frame.type);
        }

        /* 5. 清除接收就绪标志 */
        nrf_rx_ready = 1;
    }


}

/**
  * @brief  打印所有寄存器状态（调试用）
  * @param  无
  * @retval 无
  * @note   用于调试和验证配置是否正确
  */
void NRF24L01_PrintRegisters(void) {
	log_printf("\r\n===== NRF24L01 寄存器状态 =====\r\n");

    /* 1. 要读取的寄存器列表 */
    uint8_t regs[] = {
        NRF_REG_CONFIG, NRF_REG_EN_AA, NRF_REG_EN_RXADDR,
        NRF_REG_SETUP_AW, NRF_REG_SETUP_RETR, NRF_REG_RF_CH,
        NRF_REG_RF_SETUP, NRF_REG_STATUS, NRF_REG_OBSERVE_TX,
        NRF_REG_FIFO_STATUS
    };

    /* 2. 寄存器名称 */
    const char *reg_names[] = {
        "CONFIG", "EN_AA", "EN_RXADDR", "SETUP_AW", "SETUP_RETR",
        "RF_CH", "RF_SETUP", "STATUS", "OBSERVE_TX", "FIFO_STATUS"
    };

    /* 3. 读取并打印每个寄存器 */
    for (uint8_t i = 0; i < sizeof(regs)/sizeof(regs[0]); i++) {
        uint8_t val = NRF24L01_ReadReg(regs[i]);
        log_printf("%-12s: 0x%02X\r\n", reg_names[i], val);
    }

    /* 4. 读取并打印地址 */
    uint8_t tx_addr[5], rx_addr[5];
    NRF24L01_ReadBuf(NRF_REG_TX_ADDR, tx_addr, 5);
    NRF24L01_ReadBuf(NRF_REG_RX_ADDR_P0, rx_addr, 5);

    log_printf("TX_ADDR: ");
    for (uint8_t i = 0; i < 5; i++) log_printf("%02X ", tx_addr[i]);
    log_printf("\r\nRX_ADDR_P0: ");
    for (uint8_t i = 0; i < 5; i++) UART_Printf("%02X ", rx_addr[i]);
    log_printf("\r\n===============================\r\n");
}



/**
 * @brief  NRF24L01主机定期发送心跳包
 * @note   需在main函数的while(1)循环中调用，无需手动定时（内部通过HAL_GetTick()判断）
 * @retval HAL_StatusTypeDef: HAL_OK=本次发送成功/未到发送时间；HAL_ERROR=发送失败（若NRF发送函数支持返回状态）
 */
HAL_StatusTypeDef NRF24L01_Master_SendHeartbeat(void)
{
    // 静态变量：仅初始化一次，每次调用函数保持上次的值
    static uint32_t last_send = 0;  // 上次发送心跳的时间戳
    static uint8_t counter = 0;     // 心跳计数器

    // 判断是否到达发送间隔（HAL_GetTick()返回系统运行时间，单位ms，溢出后自动处理）
    if (HAL_GetTick() - last_send > HEARTBEAT_INTERVAL_MS)
    {
        // 更新上次发送时间为当前时间
        last_send = HAL_GetTick();

        // 拼接心跳字符串（缓冲区32字节，与原逻辑一致）
        char heartbeat[32];
        sprintf(heartbeat, "Remote Heartbeat %d", counter++);

        // 打印调试日志（保留原逻辑）
        log_printf("\r\n遥控端发送心跳: %s\r\n", heartbeat);

        // 发送心跳帧（调用原NRF发送接口，返回发送状态）
        // 注：若你的NRF24L01_SendFrame返回值不是HAL_StatusTypeDef，需根据实际返回值调整
        if (NRF24L01_SendFrame(FRAME_TYPE_CMD, (uint8_t*)heartbeat, strlen(heartbeat)) == SUCCESS)
        {
            return HAL_OK;  // 发送成功
        }
        else
        {
        	Error_Handler();
            return HAL_ERROR; // 发送失败
        }
    }

    // 未到发送间隔，返回正常状态
    return HAL_OK;
}







