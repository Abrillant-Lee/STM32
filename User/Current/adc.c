#include "adc.h"

__IO uint16_t ADC_ConvertedValue;

/*
 * name  :  ADCx_GPIO_Config
 * brief :  ADC GPIO 初始化
 * param :  无
 * return:  无
 */
static void ADCx_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_GPIO_APBxClock_FUN(ADC_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin  = ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(ADC_PORT, &GPIO_InitStructure);
}

/*
 * name  :  ADCx_Mode_Config
 * brief :  ADC 模式初始化
 * param :  无
 * return:  无
 */
static void ADCx_Mode_Config(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    ADC_APBxClock_FUN(ADC_CLK, ENABLE);
    ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;      // 只使用一个ADC，属于独立模式
    ADC_InitStructure.ADC_ScanConvMode       = DISABLE;                   // 禁止扫描模式，多通道才要，单通道不需要
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                    // 连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None; // 不用外部触发转换，软件开启即可
    ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;       // 转换结果右对齐
    ADC_InitStructure.ADC_NbrOfChannel       = 1;                         // 转换通道1个
    ADC_Init(ADCx, &ADC_InitStructure);                                   // 初始化ADC
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);                                     // 配置ADC时钟为PCLK2的8分频，即9MHz
    ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1,                        // 配置 ADC 通道转换顺序和采样时间
                             ADC_SampleTime_55Cycles5);
    ADC_ITConfig(ADCx, ADC_IT_EOC, ENABLE);     // ADC 转换结束产生中断，在中断服务程序中读取转换值
    ADC_Cmd(ADCx, ENABLE);                      // 开启ADC ，并开始转换
    ADC_ResetCalibration(ADCx);                 // 初始化ADC 校准寄存器
    while (ADC_GetResetCalibrationStatus(ADCx)) // 等待校准寄存器初始化完成
        ;
    ADC_StartCalibration(ADCx); // ADC开始校准
    while (ADC_GetCalibrationStatus(ADCx))
        ;                                   // 等待校准完成
    ADC_SoftwareStartConvCmd(ADCx, ENABLE); // 由于没有采用外部触发，所以使用软件触发ADC转换
}
/*
 * name  :  ADC_NVIC_Config
 * brief :  ADC 中断初始化
 * param :  无
 * return:  无
 */
static void ADC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    // 优先级分组
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    // 配置中断优先级
    NVIC_InitStructure.NVIC_IRQChannel                   = ADC_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * name  :  ADCx_Init
 * brief :  ADC 初始化
 * param :  无
 * return:  无
 */
void ADCx_Init(void)
{
    ADCx_GPIO_Config();
    ADCx_Mode_Config();
    ADC_NVIC_Config();
}

/*
 * name  :  ADC_Value_printf
 * brief :  ADC 值打印
 * param :  无
 * return:  无
 */
void ADC_Value_printf(void)
{
    ADC_ConvertedValueLocal = (float)ADC_ConvertedValue / 4096 * 3.3;
    printf("\r\n The current AD value = 0x%04X \r\n",
           ADC_ConvertedValue);
    printf("\r\n The current AD value = %f V \r\n",
           ADC_ConvertedValueLocal);
    printf("\r\n\r\n");
}