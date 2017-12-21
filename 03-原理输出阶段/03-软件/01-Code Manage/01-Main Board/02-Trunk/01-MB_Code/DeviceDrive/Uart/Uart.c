/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: Uart.c
* 摘    要: STM32F10XX串口驱动程序
*
* 当前版本: 1.0
* 作    者: 黎明
* 完成日期: 2013-11-15
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
#include  "Uart.h"
/*
********************************************************************************
*                               宏定义
********************************************************************************
*/
#define DEF_UART_COUNT  6
#define DEF_UART_DEBUG  DEF_UART_COM2

/*
********************************************************************************
*                               结构定义
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
*                               内部函数
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
*                               内部变量
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
*                               功能函数
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
函数名称:   USART_Receive
函数功能:   串口接收服务函数
输入参数:   port：串口号	
输出参数:   none
返 回 值:   none
创建日期:   2013-11-07 
注    意:    
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
函数名称:   USART_IRQHandle
函数功能:   串口中断服务函数
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2013-11-07 
注    意:    
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
函数名称:   UART_Configuration
函数功能:   Config uart
输入参数:   com：uart id
输出参数:   none
返 回 值:   none
创建日期:   2013-11-07 
注    意:    
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
        1:时钟
        2:端口
        3:管脚
        4:AF
    */
    psource->RxClockCmd(psource->TxClk,ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = psource->TxPin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    
    GPIO_Init(psource->TxPort, &GPIO_InitStructure);   
    /*      
        Config RX:
        1:时钟
        2:端口
        3:管脚
        4:AF
    */
    psource->RxClockCmd(psource->RxClk,ENABLE);
  
    GPIO_InitStructure.GPIO_Pin = psource->RxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    
    GPIO_Init(psource->RxPort, &GPIO_InitStructure);
 
    /*      
        Config Uart:
        1:波特率 = 115200
        2:数据位 = 8
        3:停止位 = 1
        4:校验位 = No 
        5:流控制位 = No
        6:模式 = Rx和Tx
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
	因为小袁同学将主板的串口和电源板的串口发送连在一起了，
	导致又是电源板程序会卡死在串口中断中，所以暂时屏蔽电源板对主板的串口接收端
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
函数名称:	UART_Write
函数功能:   发送串口数据
输入参数:	com：串口号
			data：数据指针
			size：数据长度
输出参数:	none
返 回 值:   none
创建日期:   2013-11-08
注    意:    
*******************************************************************************
*/
void UART_Write(uint8_t com, uint8_t *data, uint16_t size)
{
    uint8_t i=0;
    
    /*
        判断串口是否打开
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
函数名称:	UART_Bind
函数功能:   绑定串口
输入参数:	port：串口端口号
			receive：数据接收服务函数
输出参数:	none
返 回 值:   0：绑定错误，1：绑定正常
创建日期:   2013-12-12
注    意:    
*******************************************************************************
*/
uint8_t UART_Bind(uint8_t com, void (*receive)(uint8_t, uint8_t))
{
  	/*
        判断串口是否打开
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
函数名称:   UART_Config
函数功能:   初始化串口
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2013-11-07 
注    意:    
*******************************************************************************
*/
void UART_Config(void)
{
    UART_Configuration(DEF_UART_COM1);
	UART_Configuration(DEF_UART_DEBUG);	
}