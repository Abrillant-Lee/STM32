#ifndef _SERVO_H_
#define _SERVO_H_

#include "stm32f10x.h"

/*使用TIM2的通道1*/
#define GENERAL_TIM               TIM2
#define GENERAL_TIM_APBxClock_FUN RCC_APB1PeriphClockCmd
#define GENERAL_TIM_CLK           RCC_APB1Periph_TIM2
// 内部时钟为72MHz，则计数一次的时间为(7200/72000000)s=0.1ms;
#define GENERAL_TIM_Prescaler (7200 - 1)
// 产生一次计数中断的周期为20ms，与驱动舵机需要的周期相符
#define GENERAL_TIM_Period (200 - 1)

// TIM2 输出比较通道1
#define GENERAL_TIM_CHx_GPIO_CLK RCC_APB2Periph_GPIOA
#define GENERAL_TIM_CHx_PORT     GPIOA
#define GENERAL_TIM_CHx_PIN      GPIO_Pin_2

#define TIM_SetComparex TIM_SetCompare3
#define TIM_OCxInit TIM_OC3Init

void GENERAL_TIM_Init(void);
void SERVO_Angle_Control(uint16_t Comparex);

#endif // !_SERVO_H_
