#include "encoder.h"

/*
 * name  :  Encoder_TIM_NVIC_Config
 * brief :  编码器中断优先级配置
 * param :  无
 * return:  无
 */
static void Encoder_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    // 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    // 设置主优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    // 设置抢占优先级为3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * name  :  Encoder_GPOI_Config
 * brief :  编码器GPIO初始化
 * param :  无
 * return:  无
 */
void Encoder_GPOI_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/*
 * name  :  Encoder_TIM2_Mode_Config
 * brief :  编码器定时器初始化
 * param :  无
 * return:  无
 */
void Encoder_TIM2_Mode_Config(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // 开启定时器2时钟

    /*--------------------时基结构体初始化-------------------------*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period = 65535;
    // 驱动CNT计数器的时钟 = Fck_int/(psc+1)
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    // 时钟分频因子 ，配置死区时间时需要用到
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    // 计数器计数模式，设置为向上计数
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    // 重复计数器的值，没用到不用管
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    // 初始化定时器
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 4.编码器配置: 定时器2，模式3，上升沿
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    // 5.输入捕获配置
    /*--------------------输入捕获结构体初始化-------------------*/
    TIM_ICInitTypeDef TIM_ICInitStructure;
    // 编码器接口通道一设置
    TIM_ICInitStructure.TIM_Channel     = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter    = 0;
    // 编码器接口通道二设置
    TIM_ICInitStructure.TIM_Channel     = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter    = 0;
    TIM_ICInit(TIM2, &TIM_ICInitStructure);
    // 清除定时器溢出更新标志位(清除计数值)
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    // 定时器2，溢出更新，使能
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    // 定时数据清零(输入捕获的值从0开始计数)
    TIM_SetCounter(TIM2, 0);
    // 定时器2使能
    TIM_Cmd(TIM2, ENABLE);
}
/*
 * name  :  Read_Speed
 * brief :  读取编码器的速度值
 * param :  无
 * return:  无
 */
u32 Read_Speed(void)
{
    int value_1;
    // 单周期位移作为速度值
    value_1 = (short)TIM_GetCounter(TIM2); // 采集编码器的计数值并保存
    TIM_SetCounter(TIM2, 0);               // 将定时器的计数值清零
    return value_1;
}

/*
 * name  :  Encoder_TIM2_Init
 * brief :  编码器初始化
 * param :  无
 * return:  无
 */
void Encoder_TIM2_Init(void)
{
    Encoder_GPOI_Config();
    Encoder_TIM2_Mode_Config();
    Encoder_TIM_NVIC_Config();
}

volatile uint32_t Encoder_Overflow_Count = 0;
void printf_speed(void)
{
    /* 电机旋转方向 */
    int8_t motor_direction = 0;
    /* 当前时刻总计数值 */
    int32_t capture_count = 0;
    /* 上一时刻总计数值 */
    static int32_t last_count = 0;
    /* 单位时间内总计数值 */
    int32_t count_per_unit = 0;
    /* 电机转轴转速 */
    float shaft_speed = 0.0f;
    /* 累积圈数 */
    float number_of_rotations = 0.0f;
    /* 20ms计算一次 */

    /* 电机旋转方向 = 计数器计数方向 */
    motor_direction = (TIM1->CR1 & TIM_CR1_DIR) ? -1 : 1;

    /* 当前时刻总计数值 = 计数器值 + 计数溢出次数 * ENCODER_TIM_PERIOD */
    capture_count = TIM1->CNT + (Encoder_Overflow_Count * 65535);

    /* 单位时间内总计数值 = 当前时刻总计数值 - 上一时刻总计数值 */
    count_per_unit = capture_count - last_count;

    /* 转轴转速 = 单位时间内的计数值 / 编码器总分辨率 * 时间系数 */
    shaft_speed = (float)count_per_unit / ENCODER_TOTAL_RESOLUTION * 50;

    /* 累积圈数 = 当前时刻总计数值 / 编码器总分辨率 */
    number_of_rotations = (float)capture_count / ENCODER_TOTAL_RESOLUTION;

    /* 记录当前总计数值，供下一时刻计算使用 */
    last_count = capture_count;

    printf("\r\n电机方向：%d\r\n", motor_direction);
    printf("单位时间内有效计数值：%d\r\n", (count_per_unit < 0 ? abs(count_per_unit) : count_per_unit));
    printf("步进电机转速：%.2f 转/秒\r\n", shaft_speed);
    printf("累计圈数：%.2f 圈\r\n", number_of_rotations);
    Delay_Ms(1000);
}
