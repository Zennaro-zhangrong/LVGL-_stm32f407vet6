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

extern SPI_HandleTypeDef hspi3;
NRFHandleTypeDef nrf = {0};


//////////////////////////////////////////////////
#define NRF_rxBuff_Size 90
NRF_FrameTypeDef tx_buffer;
CCM NRF_FrameTypeDef rx_buffer[NRF_rxBuff_Size];
volatile uint8_t Read_index = 0;
volatile uint8_t Write_index = 0;
volatile uint8_t rxBuff_count = 0;
//////////////////////////////////////////////////


/**
  * @brief  SPI单字节读写函数
  * @param  data: 要发送的数据
  * @retval 接收到的数据
  * @note   NRF24L01使用SPI模式0，MSB
  */
static uint8_t SPI_ReadWriteByte(uint8_t data) {
    uint8_t rx_data = 0;
    HAL_SPI_TransmitReceive(nrf.SPI_CHANNEL, &data, &rx_data, 1, 5);
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
static uint8_t NRF24L01_ReadReg(uint8_t reg) {
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
static void NRF24L01_WriteReg(uint8_t reg, uint8_t value) {
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
static void NRF24L01_ReadBuf(uint8_t reg, uint8_t *buf, uint8_t len) {
    NRF_CS_LOW();
    SPI_ReadWriteByte(NRF_CMD_R_REGISTER | (reg & 0x1F));  // 发送读命令
    HAL_SPI_Receive(nrf.SPI_CHANNEL, buf, len, 10);
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
static void NRF24L01_WriteBuf(uint8_t reg, uint8_t *buf, uint8_t len) {
    NRF_CS_LOW();
    SPI_ReadWriteByte(NRF_CMD_W_REGISTER | (reg & 0x1F));  // 发送写命令
    HAL_SPI_Transmit(nrf.SPI_CHANNEL, buf, len, 10);
    NRF_CS_HIGH();
}

/**
  * @brief  清空TX FIFO缓冲区
  * @param  无
  * @retval 无
  * @note   在发送失败或重新发送前必须清空TX FIFO
  */
static void NRF24L01_ClearTX(void) {
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
static void NRF24L01_ClearRX(void) {
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
static uint8_t NRF24L01_ReadStatus(void) {
    NRF_CS_LOW();
    uint8_t status = SPI_ReadWriteByte(NRF_CMD_NOP);  // 发送NOP命令读取状态
    NRF_CS_HIGH();
    return status;
}

/**
  * @brief  检测NRF24L01是否存在
  * @param  无
  * @retval 0=存在，1=不存在
  */
static uint8_t NRF24L01_Check(void) {
	log_printf("check NRF24L01 whether it is normal!\r\n");


    // 验证：读写CONFIG寄存器
    uint8_t config_original = NRF24L01_ReadReg(NRF_REG_CONFIG);
    // 测试写入和读取（使用非0值，避免与上电默认值冲突）
    NRF24L01_WriteReg(NRF_REG_CONFIG, 0x0C);// 测试值
    NRF24L01_ReadReg(NRF_REG_CONFIG);
    uint8_t config_read = NRF24L01_ReadReg(NRF_REG_CONFIG);
    if (config_read != 0x0C) {
    	log_printf("Configuration register test failed: write[0x0C] but read[0x%02X]\r\n", config_read);
        return 1;
    }
    // 恢复原始配置
    NRF24L01_WriteReg(NRF_REG_CONFIG, config_original);

    log_printf("Configuration register test pass!\r\n");
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
	log_printf("\r\n----------Initialize NRF24L01----------\r\n");

    /* 1. 硬件复位 */
	nrf.SPI_CHANNEL = &hspi3;
	nrf.SPI_IRQ_GPIO_Port = NRF_IRQ_GPIO_Port;
	nrf.SPI_IRQ_Pin = NRF_IRQ_Pin;
	nrf.rx_buffer = rx_buffer;
	nrf.tx_buffer = &tx_buffer;
    NRF_CS_HIGH();  // CS置高，未选中
    NRF_CE_LOW();   // CE置低，进入待机模式
    HAL_Delay(100); // 等待芯片完全复位（手册要求>100ms）
    uint8_t test_val = 0;
    HAL_SPI_Transmit(nrf.SPI_CHANNEL, &test_val, 1, 100); // 预发送，使SPI进入正确工作模式，不可缺少！！！

    /* 2. SPI通信测试（关键步骤） */
    if (NRF24L01_Check() == 1) {
        log_printf("Initialize ERROR! Not found NRF24L01\r\n");
        return 1;
    }

    /* 3. 进入掉电模式进行配置 */
    NRF24L01_WriteReg(NRF_REG_CONFIG, 0x00);
    HAL_Delay(5);


    /* 4. 清空所有状态 */
    NRF24L01_WriteReg(NRF_REG_STATUS, 0x70); // 清除所有中断标志
    NRF24L01_ClearTX();  // 清空发送缓冲区
    NRF24L01_ClearRX();  // 清空接收缓冲区

    /* 5. 配置自动应答 */
    NRF24L01_WriteReg(NRF_REG_EN_AA, 0x01);      // 使能通道0自动应答
    NRF24L01_WriteReg(NRF_REG_EN_RXADDR, 0x01);  // 使能通道0接收

    /* 6. 配置地址宽度（5字节） */
    NRF24L01_WriteReg(NRF_REG_SETUP_AW, 0x03);

    /* 7. 配置自动重发：500us延迟，10次重发 */
    NRF24L01_WriteReg(NRF_REG_SETUP_RETR, 0x1A); // 00101010: 500us, 10次

    /* 8. 配置射频通道0~125（2.476GHz） */
    NRF24L01_WriteReg(NRF_REG_RF_CH, 76);

    /* 9. 配置射频参数*/
    NRF24L01_WriteReg(NRF_REG_RF_SETUP, NRF_RF_2MBPS_0DBM);

    /* 10. 配置接收地址和发送地址（两个板子相同才能互相通信） */
    uint8_t addr[5] = {0x34, 0x43, 0x56, 0x65, 0x78};
    NRF24L01_WriteBuf(NRF_REG_RX_ADDR_P0, addr, 5);  // 接收地址（管道0）
    NRF24L01_WriteBuf(NRF_REG_TX_ADDR, addr, 5);     // 发送地址

    /* 11. 配置接收有效载荷长度（32字节） */
    NRF24L01_WriteReg(NRF_REG_RX_PW_P0, NRF_MAX_PAYLOAD_LEN);

    /* 12. 再次清空FIFO和状态寄存器 */
    NRF24L01_ClearTX();
    NRF24L01_ClearRX();
    NRF24L01_WriteReg(NRF_REG_STATUS, 0x70);//清除所有中断标志

    /* 13. 配置为接收模式并上电 */
    // CONFIG配置：0x0F = 00001111
    // bit0=1: PRIM_RX=1（接收模式）
    // bit3=1: EN_CRC=1（CRC使能）
    // bit2=1: CRCO=1（2字节CRC）
    // bit1=1: PWR_UP=1（上电）
    NRF24L01_WriteReg(NRF_REG_CONFIG, CONFIG_PWR_UP | CONFIG_EN_CRC | CONFIG_CRC0 | CONFIG_PRIM_RX);
    HAL_Delay(5);  // 等待稳定

    /* 14. 启动接收（CE置高） */
    NRF_CE_HIGH();
    HAL_Delay(5);

    /* 15. 验证配置 */
    uint8_t config = NRF24L01_ReadReg(NRF_REG_CONFIG);
    log_printf("In the end, CONFIG = 0x%02X (bit0=PRIM_RX=%d)\r\n",
               config, (config & CONFIG_PRIM_RX) ? 1 : 0);
    log_printf("-----Initialization completed!-----\r\n\r\n");
    return 0;
}


/**
  * @brief  进入发送模式（内部函数）
  * @param  无
  * @retval 无
  * @note   发送数据前必须切换到发送模式
  */
static void NRF24L01_EnterTxMode(void) {
    NRF_CE_LOW();  // 先拉低CE进入待机模式

    /* 配置为发送模式（清除PRIM_RX位） */
    uint8_t config = NRF24L01_ReadReg(NRF_REG_CONFIG);
    config &= ~CONFIG_PRIM_RX;  // bit0=0（发送模式）
    config |= CONFIG_PWR_UP;    // bit1=1（保持上电）
    config |= CONFIG_EN_CRC;    // bit3=1（CRC使能）

    NRF24L01_WriteReg(NRF_REG_CONFIG, config);

    /* 清除发送相关中断标志 */
    NRF24L01_WriteReg(NRF_REG_STATUS, STATUS_TX_DS | STATUS_MAX_RT);

    log_printf("Switch to transmit mode!\r\n");
}

/**
  * @brief  进入接收模式（内部函数）
  * @param  无
  * @retval 无
  * @note   发送完成后必须切换回接收模式
  */
static void NRF24L01_EnterRxMode(void) {
    NRF_CE_LOW();  // 先拉低CE进入待机模式

    /* 配置为接收模式（设置PRIM_RX位） */
    uint8_t config = NRF24L01_ReadReg(NRF_REG_CONFIG);
    config |= CONFIG_PWR_UP;    // bit1=1（保持上电）
    config |= CONFIG_PRIM_RX;   // bit0=1（接收模式）
    config |= CONFIG_EN_CRC;    // bit3=1（CRC使能）
    NRF24L01_WriteReg(NRF_REG_CONFIG, config);


    /* 清除接收中断标志 */
    NRF24L01_WriteReg(NRF_REG_STATUS, STATUS_RX_DR);

    /* 启动接收（CE置高） */
    NRF_CE_HIGH();

    log_printf("Switch to Receive mode!\r\n");
}
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
uint8_t NRF24L01_SendDatas(uint8_t cmd, uint8_t len, uint8_t* data){
    /* 1. 参数检查 */
    if (len > (CMD_DATA_LEN - 2)) {
    	log_printf("NRF send error,the data is too long.\r\n");
        return 1;
    }


    /* 2. 准备发送数据 */
    nrf.tx_buffer->cmd = cmd;
    nrf.tx_buffer->length = len;
    memset(nrf.tx_buffer->data, 0, NRF_MAX_PAYLOAD_LEN - 2);
    memcpy(nrf.tx_buffer->data, data, len);  // 复制数据

    /* 3. 进入发送模式 */
    NRF24L01_EnterTxMode();
    /* 4. 清空TX FIFO（避免旧数据干扰） */
    NRF24L01_ClearTX();
    /* 5. 写入发送数据到TX FIFO */
    NRF_CS_LOW();
    SPI_ReadWriteByte(NRF_CMD_W_TX_PAYLOAD);     // 写TX数据命令
    uint8_t Sdata[NRF_MAX_PAYLOAD_LEN] = {0};
    Sdata[0] = nrf.tx_buffer->cmd;
    Sdata[1] = nrf.tx_buffer->length;
    memcpy(&Sdata[2], nrf.tx_buffer->data, NRF_MAX_PAYLOAD_LEN - 2);
    HAL_SPI_Transmit(nrf.SPI_CHANNEL, Sdata, NRF_MAX_PAYLOAD_LEN, 15);

    NRF_CS_HIGH();

    /* 6. 触发发送（CE高脉冲） */
    NRF_CE_HIGH();   // 开始发送
    HAL_Delay(1);    // 保持至少10us（手册要求）
    NRF_CE_LOW();    // 发送完成，进入待机模式
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
    if (GPIO_Pin != nrf.SPI_IRQ_Pin) return;
    if(HAL_GPIO_ReadPin(nrf.SPI_IRQ_GPIO_Port, GPIO_Pin) == GPIO_PIN_RESET){
        /* 1. 读取状态寄存器，确定中断类型 */
        uint8_t status = NRF24L01_ReadStatus();

        /* 2. 接收完成中断（RX_DR） */
        if (status & STATUS_RX_DR){
        	//log_printf("[NRF_IRQ] Receive data packet\r\n");
        	if(rxBuff_count < NRF_rxBuff_Size){
        		 /* 2.1 读取RX FIFO中的数据 */
        		 NRF_CS_LOW();
        		 SPI_ReadWriteByte(NRF_CMD_R_RX_PAYLOAD);  // 读RX数据命令

        		 uint8_t Sdata[NRF_MAX_PAYLOAD_LEN] = {0};
        		 uint8_t Rdata[NRF_MAX_PAYLOAD_LEN] = {0};
        		 memset(Sdata, 0xFF, NRF_MAX_PAYLOAD_LEN);
        		 HAL_SPI_TransmitReceive(nrf.SPI_CHANNEL, Sdata, Rdata, NRF_MAX_PAYLOAD_LEN, 15);
        		 rx_buffer[Write_index].cmd = Rdata[0];
        		 rx_buffer[Write_index].length = Rdata[1];
        		 memcpy(rx_buffer[Write_index].data, &Rdata[2], NRF_MAX_PAYLOAD_LEN - 2);
            	 Write_index++;
            	 rxBuff_count++;
            	 if(Write_index == NRF_rxBuff_Size) Write_index = 0;
            	 NRF_CS_HIGH();
            }else{
            	//log_printf("[NRF_IRQ] error:Receive buffer is full!\r\n");
            	//log_printf("nrfFULL\r\n");
            }


            NRF24L01_ClearRX();//清除接收缓冲区
        }
        /* 3. 发送完成中断（TX_DS） */
        else if (status & STATUS_TX_DS){
            NRF24L01_ClearTX();  // 清空TX FIFO
            NRF24L01_EnterRxMode();//发送完成后切换为接收模式
        }
        /* 4. 重发超限中断（MAX_RT） */
        else if (status & STATUS_MAX_RT) {
        	log_printf("[NRF_IRQ] No ACK response was received!!!\r\n");
            NRF24L01_ClearTX();  // 清空TX FIFO（必须）
            NRF24L01_EnterRxMode();//发送完成后切换为接收模式
        }
        /* 5. 未知中断 */
        else {
        	log_printf("[IRQ] Unknown interruption?\r\n");
        }
        NRF24L01_WriteReg(NRF_REG_STATUS, 0x70); // 清除所有中断标志
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
    if (rxBuff_count > 0) {
    	log_printf("NRF Receive data packet:");
    	log_printf("[0x%02X] ", rx_buffer[Read_index].cmd);
    	log_printf("[0x%02X] ", rx_buffer[Read_index].length);
    	for(int i = 0; i < rx_buffer[Read_index].length; i++){
    		log_printf("0x%02X ",(uint8_t)rx_buffer[Read_index].data[i]);
    	}
    	log_printf("\r\n");
    	rxBuff_count--;
    	Read_index++;
    	if(Read_index == NRF_rxBuff_Size) Read_index = 0;
    }


}





void NRF_InformationInteraction(void){
	static uint32_t lasttime = 0;
	uint8_t databuff[32] = {0};
	if(HAL_GetTick() > lasttime + 200){

		lasttime = HAL_GetTick();
	}
}





