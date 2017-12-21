/*
********************************************************************************
* 
*                       Ambulanc(Shenzhen) Tech Co.,Ltd. 
*                   Copyright (C) 2011, All Rights Reserved 
*
*
* 模块名称：i2c_ee.c
* 模块功能：基于 STM32 的硬件 i2c eeprom
* 创建日期：2009-08-15
* 创 建 者：冯耿超
* 声    明：本模块适用于任何基于uCOS操作系统的平台
* 当前版本：V1.0
* 修改版本：V1.0 该版本为创建版本，并通过AII6000A 急救呼吸机进行验证
* 注    意: 
********************************************************************************
* 替代版本:
* 修改日期: 
* 修改原因: 
* 修改内容:
* 修改结果: 
* 注    意: 
********************************************************************************
*/

/*
********************************************************************************
*                                       包含头文件
********************************************************************************
*/
#define	    PCA_MODULE
#include    <PCA9535.h>
//#include    "app.h"
/* 2011-10-25 V1.4*/
//#include    <i2c_ee.h>


/*
********************************************************************************
*                                       内部变量
********************************************************************************
*/

/*
********************************************************************************
********************************************************************************
*                                       功能模块
********************************************************************************
********************************************************************************
*/


void OSdelay(int time)
{
	while(time--)
		;
}


static void MCPSpiSimWrite(CPU_INT08U dat) 
{
       CPU_INT08U  j;
	GPIO_ResetBits(DEF_MCP23S17_SPI_PORT,DEF_MCP23S17_SPI_SCK);
       for(j = 0;j<8;j++)
	{
//        GPIO_ResetBits(DEF_MCP23S17_SPI_PORT,DEF_MCP23S17_SPI_SCK);
        if (( dat & 0x80  )== 0x80)
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
* 函数名称：PCA9535_I2C_Driver_Sta
* 函数描述：PCB9535 IO扩展芯片 基于 I2C 协议,配置输出口状态,SPI芯片默认输出
*           的寄存器驱动函数
* 入口参数：DEVICE_ADDRESS   器件地址
*           PCA_Command      命令字
*           reg0             器件寄存器0
*           reg1             器件寄存器1
* 出口参数：void
* 创建时间：2009-9-22
* 修改时间：
********************************************************************************
*/
void  PCA9535_I2C_Driver_Sta(u16 DEVICE_ADDRESS, u8 PCA_Command, u8 reg0, u8 reg1)
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
* 创建时间：2009-9-22
* 修改时间：
********************************************************************************
*/
void  PCA9535_I2C_Driver_Port(u16 DEVICE_ADDRESS, u8 PortNum, u8 PortData)
{
#if OS_CRITICAL_METHOD == 3                      //Allocate storage for CPU status register           
    OS_CPU_SR  cpu_sr = 0;
#endif 
    OS_ENTER_CRITICAL();
//    I2C_EE_ByteWrite(DEVICE_ADDRESS,&PortData,PortNum);
	(void)DEVICE_ADDRESS;
	GPIO_ResetBits(DEF_MCP23S17_SPI_PORT,DEF_MCP23S17_SPI_SS);
	MCPSpiSimWrite(0X40);		//写命令
	MCPSpiSimWrite(PortNum);	//端口
	MCPSpiSimWrite(PortData);	//数据
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
* 创建时间：2009-9-22
* 修改时间：
********************************************************************************
*/
void BSP_PCA9535_Init(void)
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
    PCA9535_I2C_Driver_Sta(PCA9535_ADDRESS, CFG_P0_CMD, STA_AS_OUTPUT, STA_AS_OUTPUT); 
    OSdelay(20);
    PCA9535_I2C_Driver_Sta(PCA9535_ADDRESS, CFG_P1_CMD, STA_AS_OUTPUT, STA_AS_OUTPUT);
    
    PCAport.P0 = 0xff;
    PCAport.P1 = OUTPUT_FF;
    PCA9535_I2C_Driver_Port(PCA9535_ADDRESS, PORT0_ADDRESS,  0xff);
    PCA9535_I2C_Driver_Port(PCA9535_ADDRESS, PORT1_ADDRESS,  0xff); 
    
    
          /*气道压LED*/
    GPIO_InitStructure.GPIO_Pin   = DEF_CTR_PRE_LED_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEF_CTR_PRE_LED_PORT, &GPIO_InitStructure); 
    GPIO_ResetBits(DEF_CTR_PRE_LED_PORT, DEF_CTR_PRE_LED_PIN);
    
    
}


/*
********************************************************************************
* 函数名称：PAW_LED_PortsDrv
* 函数描述：PCA9535 IO驱动 
* 入口参数：DEVICE_ADDRESS   器件地址
*           portsAdd         端口号起始地址
*           PortData         端口数据
* 出口参数：void
* 创建时间：2011-10-25
* 修改时间：
********************************************************************************
*/
/* 2011-10-25 V1.4*/
void PAW_LED_PortsDrv(u16 DEVICE_ADDRESS, u8 portsAdd, u8 *PortData)
{
//    I2C_EE_BufferWrite(DEVICE_ADDRESS,PortData, portsAdd, 2);
	(void)DEVICE_ADDRESS;
	(void)portsAdd;
	PCA9535_I2C_Driver_Port(PCA9535_ADDRESS, PORT0_ADDRESS,  *PortData);
	PCA9535_I2C_Driver_Port(PCA9535_ADDRESS, PORT1_ADDRESS,  *(PortData+1));
}
