/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: Power.c
* ժ    Ҫ: PowerSTM32F10XX��������
*
* ��ǰ�汾: 1.0
* ��    ��: ����
* �������: 2015-09-10
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
#include "Power.h"
/*
********************************************************************************
*                               �궨��
********************************************************************************
*/
/*
    Power Switch count
*/
#define DEF_POWER_SWITCH_COUNT   5
/*
    Power Status count
*/
#define DEF_POWER_STATUS_COUNT   2
/*
    Power Value count
*/
#define DEF_POWER_VALUE_COUNT    7

/*
********************************************************************************
*                               ���Ͷ���
********************************************************************************
*/
/*      Power Source      */
typedef struct
{
  PORT_TypeDef *Port;
  PIN_TypeDef  Pin;
  CLK_TypeDef  Clk;
  RCC_XXXPeriphClockCmd ClockCmd; 
}POWER_SOURCE;
/*
********************************************************************************
*                               �ڲ�����
********************************************************************************
*/
/*      
    Power Switch Source      
*/
static POWER_SOURCE m_PowerSwithSource[DEF_POWER_SWITCH_COUNT]=
{   
    /*       ��Դʹ��  */
    {GPIOA, GPIO_Pin_8, RCC_APB2Periph_GPIOA, RCC_APB2PeriphClockCmd},
    
    /*       ��ŷ���Դʹ��   */
    {GPIOC, GPIO_Pin_8, RCC_APB2Periph_GPIOC, RCC_APB2PeriphClockCmd},
    
    /*       ��ŷ����ؿ���ʹ��   */
	{GPIOB, GPIO_Pin_9, RCC_APB2Periph_GPIOB, RCC_APB2PeriphClockCmd},  
	
    /*       ���оƬʹ��   */ 
	{GPIOB, GPIO_Pin_0, RCC_APB2Periph_GPIOB, RCC_APB2PeriphClockCmd},  
	
	/*       �����ȶ�       */
    {GPIOA, GPIO_Pin_0, RCC_APB2Periph_GPIOA, RCC_APB2PeriphClockCmd},     
};
/*
********************************************************************************
********************************************************************************
*                               ���ܺ���
********************************************************************************
********************************************************************************
*/
/*
******************************************************************************
��������:   POWER_SwitchGpioConfig
��������:   ���õ�Դ����GPIO
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-09-10
ע    ��:   none
*******************************************************************************
*/
static void POWER_SwitchGpioConfig(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    uint8_t i;
    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    for(i=0; i<DEF_POWER_SWITCH_COUNT; i++)
    {     
        /*      ����ʱ��    */
        m_PowerSwithSource[i].ClockCmd(m_PowerSwithSource[i].Clk, ENABLE);
        
        /*      ���ö˿�        */
        GPIO_InitStructure.GPIO_Pin = m_PowerSwithSource[i].Pin;
        GPIO_Init(m_PowerSwithSource[i].Port, &GPIO_InitStructure);    
    }
}
/*
********************************************************************************
********************************************************************************
*                               �ӿں���
********************************************************************************
********************************************************************************
*/
/*
******************************************************************************
��������:   POWER_Config
��������:   ����POWER
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-09-10
ע    ��:   none
*******************************************************************************
*/
void POWER_Config(void)
{
    /*      ���õ�Դ����GPIO       */
    POWER_SwitchGpioConfig();
}
/*
******************************************************************************
��������:   POWER_SwitchSet
��������:   �򿪻��߹رյ�Դ���ߵ�ƽ�����͵�ƽ��
�������:   power����Դ���
            status�����õ�״̬��1���򿪣�0���ر�
�������:   none
�� �� ֵ:   none
��������:   2015-09-10
ע    ��:    
*******************************************************************************
*/
void POWER_SwitchSet(INT8U power, INT8U status)
{
    if(DEF_POWER_OPEN == status)
    {      
        /*      ����Դ      */
        GPIO_SetBits(m_PowerSwithSource[power].Port, m_PowerSwithSource[power].Pin);
        Debug("��Device��Power %d is open... \r\n", (power+1));
    }
    else
    {
        /*      �ص�Դ      */
        GPIO_ResetBits(m_PowerSwithSource[power].Port, m_PowerSwithSource[power].Pin);
        Debug("��Device��Power %d is close... \r\n", (power+1));
    }
}
