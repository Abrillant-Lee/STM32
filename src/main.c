#include "stm32f10x.h"
#include "dht11.h"
#include "usart.h"
#include "delay.h"
#include "huaweiyun.h"
#include "motor.h"

int main(void)
{
    // /*初始化串口*/
    // USART_Config();
    // /*初始化DTT11的引脚*/
    // DHT11_Init();
    // /*初始化TIMx*/
    GENERAL_TIM_Init();
    SERVO_Angle_Control(15);
    while (1) {
        // Printf_TempAndHumidity();
        // Delay_Ms(2000);
    }
}
