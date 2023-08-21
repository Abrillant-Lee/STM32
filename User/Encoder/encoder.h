#ifndef _ENCODER_H
#define _ENCODER_H

#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"

/* 编码器物理分辨率 */
#define ENCODER_RESOLUTION 600

/* 经过倍频之后的总分辨率 */
#define ENCODER_TOTAL_RESOLUTION (ENCODER_RESOLUTION * 4) /* 4倍频后的总分辨率 */

void Encoder_TIM2_Init(void);
u32 Read_Speed(void);
void printf_speed(void);
#endif // !_ENCODER_H
