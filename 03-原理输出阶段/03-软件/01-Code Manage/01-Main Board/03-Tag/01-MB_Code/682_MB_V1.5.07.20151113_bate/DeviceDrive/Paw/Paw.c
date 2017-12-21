/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: Paw.c
* 摘    要: Paw驱动程序
*
* 当前版本: 1.0
* 作    者: 曾健
* 完成日期: 2015-08-28
* 内    容:
* 注    意: none                                                                  
*******************************************************************************
* 取代版本: 
* 作    者:
* 完成日期: 
* 修改内容:
* 修改文件: 
*******************************************************************************
*/
/*
*******************************************************************************
*                               包含头文件
*******************************************************************************
*/
#include "Share.h"
#include "Paw.h"
/*
********************************************************************************
*                               宏定义
********************************************************************************
*/
/*       气道压LED控制                         */
#define DEF_MCP23S17_SPI_SS                       (GPIO_Pin_4)
#define DEF_MCP23S17_SPI_SCK                      (GPIO_Pin_5)
#define DEF_MCP23S17_SPI_MOSI                     (GPIO_Pin_7)
#define DEF_MCP23S17_SPI_MISO                     (GPIO_Pin_6)
#define DEF_MCP23S17_SPI_PORT                     (GPIOA)

#define		    PCA9535_ADDRESS		    0xe8//(0x40)                  // 器件地址
#define		    CFG_P0_CMD			    (0x00)                  // 端口0 命令
#define		    CFG_P1_CMD			    (0X01)                  // 端口1 命令

#define		    STA_AS_OUTPUT		    (0x00)                  // 端口作为输出 命令
#define		    OUTPUT_FF			    (0xff)                  // 端口输出全部为高电平
#define		    OUTPUT_00			    (0x00)                  // 端口输出全部为低电平

#define		    PORT0_ADDRESS		    (0x00)                  // 器件端口0 地址
#define		    PORT1_ADDRESS		    (0x01)                  // 器件端口1 地址
/*
********************************************************************************
*                                       内部变量
********************************************************************************
*/
/*      端口数据内容      */
static INT8U m_Pre[2] = {0xff, 0x7f};  
/*
********************************************************************************
********************************************************************************
*                               接口函数
********************************************************************************
********************************************************************************
*/
static void PAW_OSdelay(INT32U time)
{
	while(time--);
}

static void PAW_MCPSpiSimWrite(INT8U dat) 
{
    INT8U j;
    
	GPIO_ResetBits(DEF_MCP23S17_SPI_PORT,DEF_MCP23S17_SPI_SCK);
    for(j = 0;j<8;j++)
	{
        if ((dat & 0x80)== 0x80)
		{            
            GPIO_SetBits(DEF_MCP23S17_SPI_PORT,DEF_MCP23S17_SPI_MOSI);
        }
		else
		{ 
            GPIO_ResetBits(DEF_MCP23S17_SPI_PORT,DEF_MCP23S17_SPI_MOSI);
        }
        GPIO_SetBits(DEF_MCP23S17_SPI_PORT,DEF_MCP23S17_SPI_SCK);
        GPIO_ResetBits(DEF_MCP23S17_SPI_PORT,DEF_MCP23S17_SPI_SCK);
        dat <<= 1;
    }
	GPIO_SetBits(DEF_MCP23S17_SPI_PORT,DEF_MCP23S17_SPI_MOSI);  
}
/*
********************************************************************************
* 函数名称：PAW_PCA9535I2CDriverSta
* 函数描述：PCB9535 IO扩展芯片 基于 I2C 协议,配置输出口状态,SPI芯片默认输出
*           的寄存器驱动函数
* 入口参数：DEVICE_ADDRESS   器件地址
*           PCA_Command      命令字
*           reg0             器件寄存器0
*           reg1             器件寄存器1
* 出口参数：void
* 创建时间：2015-08-28
* 修改时间：
********************************************************************************
*/
static void  PAW_PCA9535I2CDriverSta(INT16U DEVICE_ADDRESS, INT8U PCA_Command, u8 reg0, INT8U reg1)
{
    (void)DEVICE_ADDRESS;    
    (void)(PCA_Command);
    (void)(reg0);
    (void)(reg1);
}
/*
********************************************************************************
* 函数名称：PCA9535_I2C_Driver_Port
* 函数描述：PCB9535 IO扩展芯片 基于 I2C 协议
*           的 IO端口 驱动函数
* 入口参数：DEVICE_ADDRESS   器件地址
*           PCA_Command      命令字
*           PortNum          端口号
*           PortData         端口数据内容
* 出口参数：void
* 创建时间：2015-08-28
* 修改时间：
********************************************************************************
*/
static void  PAW_PCA9535I2cDriverPort(INT16U DEVICE_ADDRESS, INT8U PortNum, INT8U PortData)
{
#if OS_CRITICAL_METHOD == 3                      //Allocate storage for CPU status register           
    OS_CPU_SR  cpu_sr = 0;
#endif 
    OS_ENTER_CRITICAL();
	(void)DEVICE_ADDRESS;
	GPIO_ResetBits(DEF_MCP23S17_SPI_PORT,DEF_MCP23S17_SPI_SS);
	PAW_MCPSpiSimWrite(0X40);		//写命令
	PAW_MCPSpiSimWrite(PortNum);	//端口
	PAW_MCPSpiSimWrite(PortData);	//数据
	GPIO_SetBits(DEF_MCP23S17_SPI_PORT,DEF_MCP23S17_SPI_SS);
    OS_EXIT_CRITICAL();
}
/*
********************************************************************************
* 函数名称：BSP_PCA9535_Init
* 函数描述：PCB9535 IO扩展芯片 基于 I2C 协议
*           驱动的初始化函数
* 入口参数：DEVICE_ADDRESS   器件地址
*           PCA_Command      命令字
*           PortNum          端口号
*           PortData         端口数据内容
* 出口参数：void
* 创建时间：2015-08-28
* 修改时间：
********************************************************************************
*/
static void PAW_PCA9535Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    /* Enable GPIOC clock*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin   =DEF_MCP23S17_SPI_SCK | DEF_MCP23S17_SPI_MOSI | DEF_MCP23S17_SPI_SS;
    GPIO_Init(DEF_MCP23S17_SPI_PORT, &GPIO_InitStructure);
    GPIO_SetBits(DEF_MCP23S17_SPI_PORT,DEF_MCP23S17_SPI_SCK | DEF_MCP23S17_SPI_MOSI | DEF_MCP23S17_SPI_SS);
	  
    /*config the port0 and port1 as output*/
    PAW_PCA9535I2CDriverSta(PCA9535_ADDRESS, CFG_P0_CMD, STA_AS_OUTPUT, STA_AS_OUTPUT); 
    PAW_OSdelay(20);
    PAW_PCA9535I2CDriverSta(PCA9535_ADDRESS, CFG_P1_CMD, STA_AS_OUTPUT, STA_AS_OUTPUT);
    
    PAW_PCA9535I2cDriverPort(PCA9535_ADDRESS, PORT0_ADDRESS,  0xff);
    PAW_PCA9535I2cDriverPort(PCA9535_ADDRESS, PORT1_ADDRESS,  0xff); 
    
    /*气道压LED*/
    GPIO_InitStructure.GPIO_Pin   = DEF_CTR_PRE_LED_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEF_CTR_PRE_LED_PORT, &GPIO_InitStructure); 
    
    /*使能 气道压 LED*/
    GPIO_SetBits(DEF_CTR_PRE_LED_PORT, DEF_CTR_PRE_LED_PIN);   
}

/*
********************************************************************************
* 函数名称: PAW_LedConfig
* 函数功能: 配置气道压Led灯
* 入口参数: void
* 出口参数: void
* 返 回 值: void
* 日    期: 2015-08-28
* 修改问题:
* 修改内容:
********************************************************************************
*/
void PAW_LedConfig(void)
{
    PAW_PCA9535Init();
}
/*
****************************************************************************************
* 函数名称: PAW_LedSetOne
* 函数功能: 打开或者关闭气道压一个LED灯     0~7,高报警灯到6灯；     9~15,5灯到低报警灯。
* 入口参数: void
* 出口参数: void
* 返 回 值: void
* 日    期: 2015-08-28
* 修改问题:
* 修改内容:
*****************************************************************************************
*/
void PAW_LedSetOne(INT32U num, INT8U status)
{    
    if(status)
    {      
	    if(num >= 8)
        {
            num++;
        }
        if(num < 8)
        {
            m_Pre[1] &= ~(0x1<<(7-num));
            PAW_PCA9535I2cDriverPort(PCA9535_ADDRESS, PORT1_ADDRESS,  m_Pre[1]);
        }
        else
        {
            m_Pre[0] &= ~(0x1<<(15-num));
            PAW_PCA9535I2cDriverPort(PCA9535_ADDRESS, PORT0_ADDRESS,  m_Pre[0]);
        }	     
    }
    else
    {
        if(num>=8)
	    {
	        num++;
	    }
        if(num < 8)
        {
            m_Pre[1] |= 0x1<<(7-num);
            PAW_PCA9535I2cDriverPort(PCA9535_ADDRESS, PORT1_ADDRESS,  m_Pre[1]);
        }
        else
        {
            m_Pre[0] |= 0x1<<(15-num);
            PAW_PCA9535I2cDriverPort(PCA9535_ADDRESS, PORT0_ADDRESS,  m_Pre[0]);
        }    
    }

}
/*
********************************************************************************
* 函数名称: PAW_LedSet
* 函数功能: 同一时刻只打开一个气道压压力值LED灯
* 入口参数: void
* 出口参数: void
* 返 回 值: void
* 日    期: 2015-08-28
* 修改问题:
* 修改内容:
********************************************************************************
*/
void PAW_LedSet(INT32U num)
{
    PAW_LedSetAllOff();
    PAW_LedSetOne((13-num), DEF_PAW_LED_ON);
}
/*
********************************************************************************
* 函数名称: PAW_LedSetAllOff
* 函数功能: 关闭全部气道压压力值Led灯
* 入口参数: void
* 出口参数: void
* 返 回 值: void
* 日    期: 2015-08-28
* 修改问题:
* 修改内容:
********************************************************************************
*/
void PAW_LedSetAllOff(void)
{
    /*端口1，地址后8未控制气道压过高到6,8个灯；端口0,地址后7未控制5到气道压过低7个灯*/  
    m_Pre[0] |= 0x7e;
    m_Pre[1] |= 0x7f;
    
    PAW_PCA9535I2cDriverPort(PCA9535_ADDRESS, PORT0_ADDRESS,  m_Pre[0]);
    PAW_PCA9535I2cDriverPort(PCA9535_ADDRESS, PORT1_ADDRESS,  m_Pre[1]);
}