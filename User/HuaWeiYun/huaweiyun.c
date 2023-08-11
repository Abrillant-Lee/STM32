#include "huaweiyun.h"

uint8_t rxBuffer[1000]; // 接收缓冲区

/*
 * name  :  HuaweiIot_reset
 * brief :  HuaweiIot模块复位
 * param :  无
 * return:  无
 */
void HuaweiIot_reset(void)
{
    Usart_SendString(USARTx, "AT+RESTORE\r\n");
    Delay_Ms(5000);
}

void HuaweiIot_init(void)
{
    Usart_SendString(USARTx, "AT\r\n");
    Delay_Ms(2000);
    Usart_SendString(USARTx, "AT+CWMODE=1\r\n");
    Delay_Ms(2000);
    Usart_SendString(USARTx, "AT+CWJAP=\"STM32-32\",\"lihaonan213\"\r\n");
    Delay_Ms(2000);
    Usart_SendString(USARTx, "AT+MQTTUSERCFG=0,1,\"NULL\",\"64a8d175ae80ef457fc080fe_0001\",\"98e26b532e704613c4650e3d0376dd65da9f49f7143c9a9661089bfdf659ef98\",0,0,\"\"\r\n");
    Delay_Ms(2000);
    Usart_SendString(USARTx, "AT+MQTTCLIENTID=0,\"64a8d175ae80ef457fc080fe_0001_0_0_2023071107\"\r\n");
    Delay_Ms(2000);
}

/*
 * name  :  HuaweiIot_send
 * brief :  HuaweiIot模块发送数据
 * param :  data:发送的数据
 * return:  无
 */
void HuaweiIot_send(uint8_t data)
{
    Usart_SendString(USARTx, "AT+MQTTCONN=0,\"92e10cb0f7.st1.iotda-device.cn-north-4.myhuaweicloud.com\",1883,1\r\n");
    Delay_Ms(1500);
    Usart_SendString(USARTx, "AT+MQTTSUB=0,\"$oc/devices/64a8d175ae80ef457fc080fe_0001/sys/properties/report\",1\r\n");
    Delay_Ms(1500);
    sprintf((char *)rxBuffer, "AT+MQTTPUB=0,\"$oc/devices/64a8d175ae80ef457fc080fe_0001/sys/properties/report\",\"{\\\"services\\\":[{\\\"service_id\\\":\\\"0001\\\"\\,\\\"properties\\\":{\\\"DS18B20\\\":%d}}]}\",0,0\r\n", data);
    Usart_SendString(USARTx, (char *)rxBuffer);
    Delay_Ms(1500);
}
