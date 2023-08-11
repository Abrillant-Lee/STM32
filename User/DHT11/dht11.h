#ifndef _DHT11_H_
#define _DHT11_H_

#include "stm32f10x.h"
#include "usart.h"
typedef struct
{
    uint8_t humi_int;  // 湿度的整数部分
    uint8_t humi_deci; // 湿度的小数部分
    uint8_t temp_int;  // 温度的整数部分
    uint8_t temp_deci; // 温度的小数部分
    uint8_t check_sum; // 校验和

} DHT11_Data_TypeDef;

#define DHT11_PORT      GPIOE
#define DHT11_PORT_CLK  RCC_APB2Periph_GPIOE
#define DHT11_PIN       GPIO_Pin_6

#define DHT11_Dout_0    GPIO_ResetBits(DHT11_PORT, DHT11_PIN)
#define DHT11_Dout_1    GPIO_SetBits(DHT11_PORT, DHT11_PIN)
#define DHT11_Dout_IN() GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN)

void DHT11_Init(void);
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data);
void Printf_TempAndHumidity(void);

#endif /*_DHT11_H_*/
