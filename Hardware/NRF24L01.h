#ifndef __NRF24L01_DMA_H
#define __NRF24L01_DMA_H

#include "main.h"
#include <string.h>
#include <stdio.h>


/* -------------------------- NRF24L01 SPI指令集 -------------------------- */
#define NRF_CMD_R_REGISTER    0x00    // 读寄存器命令（需与寄存器地址拼接，如0x00 | 0x07 = 读STATUS寄存器）
#define NRF_CMD_W_REGISTER    0x20    // 写寄存器命令（需与寄存器地址拼接，如0x20 | 0x00 = 写CONFIG寄存器）
#define NRF_CMD_R_RX_PAYLOAD  0x61    // 读取RX FIFO有效载荷（最多32字节，地址自动递增）
#define NRF_CMD_W_TX_PAYLOAD  0xA0    // 写入TX FIFO有效载荷（最多32字节，地址自动递增）
#define NRF_CMD_FLUSH_TX      0xE1    // 清空TX FIFO缓冲区（发送失败/重发前必须调用）
#define NRF_CMD_FLUSH_RX      0xE2    // 清空RX FIFO缓冲区（接收溢出/重新接收前调用）
#define NRF_CMD_REUSE_TX_PL   0xE3    // 重用上一次的TX有效载荷（用于ACK应答包）
#define NRF_CMD_NOP           0xFF    // 空操作命令（用于读取状态寄存器、空字节传输）

/* -------------------------- NRF24L01 寄存器地址定义 -------------------------- */
#define NRF_REG_CONFIG        0x00    // 配置寄存器（核心配置：收发模式、CRC、上电状态）
#define NRF_REG_EN_AA         0x01    // 自动应答使能寄存器（控制各管道是否开启ACK确认）
#define NRF_REG_EN_RXADDR     0x02    // 接收地址使能寄存器（控制各管道是否使能接收）
#define NRF_REG_SETUP_AW      0x03    // 地址宽度设置寄存器（可选3/4/5字节，默认5字节）
#define NRF_REG_SETUP_RETR    0x04    // 自动重发配置寄存器（重发延迟+最大重发次数）
#define NRF_REG_RF_CH         0x05    // 射频通道寄存器（2.4GHz频段，0~125对应2400~2525MHz）
#define NRF_REG_RF_SETUP      0x06    // 射频参数配置寄存器（速率、发射功率、低噪声放大器）
#define NRF_REG_STATUS        0x07    // 状态寄存器（中断标志、TX/RX FIFO状态、数据管道号）
#define NRF_REG_OBSERVE_TX    0x08    // 发送观察寄存器（重发次数、丢失包计数，仅调试用）
#define NRF_REG_RPD           0x09    // 接收功率检测寄存器（检测当前频道是否有信号）
#define NRF_REG_RX_ADDR_P0    0x0A    // 接收管道0地址寄存器（5字节，需与发送方TX_ADDR一致）
#define NRF_REG_TX_ADDR       0x10    // 发送地址寄存器（5字节，需与接收方RX_ADDR_P0一致）
#define NRF_REG_RX_PW_P0      0x11    // 接收管道0有效载荷长度寄存器（0~32字节，需与发送方匹配）
#define NRF_REG_FIFO_STATUS   0x17    // FIFO状态寄存器（TX/RX FIFO空/满标志、发送状态）

/* -------------------------- STATUS寄存器位定义（0x07） -------------------------- */
#define STATUS_RX_DR          (1 << 6)    // 接收数据就绪中断标志（收到数据后置1，需手动清0）
#define STATUS_TX_DS          (1 << 5)    // 发送数据完成中断标志（发送成功+收到ACK后置1）
#define STATUS_MAX_RT         (1 << 4)    // 最大重发次数超限中断标志（重发N次仍无ACK后置1）

/* -------------------------- CONFIG寄存器位定义（0x00） -------------------------- */
#define CONFIG_MASK_RX_DR     (1 << 6)    // 屏蔽RX_DR中断（1=屏蔽，0=使能）
#define CONFIG_MASK_TX_DS     (1 << 5)    // 屏蔽TX_DS中断（1=屏蔽，0=使能）
#define CONFIG_MASK_MAX_RT    (1 << 4)    // 屏蔽MAX_RT中断（1=屏蔽，0=使能）
#define CONFIG_EN_CRC         (1 << 3)    // 使能CRC校验（1=开启，0=关闭，建议开启）
#define CONFIG_CRC0           (1 << 2)    // CRC长度选择（1=2字节CRC，0=1字节CRC）
#define CONFIG_PWR_UP         (1 << 1)    // 上电控制位（1=上电，0=掉电，配置前需掉电）
#define CONFIG_PRIM_RX        (1 << 0)    // 主模式选择（1=接收模式，0=发送模式）


/* -------------------------- 数据包帧格式配置 -------------------------- */
#define NRF_MAX_PAYLOAD_LEN   32          // NRF24L01最大有效载荷长度（硬件限制，不可超过32字节）
#define FRAME_HEAD_LEN        1           // 帧头长度（用于存放帧类型，如CMD/VIDEO/MOVE）
#define CMD_DATA_LEN          (NRF_MAX_PAYLOAD_LEN - FRAME_HEAD_LEN)  // 指令数据有效长度（31字节）


/* -------------------------- 帧类型定义 -------------------------- */
#define FRAME_TYPE_CMD        0x01        // 指令帧（用于控制命令、心跳包等文本/短指令传输）
#define FRAME_TYPE_VIDEO      0x02        // 视频帧（用于视频数据传输，可扩展为二进制数据流）
#define FRAME_TYPE_MOVE       0x03        // 运动控制帧（用于电机、云台等运动控制指令）

/* -------------------------- 射频参数配置（RF_SETUP寄存器0x06） -------------------------- */
#define NRF_RF_1MBPS_0DBM     0x07        // 射频配置：1Mbps传输速率，0dBm发射功率（默认，兼容性最好）
#define NRF_RF_2MBPS_0DBM     0x0E        // 射频配置：2Mbps传输速率，0dBm发射功率（速率更快，传输距离略短）
#define NRF_RF_250KBPS_0DBM   0x26        // 射频配置：250kbps传输速率，0dBm发射功率（速率最慢，传输距离最远）

// 数据结构
typedef struct {
    uint8_t cmd;
    uint8_t length;       // 序列号
    uint8_t data[NRF_MAX_PAYLOAD_LEN - 2]; // 数据段
} NRF_FrameTypeDef;

typedef enum{
	Receive_mode, //表示设备处于接受模式
	Transmit_mode,//表示设备处于发送模式
	Slave_mode,//表示当前设备扮演从机
	Master_mode,//表示当前设备扮演主机
}NRF_StatusTypeDef;



/* 引脚操作宏 ----------------------------------------------------------------*/
#define NRF_CS_LOW()   HAL_GPIO_WritePin(NRF_CS_GPIO_Port, NRF_CS_Pin, GPIO_PIN_RESET)   // 选中NRF24L01
#define NRF_CS_HIGH()  HAL_GPIO_WritePin(NRF_CS_GPIO_Port, NRF_CS_Pin, GPIO_PIN_SET)     // 释放NRF24L01
#define NRF_CE_LOW()   HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_RESET)   // 禁用芯片（待机模式）
#define NRF_CE_HIGH()  HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_SET)     // 使能芯片（接收/发送
typedef struct{
	SPI_HandleTypeDef *SPI_CHANNEL;//使用的数据通道
	uint16_t SPI_IRQ_Pin;//SPI_中断引脚
	GPIO_TypeDef* SPI_IRQ_GPIO_Port;//SPI_中断引脚
	NRF_FrameTypeDef *tx_buffer;
	NRF_FrameTypeDef *rx_buffer;
}NRFHandleTypeDef;




// 函数声明
uint8_t NRF24L01_Init(void);
uint8_t NRF24L01_SendDatas(uint8_t cmd, uint8_t len, uint8_t* data);
void NRF24L01_ProcessRxData(void);
void NRF_EXTI_Callback(uint16_t GPIO_Pin);
#endif // __NRF24L01_DMA_H
