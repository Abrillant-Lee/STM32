#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"

void Delay_Us(__IO uint32_t us);
void Delay_Ms(__IO uint32_t ms);

#define DHT11_IN GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) 
#endif /* __SYSTICK_H */
