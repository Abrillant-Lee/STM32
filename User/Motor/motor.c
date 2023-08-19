#include "motor.h"

/*
 * name  :  GENERAL_TIM_GPIO_Config
 * brief :  TIM2_CH1引脚配置
 * param :  无
 * return:  无
 */
static void GENERAL_TIM_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // 输出比较通道1 GPIO 初始化
    RCC_APB2PeriphClockCmd(GENERAL_TIM_CHx_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GENERAL_TIM_CHx_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GENERAL_TIM_CHx_PORT, &GPIO_InitStructure);
}

/*
 * name  :  GPIO_DIR_Config
 * brief :  DIR引脚配置
 * param :  无
 * return:  无
 */
void GPIO_DIR_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(DIR_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = DIR_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DIR_PORT, &GPIO_InitStructure);
}

/*
 * name  :  GENERAL_TIM_Mode_Config
 * brief :  TIM2模式配置
 * param :  无
 * return:  无
 */
static void GENERAL_TIM_Mode_Config(void)
{
    // 开启定时器时钟,即内部时钟CK_INT=72M
    GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK, ENABLE);

    /*--------------------时基结构体初始化-------------------------*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period = GENERAL_TIM_Period;
    // 驱动CNT计数器的时钟 = Fck_int/(psc+1)
    TIM_TimeBaseStructure.TIM_Prescaler = GENERAL_TIM_Prescaler;
    // 时钟分频因子 ，配置死区时间时需要用到
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    // 计数器计数模式，设置为向上计数
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    // 重复计数器的值，没用到不用管
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    // 初始化定时器
    TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);

    /*--------------------输出比较结构体初始化-------------------*/
    TIM_OCInitTypeDef TIM_OCInitStructure;
    // 配置为PWM模式1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    // 输出使能
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    // 输出通道电平极性配置
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    // 输出比较通道 1
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCxInit(GENERAL_TIM, &TIM_OCInitStructure);
    TIM_ARRPreloadConfig(GENERAL_TIM, ENABLE);

    // 使能计数器
    TIM_Cmd(GENERAL_TIM, ENABLE);
}

/*
 * name  :  GENERAL_TIM_Init
 * brief :  TIM2初始化
 * param :  无
 * return:  无
 */
void GENERAL_TIM_Init(void)
{
    GENERAL_TIM_GPIO_Config();
    GENERAL_TIM_Mode_Config();
    // 电机方向引脚初始化
    GPIO_DIR_Config();
}

/*
 * name  :  SERVO_Angle_Control
 * brief :  占空比配置
 * param :  Comparex:占空比
 * return:  无
 */
void Duty_Cycle_Control(uint16_t Comparex)
{
    TIM_SetComparex(GENERAL_TIM, Comparex);
}
