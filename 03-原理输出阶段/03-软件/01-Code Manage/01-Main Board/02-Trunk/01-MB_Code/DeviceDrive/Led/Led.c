/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: Led.c
* ժ    Ҫ: LedSTM32F10XX��������
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
#include "Led.h"
/*
********************************************************************************
*                               �궨��
********************************************************************************
*/
/*
    Led count
*/
#define DEF_LED_COUNT   6
/*
********************************************************************************
*                               ���Ͷ���
********************************************************************************
*/
/*      Led Source      */
typedef struct
{
  PORT_TypeDef *Port;
  PIN_TypeDef  Pin;
  CLK_TypeDef  Clk;
  RCC_XXXPeriphClockCmd ClockCmd;
}LED_SOURCE;
/*
********************************************************************************
*                               �ڲ�����
********************************************************************************
*/
/*      
    Led Source      
*/
static LED_SOURCE m_LedSource[DEF_LED_COUNT]=
{ 
    /*       ��ص�_��      */
    {GPIOB, GPIO_Pin_5, RCC_APB2Periph_GPIOB, RCC_APB2PeriphClockCmd},
    /*       ��ص�_��      */
    {GPIOC, GPIO_Pin_12, RCC_APB2Periph_GPIOC, RCC_APB2PeriphClockCmd},
    /*       ��������       */
    {GPIOB, GPIO_Pin_1, RCC_APB2Periph_GPIOB, RCC_APB2PeriphClockCmd},
    /*       ��Ũ�ȹ��߱�����       */
    {GPIOB, GPIO_Pin_3, RCC_APB2Periph_GPIOB, RCC_APB2PeriphClockCmd},
    /*       ��Ũ�ȹ��ͱ�����       */
    {GPIOD, GPIO_Pin_2, RCC_APB2Periph_GPIOD, RCC_APB2PeriphClockCmd},
    /*       ����������       */
    {GPIOB, GPIO_Pin_7, RCC_APB2Periph_GPIOB, RCC_APB2PeriphClockCmd},  
};

/*����LED������*/
static INT8U m_LedContrl[DEF_LED_COUNT][2]=
{
  {DEF_LED_CLOSE, DEF_LED_OPEN},
  {DEF_LED_CLOSE, DEF_LED_OPEN},
  {DEF_LED_CLOSE, DEF_LED_OPEN},

  {DEF_LED_OPEN, DEF_LED_CLOSE},
  {DEF_LED_OPEN, DEF_LED_CLOSE},
  {DEF_LED_OPEN, DEF_LED_CLOSE},
}; 
/*
********************************************************************************
********************************************************************************
*                               �ӿں���
********************************************************************************
********************************************************************************
*/
/*
******************************************************************************
��������:   LED_Config
��������:   ����Led
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-08-28
ע    ��:   none
*******************************************************************************
*/
void LED_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	INT8U i;
	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    	
    for(i=0; i<DEF_LED_COUNT; i++)
    {     
        /*      ����ʱ��    */
        m_LedSource[i].ClockCmd(RCC_APB2Periph_AFIO|m_LedSource[i].Clk, ENABLE);
        
        /*      ���ö˿�        */
        GPIO_InitStructure.GPIO_Pin = m_LedSource[i].Pin;
        GPIO_Init(m_LedSource[i].Port, &GPIO_InitStructure);
        
        /*      �ر�Led   */
        if(i < 3)
        {
            GPIO_ResetBits(m_LedSource[i].Port, m_LedSource[i].Pin);
        }
        else
        {
            GPIO_SetBits(m_LedSource[i].Port, m_LedSource[i].Pin);
        }
        Debug("��Device��Led %d is close... \r\n", (i+1));
    }	
}
/*
******************************************************************************
��������:   LED_Set
��������:   �򿪻��߹ر�Led���ߵ�ƽ�����͵�ƽ��
�������:   led�����
            status�����õ�״̬��1���򿪣�0���ر�
�������:   none
�� �� ֵ:   none
��������:   2015-08-28
ע    ��:    
*******************************************************************************
*/
void LED_Set(INT8U led, INT8U status)
{
    INT8U flag = DEF_LED_CLOSE;
    
    flag = m_LedContrl[led][status];
    
    if(DEF_LED_OPEN == flag)
    {      
        /*      ����      */
        GPIO_SetBits(m_LedSource[led].Port, m_LedSource[led].Pin);
        Debug("��Device��Led %d is open... \r\n", (led+1));
    }
    else
    {
        /*      �ص�      */
        GPIO_ResetBits(m_LedSource[led].Port, m_LedSource[led].Pin);
        Debug("��Device��Led %d is close... \r\n", (led+1));
    }
}