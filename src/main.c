#include "stm32f10x.h"
#include "dht11.h"
#include "usart.h"
#include "delay.h"
#include "huaweiyun.h"
#include "motor.h"
#include "encoder.h"
extern __IO uint16_t ADC_ConvertedValue;

int main(void)
{
    /*初始化串口*/
    USART_Config();
    /*初始化DTT11的引脚*/
    DHT11_Init();
    /*初始化TIMx*/
    GENERAL_TIM_Init();
    /*初始化定时器输出比较通道1的GPIO*/
    Duty_Cycle_Control(150);

    // 编码器初始化
    Encoder_TIM2_Init();
    // ADC 初始化
    ADCx_Init();

    while (1) {
    }
}
