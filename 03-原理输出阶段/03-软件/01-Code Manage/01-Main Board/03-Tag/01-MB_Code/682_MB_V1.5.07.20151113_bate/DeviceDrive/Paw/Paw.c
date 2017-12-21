/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: Paw.c
* ժ    Ҫ: Paw��������
*
* ��ǰ�汾: 1.0
* ��    ��: ����
* �������: 2015-08-28
* ��    ��:
* ע    ��: none                                                                  
*******************************************************************************
* ȡ���汾: 
* ��    ��:
* �������: 
* �޸�����:
* �޸��ļ�: 
*******************************************************************************
*/
/*
*******************************************************************************
*                               ����ͷ�ļ�
*******************************************************************************
*/
#include "Share.h"
#include "Paw.h"
/*
********************************************************************************
*                               �궨��
********************************************************************************
*/
/*       ����ѹLED����                         */
#define DEF_MCP23S17_SPI_SS                       (GPIO_Pin_4)
#define DEF_MCP23S17_SPI_SCK                      (GPIO_Pin_5)
#define DEF_MCP23S17_SPI_MOSI                     (GPIO_Pin_7)
#define DEF_MCP23S17_SPI_MISO                     (GPIO_Pin_6)
#define DEF_MCP23S17_SPI_PORT                     (GPIOA)

#define		    PCA9535_ADDRESS		    0xe8//(0x40)                  // ������ַ
#define		    CFG_P0_CMD			    (0x00)                  // �˿�0 ����
#define		    CFG_P1_CMD			    (0X01)                  // �˿�1 ����

#define		    STA_AS_OUTPUT		    (0x00)                  // �˿���Ϊ��� ����
#define		    OUTPUT_FF			    (0xff)                  // �˿����ȫ��Ϊ�ߵ�ƽ
#define		    OUTPUT_00			    (0x00)                  // �˿����ȫ��Ϊ�͵�ƽ

#define		    PORT0_ADDRESS		    (0x00)                  // �����˿�0 ��ַ
#define		    PORT1_ADDRESS		    (0x01)                  // �����˿�1 ��ַ
/*
********************************************************************************
*                                       �ڲ�����
********************************************************************************
*/
/*      �˿���������      */
static INT8U m_Pre[2] = {0xff, 0x7f};  
/*
********************************************************************************
********************************************************************************
*                               �ӿں���
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
* �������ƣ�PAW_PCA9535I2CDriverSta
* ����������PCB9535 IO��չоƬ ���� I2C Э��,���������״̬,SPIоƬĬ�����
*           �ļĴ�����������
* ��ڲ�����DEVICE_ADDRESS   ������ַ
*           PCA_Command      ������
*           reg0             �����Ĵ���0
*           reg1             �����Ĵ���1
* ���ڲ�����void
* ����ʱ�䣺2015-08-28
* �޸�ʱ�䣺
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
* �������ƣ�PCA9535_I2C_Driver_Port
* ����������PCB9535 IO��չоƬ ���� I2C Э��
*           �� IO�˿� ��������
* ��ڲ�����DEVICE_ADDRESS   ������ַ
*           PCA_Command      ������
*           PortNum          �˿ں�
*           PortData         �˿���������
* ���ڲ�����void
* ����ʱ�䣺2015-08-28
* �޸�ʱ�䣺
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
	PAW_MCPSpiSimWrite(0X40);		//д����
	PAW_MCPSpiSimWrite(PortNum);	//�˿�
	PAW_MCPSpiSimWrite(PortData);	//����
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
* ����ʱ�䣺2015-08-28
* �޸�ʱ�䣺
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
    
    /*����ѹLED*/
    GPIO_InitStructure.GPIO_Pin   = DEF_CTR_PRE_LED_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEF_CTR_PRE_LED_PORT, &GPIO_InitStructure); 
    
    /*ʹ�� ����ѹ LED*/
    GPIO_SetBits(DEF_CTR_PRE_LED_PORT, DEF_CTR_PRE_LED_PIN);   
}

/*
********************************************************************************
* ��������: PAW_LedConfig
* ��������: ��������ѹLed��
* ��ڲ���: void
* ���ڲ���: void
* �� �� ֵ: void
* ��    ��: 2015-08-28
* �޸�����:
* �޸�����:
********************************************************************************
*/
void PAW_LedConfig(void)
{
    PAW_PCA9535Init();
}
/*
****************************************************************************************
* ��������: PAW_LedSetOne
* ��������: �򿪻��߹ر�����ѹһ��LED��     0~7,�߱����Ƶ�6�ƣ�     9~15,5�Ƶ��ͱ����ơ�
* ��ڲ���: void
* ���ڲ���: void
* �� �� ֵ: void
* ��    ��: 2015-08-28
* �޸�����:
* �޸�����:
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
* ��������: PAW_LedSet
* ��������: ͬһʱ��ֻ��һ������ѹѹ��ֵLED��
* ��ڲ���: void
* ���ڲ���: void
* �� �� ֵ: void
* ��    ��: 2015-08-28
* �޸�����:
* �޸�����:
********************************************************************************
*/
void PAW_LedSet(INT32U num)
{
    PAW_LedSetAllOff();
    PAW_LedSetOne((13-num), DEF_PAW_LED_ON);
}
/*
********************************************************************************
* ��������: PAW_LedSetAllOff
* ��������: �ر�ȫ������ѹѹ��ֵLed��
* ��ڲ���: void
* ���ڲ���: void
* �� �� ֵ: void
* ��    ��: 2015-08-28
* �޸�����:
* �޸�����:
********************************************************************************
*/
void PAW_LedSetAllOff(void)
{
    /*�˿�1����ַ��8δ��������ѹ���ߵ�6,8���ƣ��˿�0,��ַ��7δ����5������ѹ����7����*/  
    m_Pre[0] |= 0x7e;
    m_Pre[1] |= 0x7f;
    
    PAW_PCA9535I2cDriverPort(PCA9535_ADDRESS, PORT0_ADDRESS,  m_Pre[0]);
    PAW_PCA9535I2cDriverPort(PCA9535_ADDRESS, PORT1_ADDRESS,  m_Pre[1]);
}