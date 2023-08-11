#include "delay.h"


void Delay_Us(__IO uint32_t us)
{
    uint32_t i;
    SysTick_Config(SystemCoreClock / 1000000);

    for (i = 0; i < us; i++) {
        // 当计数器的值减小到0的时候，CRTL寄存器的位16会置1
        while (!((SysTick->CTRL) & (1 << 16)))
            ;
    }
    // 关闭SysTick定时器
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void Delay_Ms(__IO uint32_t ms)
{
    uint32_t i;
    SysTick_Config(SystemCoreClock / 1000);

    for (i = 0; i < ms; i++) {
        // 当计数器的值减小到0的时候，CRTL寄存器的位16会置1
        // 当置1时，读取该位会清0
        while (!((SysTick->CTRL) & (1 << 16)))
            ;
    }
    // 关闭SysTick定时器
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/*********************************************END OF FILE**********************/
