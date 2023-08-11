#include "dht11.h"
#include "delay.h"

void DHT11_GPIO_Config(void);
void DHT11_Mode_IPU(void);
void DHT11_Mode_Out_PP(void);
/*
 * name  :  DHT11_GPIO_Config
 * brief :  DHT11的GPIO配置
 * param :  无
 * return:  无
 */
void DHT11_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(DHT11_PORT_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}

/**
 * @brief  DHT11 初始化函数
 * @param  无
 * @retval 无
 */
void DHT11_Init(void)
{
    DHT11_GPIO_Config();

    DHT11_Dout_1; // 拉高GPIOB10
}

/*
 * name  : DHT11_Mode_IPU
 * brief : DHT11-DATA引脚变为上拉输入模式
 * param : 无
 * return: 无
 */
void DHT11_Mode_IPU(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}

/*
 * name  :  DHT11_Mode_Out_PP
 * brief :  DHT11-DATA引脚变为推挽输出模式
 * param :  无
 * return:  无
 */
void DHT11_Mode_Out_PP(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}

/*
 * name  :  DHT11_ReadByte
 * brief :  读取一个字节
 * param :  无
 * return:  读取到的数据
 */
static uint8_t DHT11_ReadByte(void)
{
    uint8_t i, temp = 0;
    for (i = 0; i < 8; i++) {
        // 等待响应完毕
        while (DHT11_Dout_IN() == Bit_RESET)
            ;
        /*DHT11 以26~28us的高电平表示“0”，以70us高电平表示“1”，二者高电平时间都为54us
         *通过检测 x us后的电平即可区别这两个状 ，x 即下面的延时
         */
        Delay_Us(54);                   // 延时x us,这个延时需要大于数据0持续的时间即可
        if (DHT11_Dout_IN() == Bit_SET) /* x us后仍为高电平表示数据“1” */
        {
            /* 等待数据1的高电平结束 */
            while (DHT11_Dout_IN() == Bit_SET)
                ;
            temp |= (uint8_t)(0x01 << (7 - i)); // 把第7-i位置1，MSB先行
        } else                                  // x us后为低电平表示数据“0”
        {
            temp &= (uint8_t) ~(0x01 << (7 - i)); // 把第7-i位置0，MSB先行
        }
    }
    return temp;
}

/*
 * name  :  DHT11_Read_TempAndHumidity
 * brief :  读取温湿度
 * param :  DHT11_Data_TypeDef *DHT11_Data
 * return:  读取成功返回1，失败返回0
 */
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data)
{
    /*输出模式*/
    DHT11_Mode_Out_PP();
    /*主机拉低*/
    DHT11_Dout_0;
    /*延时18ms*/
    Delay_Ms(18);
    /*总线拉高 主机延时30us*/
    DHT11_Dout_1;
    Delay_Us(30); // 延时30us
    /*主机设为输入 判断从机响应信号*/
    DHT11_Mode_IPU();
    /*判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行*/
    if (DHT11_Dout_IN() == Bit_RESET) {
        /*轮询直到从机发出 的80us 低电平 响应信号结束*/
        while (DHT11_Dout_IN() == Bit_RESET)
            ;
        /*轮询直到从机发出的 80us 高电平 标置信号结束*/
        while (DHT11_Dout_IN() == Bit_SET)
            ;
        /*开始接收数据*/
        DHT11_Data->humi_int  = DHT11_ReadByte();
        DHT11_Data->humi_deci = DHT11_ReadByte();
        DHT11_Data->temp_int  = DHT11_ReadByte();
        DHT11_Data->temp_deci = DHT11_ReadByte();
        DHT11_Data->check_sum = DHT11_ReadByte();
        /*读取结束，引脚改为输出模式*/
        DHT11_Mode_Out_PP();
        /*主机拉高*/
        DHT11_Dout_1;
        /*检查读取的数据是否正确*/
        if (DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int + DHT11_Data->temp_deci)
            return SUCCESS;
        else
            return ERROR;
    } else
        return ERROR;
}

/*
 * name  :  Printf_TempAndHumidity
 * brief :  输出温湿度
 * param :  无
 * return:  无
 */
void Printf_TempAndHumidity(void)
{
    DHT11_Data_TypeDef DHT11_Data;
    /*调用DHT11_Read_TempAndHumidity读取温湿度，若成功则输出该信息*/
    if (DHT11_Read_TempAndHumidity(&DHT11_Data) == SUCCESS) {
        printf("湿度为%d.%d ％RH ，温度为 %d.%d℃ \r\n",
               DHT11_Data.humi_int, DHT11_Data.humi_deci, DHT11_Data.temp_int, DHT11_Data.temp_deci);
    } else {
        printf("Read DHT11 ERROR!\r\n");
    }
}
