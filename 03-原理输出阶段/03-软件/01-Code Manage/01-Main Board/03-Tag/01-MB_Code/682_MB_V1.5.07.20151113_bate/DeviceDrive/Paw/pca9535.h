
#ifndef PCA9535_H_
#define PCA9535_H_


/*
*********************************************************************************************************
*                                           INCLUDE FILES
*********************************************************************************************************
*/
#include <bsp.h>

/*
*********************************************************************************************************
*                                           EXTERNS
*********************************************************************************************************
*/
#ifdef   PCA_MODULE
#define  PCA_EXT
#else
#define  PCA_EXT  extern
#endif

/*
*********************************************************************************************************
*                                             DEFINES
*********************************************************************************************************
*/
#define DEF_MCP23S17_SPI_SS                       (GPIO_Pin_4)
#define DEF_MCP23S17_SPI_SCK                      (GPIO_Pin_5)
#define DEF_MCP23S17_SPI_MOSI                     (GPIO_Pin_7)
#define DEF_MCP23S17_SPI_MISO                     (GPIO_Pin_6)
#define DEF_MCP23S17_SPI_PORT                     (GPIOA)

#define		    PCA9535_ADDRESS		    0xe8//(0x40)                  // ������ַ
//#define		    CFG_P0_CMD			    (0x06)                  // �˿�0 ����
//#define		    CFG_P1_CMD			    (0X07)                  // �˿�1 ����
#define		    CFG_P0_CMD			    (0x00)                  // �˿�0 ����
#define		    CFG_P1_CMD			    (0X01)                  // �˿�1 ����


#define		    PCA9535_TEST_CMD	    (0x02)                  // ������������
    
//#define		    PORT0_ADDRESS		    (0x02)                  // �����˿�0 ��ַ
//#define		    PORT1_ADDRESS		    (0x03)                  // �����˿�1 ��ַ
#define		    PORT0_ADDRESS		    (0x00)                  // �����˿�0 ��ַ
#define		    PORT1_ADDRESS		    (0x01)                  // �����˿�1 ��ַ

#define		    STA_AS_OUTPUT		    (0x00)                  // �˿���Ϊ��� ����

#define		    OUTPUT_FF			    (0xff)                  // �˿����ȫ��Ϊ�ߵ�ƽ
#define		    OUTPUT_00			    (0x00)                  // �˿����ȫ��Ϊ�͵�ƽ

/*
*********************************************************************************************************
*                                       GLOBAL VARIABLES
*********************************************************************************************************
*/
typedef struct {
  	unsigned char P0;
	unsigned char P1;
}PCA;

PCA_EXT PCA PCAport;
/*
*********************************************************************************************************
*                                    FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void  		PCA9535_I2C_Driver_Sta		(u16 DEVICE_ADDRESS, u8 PCA_Command, u8 reg0, u8 reg1);
void  		PCA9535_I2C_Driver_Port		(u16 DEVICE_ADDRESS, u8 PortNum, u8 PortData);
void 		BSP_PCA9535_Init			(void);
/* 2011-10-25 V1.4*/
void        PAW_LED_PortsDrv            (u16 DEVICE_ADDRESS, u8 portsAdd, u8 *PortData);

#endif
