#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

#define ADC_APBxClock_FUN RCC_APB2PeriphClockCmd
#define ADCx              ADC2
#define ADC_CLK           RCC_APB2Periph_ADC2

// ADC GPIO宏定义
// 注意：用作ADC采集的IO必须没有复用，否则采集电压会有影响
#define ADC_GPIO_APBxClock_FUN RCC_APB2PeriphClockCmd
#define ADC_GPIO_CLK           RCC_APB2Periph_GPIOA
#define ADC_PORT               GPIOA
#define ADC_PIN                GPIO_Pin_0
// ADC 通道宏定义
#define ADC_CHANNEL ADC_Channel_0

// ADC 中断相关宏定义
#define ADC_IRQ        ADC1_2_IRQn
#define ADC_IRQHandler ADC1_2_IRQHandler

// 局部变量，用于保存转换计算后的电压值
float ADC_ConvertedValueLocal;

void ADCx_Init(void);
void ADC_Value_printf(void)

#endif /* __ADC_H */
