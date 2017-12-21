/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: Uart.c
* ժ    Ҫ: STM32F10XX������������
*
* ��ǰ�汾: 1.0
* ��    ��: ����
* �������: 2013-11-15
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
#include  "Uart.h"
/*
********************************************************************************
*                               �궨��
********************************************************************************
*/
#define DEF_UART_COUNT  6
#define DEF_UART_DEBUG  DEF_UART_COM2

/*
********************************************************************************
*                               �ṹ����
********************************************************************************
*/

typedef struct  
{
  USART_TypeDef *pCom;
  CLK_TypeDef   ComClk;
  RCC_XXXPeriphClockCmd ComClockCmd;
    
  PORT_TypeDef* TxPort;
  CLK_TypeDef   TxClk;  
  RCC_XXXPeriphClockCmd TxClockCmd;
  PIN_TypeDef   TxPin;
  PS_TypeDef    TxPinSource; 
    
  PORT_TypeDef*  RxPort;
  CLK_TypeDef   RxClk;  
  RCC_XXXPeriphClockCmd RxClockCmd;
  PIN_TypeDef   RxPin;
  PS_TypeDef    RxPinSource; 
  
  IRQN_TypeDef  IRQn;
  IRQINTID_TypeDef IRQIntId;
  IRQHANDLE     IRQHandle;

}UART_SOURCE;

typedef struct  
{
  uint8_t Enable;
  uint8_t Config;
  void (*receive)(uint8_t, uint8_t);
}UART_CTR;

/*
********************************************************************************
*                               �ڲ�����
********************************************************************************
*/
static void USART1_IRQHandle(void);
//static void USART2_IRQHandle(void);
static void USART3_IRQHandle(void);
//static void USART4_IRQHandle(void);
//static void USART5_IRQHandle(void);
//static void USART6_IRQHandle(void);
/*
********************************************************************************
*                               �ڲ�����
********************************************************************************
*/
/*
    Uart Source
*/
static UART_SOURCE m_UartSource[DEF_UART_COUNT]=
{
  /*      Uart1 Source      */
  {
    USART1, RCC_APB2Periph_USART1, RCC_APB2PeriphClockCmd,
    GPIOA, RCC_APB2Periph_GPIOA, RCC_APB2PeriphClockCmd, GPIO_Pin_9, GPIO_PinSource9,
    GPIOA, RCC_APB2Periph_GPIOA, RCC_APB2PeriphClockCmd, GPIO_Pin_10, GPIO_PinSource10,
    USART1_IRQChannel, BSP_INT_ID_USART1, USART1_IRQHandle 
  },
  /*      Uart2 Source      */
  {
    USART3, RCC_APB1Periph_USART3, RCC_APB1PeriphClockCmd,
    GPIOB, RCC_APB2Periph_GPIOB, RCC_APB2PeriphClockCmd, GPIO_Pin_10, GPIO_PinSource0,
    GPIOB, RCC_APB2Periph_GPIOB, RCC_APB2PeriphClockCmd, GPIO_Pin_11, GPIO_PinSource11,
    USART3_IRQChannel, BSP_INT_ID_USART3, USART3_IRQHandle 
  },
  /*      Uart3 Source      */
  {
    0
  },
  /*      Uart4 Source      */
  {
    0
  },
   /*     Uart5 Source      */
  {
    0
  },
  /*      Uart6 Source      */
  {
    0
  },  
};
/*
    Uart Control
*/
static UART_CTR m_UartCtr[DEF_UART_COUNT]=
{
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0},
};


/*
********************************************************************************
********************************************************************************
*                               ���ܺ���
********************************************************************************
********************************************************************************
*/

#if DEF_DEBUG_EN > 0u
int fputc(int ch, FILE *f)
{
    if (!m_UartCtr[DEF_UART_DEBUG].Enable)
    {
        return 0;
    }
    /* Place your implementation of fputc here */
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(m_UartSource[DEF_UART_DEBUG].pCom, USART_FLAG_TC) == RESET)
    {}

    /* write a character to the USART */
    USART_SendData(m_UartSource[DEF_UART_DEBUG].pCom, (uint8_t) ch);
    return ch;
}
#endif

/*
******************************************************************************
��������:   USART_Receive
��������:   ���ڽ��շ�����
�������:   port�����ں�	
�������:   none
�� �� ֵ:   none
��������:   2013-11-07 
ע    ��:    
*******************************************************************************
*/
static void USART_Receive(uint8_t port)
{
    uint8_t data;
	UART_SOURCE *psource;	
	UART_CTR *pctr;

	psource = &m_UartSource[port];
	pctr = &m_UartCtr[port];
    
    if(USART_GetITStatus(psource->pCom, USART_IT_RXNE) != RESET)    
    {          
        /*  Get uart data   */
        data = USART_ReceiveData(psource->pCom);
		//USART_SendData(psource->pCom, data);
		pctr->receive(port, data);
    } 
    
    /*    Clear uart inttruput  */
    USART_ClearITPendingBit(psource->pCom,  USART_IT_RXNE);    	  
}
/*
******************************************************************************
��������:   USART_IRQHandle
��������:   �����жϷ�����
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2013-11-07 
ע    ��:    
*******************************************************************************
*/
static void USART1_IRQHandle(void)
{
	USART_Receive(DEF_UART_COM1);    
}
//static void USART2_IRQHandle(void){;}
static void USART3_IRQHandle(void){;}
//static void USART4_IRQHandle(void){;}
//static void USART5_IRQHandle(void){;}
//static void USART6_IRQHandle(void){;}

/*
******************************************************************************
��������:   UART_Configuration
��������:   Config uart
�������:   com��uart id
�������:   none
�� �� ֵ:   none
��������:   2013-11-07 
ע    ��:    
*******************************************************************************
*/
static void UART_Configuration(uint8_t com)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure; 
    NVIC_InitTypeDef NVIC_InitStructure;
    
    UART_SOURCE *psource;
    
    m_UartCtr[com].Enable = 1;
    /*
        Get uart source
    */
    psource = &m_UartSource[com];
      
    /*      
        Config Uart clock      
    */
    psource->ComClockCmd(psource->ComClk,ENABLE);
    
    /*      
        Config TX:
        1:ʱ��
        2:�˿�
        3:�ܽ�
        4:AF
    */
    psource->RxClockCmd(psource->TxClk,ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = psource->TxPin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    
    GPIO_Init(psource->TxPort, &GPIO_InitStructure);   
    /*      
        Config RX:
        1:ʱ��
        2:�˿�
        3:�ܽ�
        4:AF
    */
    psource->RxClockCmd(psource->RxClk,ENABLE);
  
    GPIO_InitStructure.GPIO_Pin = psource->RxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    
    GPIO_Init(psource->RxPort, &GPIO_InitStructure);
 
    /*      
        Config Uart:
        1:������ = 115200
        2:����λ = 8
        3:ֹͣλ = 1
        4:У��λ = No 
        5:������λ = No
        6:ģʽ = Rx��Tx
    */
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(psource->pCom, &USART_InitStructure);
    
    /*
        Enble Rx interrupt
    */
	
/*
	��ΪСԬͬѧ������Ĵ��ں͵�Դ��Ĵ��ڷ�������һ���ˣ�
	�������ǵ�Դ�����Ῠ���ڴ����ж��У�������ʱ���ε�Դ�������Ĵ��ڽ��ն�
*/
 //   USART_ITConfig(psource->pCom,USART_IT_RXNE,ENABLE);
    USART_HalfDuplexCmd(psource->pCom,ENABLE);	
    /*
        Enble Rx Uart
    */
    USART_Cmd(psource->pCom, ENABLE);
    
    /*
        Config interrupt
    */
    NVIC_InitStructure.NVIC_IRQChannel = psource->IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    BSP_IntVectSet(psource->IRQIntId, psource->IRQHandle);	
}
/*
******************************************************************************
��������:	UART_Write
��������:   ���ʹ�������
�������:	com�����ں�
			data������ָ��
			size�����ݳ���
�������:	none
�� �� ֵ:   none
��������:   2013-11-08
ע    ��:    
*******************************************************************************
*/
void UART_Write(uint8_t com, uint8_t *data, uint16_t size)
{
    uint8_t i=0;
    
    /*
        �жϴ����Ƿ��
    */
    if (!m_UartCtr[com].Enable)
    {
        return;
    }
        
    while(i<size)
    {
        USART_SendData(m_UartSource[com].pCom, *data++);
        while (USART_GetFlagStatus(m_UartSource[com].pCom, USART_FLAG_TXE) == RESET);
        i++;
    }
}
/*
******************************************************************************
��������:	UART_Bind
��������:   �󶨴���
�������:	port�����ڶ˿ں�
			receive�����ݽ��շ�����
�������:	none
�� �� ֵ:   0���󶨴���1��������
��������:   2013-12-12
ע    ��:    
*******************************************************************************
*/
uint8_t UART_Bind(uint8_t com, void (*receive)(uint8_t, uint8_t))
{
  	/*
        �жϴ����Ƿ��
    */
    if (!m_UartCtr[com].Enable)
    {
        return 0;
    }
	
	m_UartCtr[com].receive = receive;
	BSP_IntVectSet(m_UartSource[com].IRQIntId, m_UartSource[com].IRQHandle);
	return 1;
}
/*
******************************************************************************
��������:   UART_Config
��������:   ��ʼ������
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2013-11-07 
ע    ��:    
*******************************************************************************
*/
void UART_Config(void)
{
    UART_Configuration(DEF_UART_COM1);
	UART_Configuration(DEF_UART_DEBUG);	
}