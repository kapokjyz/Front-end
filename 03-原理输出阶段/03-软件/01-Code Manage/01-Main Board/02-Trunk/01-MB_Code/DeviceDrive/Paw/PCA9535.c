/*
********************************************************************************
* 
*                       Ambulanc(Shenzhen) Tech Co.,Ltd. 
*                   Copyright (C) 2011, All Rights Reserved 
*
*
* ģ�����ƣ�i2c_ee.c
* ģ�鹦�ܣ����� STM32 ��Ӳ�� i2c eeprom
* �������ڣ�2009-08-15
* �� �� �ߣ��빢��
* ��    ������ģ���������κλ���uCOS����ϵͳ��ƽ̨
* ��ǰ�汾��V1.0
* �޸İ汾��V1.0 �ð汾Ϊ�����汾����ͨ��AII6000A ���Ⱥ�����������֤
* ע    ��: 
********************************************************************************
* ����汾:
* �޸�����: 
* �޸�ԭ��: 
* �޸�����:
* �޸Ľ��: 
* ע    ��: 
********************************************************************************
*/

/*
********************************************************************************
*                                       ����ͷ�ļ�
********************************************************************************
*/
#define	    PCA_MODULE
#include    <PCA9535.h>
//#include    "app.h"
/* 2011-10-25 V1.4*/
//#include    <i2c_ee.h>


/*
********************************************************************************
*                                       �ڲ�����
********************************************************************************
*/

/*
********************************************************************************
********************************************************************************
*                                       ����ģ��
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
* �������ƣ�PCA9535_I2C_Driver_Sta
* ����������PCB9535 IO��չоƬ ���� I2C Э��,���������״̬,SPIоƬĬ�����
*           �ļĴ�����������
* ��ڲ�����DEVICE_ADDRESS   ������ַ
*           PCA_Command      ������
*           reg0             �����Ĵ���0
*           reg1             �����Ĵ���1
* ���ڲ�����void
* ����ʱ�䣺2009-9-22
* �޸�ʱ�䣺
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
* �������ƣ�PCA9535_I2C_Driver_Port
* ����������PCB9535 IO��չоƬ ���� I2C Э��
*           �� IO�˿� ��������
* ��ڲ�����DEVICE_ADDRESS   ������ַ
*           PCA_Command      ������
*           PortNum          �˿ں�
*           PortData         �˿���������
* ���ڲ�����void
* ����ʱ�䣺2009-9-22
* �޸�ʱ�䣺
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
	MCPSpiSimWrite(0X40);		//д����
	MCPSpiSimWrite(PortNum);	//�˿�
	MCPSpiSimWrite(PortData);	//����
	GPIO_SetBits(DEF_MCP23S17_SPI_PORT,DEF_MCP23S17_SPI_SS);
    OS_EXIT_CRITICAL();
}
	
/*
********************************************************************************
* �������ƣ�BSP_PCA9535_Init
* ����������PCB9535 IO��չоƬ ���� I2C Э��
*           �����ĳ�ʼ������
* ��ڲ�����DEVICE_ADDRESS   ������ַ
*           PCA_Command      ������
*           PortNum          �˿ں�
*           PortData         �˿���������
* ���ڲ�����void
* ����ʱ�䣺2009-9-22
* �޸�ʱ�䣺
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
    
    
          /*����ѹLED*/
    GPIO_InitStructure.GPIO_Pin   = DEF_CTR_PRE_LED_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEF_CTR_PRE_LED_PORT, &GPIO_InitStructure); 
    GPIO_ResetBits(DEF_CTR_PRE_LED_PORT, DEF_CTR_PRE_LED_PIN);
    
    
}


/*
********************************************************************************
* �������ƣ�PAW_LED_PortsDrv
* ����������PCA9535 IO���� 
* ��ڲ�����DEVICE_ADDRESS   ������ַ
*           portsAdd         �˿ں���ʼ��ַ
*           PortData         �˿�����
* ���ڲ�����void
* ����ʱ�䣺2011-10-25
* �޸�ʱ�䣺
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
