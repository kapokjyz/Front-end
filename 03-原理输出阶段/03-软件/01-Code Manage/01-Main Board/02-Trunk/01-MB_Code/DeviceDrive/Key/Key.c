/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: Key.c
* ժ    Ҫ: STM32F10XX������������
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
#include  "Share.h"
#include  "Key.h"
/*
********************************************************************************
*                               �궨��
********************************************************************************
*/
/*      Key Count   */
#define DEF_KEY_COUNT      3
/*      û������        */
#define DEF_KEY_NONE        0
/*
********************************************************************************
*                               �ṹ����
********************************************************************************
*/
/*      Key Source      */
typedef struct
{
  PORT_TypeDef *Port;
  PIN_TypeDef  Pin;
  CLK_TypeDef  Clk;
  RCC_XXXPeriphClockCmd ClockCmd;
}KEY_SOURCE;
/*
********************************************************************************
*                               �ڲ�����
********************************************************************************
*/
/*      
    Key Source      
*/
static KEY_SOURCE m_KeySource[DEF_KEY_COUNT]=
{
  /*        k1 : ���ػ���      */
  {GPIOC, GPIO_Pin_3, RCC_APB2Periph_GPIOC, RCC_APB2PeriphClockCmd},
    /*      k2 : У׼��      */
  {GPIOA, GPIO_Pin_1, RCC_APB2Periph_GPIOA, RCC_APB2PeriphClockCmd},
    /*      k3 : ������      */
  {GPIOA, GPIO_Pin_2, RCC_APB2Periph_GPIOA, RCC_APB2PeriphClockCmd},
};
/*      
    Key Value      
*/
static INT8U m_KeyValue[DEF_KEY_COUNT]=
{
    DEF_KEY_CODE_PPOWER_SWITCH,
    DEF_KEY_CODE_ADJUST,
    DEF_KEY_CODE_ALARM_SOUND
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
��������:   KEY_Config
��������:   ���I���ú���
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-08-28
ע    ��:    
*******************************************************************************
*/
void KEY_Config(void)
{
    INT8U i;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    for (i=0; i<DEF_KEY_COUNT; i++)
    {
        /*      ����ʱ��    */
        m_KeySource[i].ClockCmd(m_KeySource[i].Clk, ENABLE);
        
        /*      ���ö˿�        */
        GPIO_InitStructure.GPIO_Pin = m_KeySource[i].Pin;
        GPIO_Init(m_KeySource[i].Port, &GPIO_InitStructure);
    }
}
/*
******************************************************************************
��������:   KEY_Scan
��������:   ����ɨ��
�������:   none
�������:   none
�� �� ֵ:   ������ֵ
��������:   2015-08-28
ע    ��:    
*******************************************************************************
*/
INT8U KEY_Scan(void)
{
    INT8U i;
    INT8U key_value;
    
    key_value = DEF_KEY_NONE;
    
    for (i=0; i<DEF_KEY_COUNT; i++)
    {
        if(!GPIO_ReadInputDataBit(m_KeySource[i].Port,m_KeySource[i].Pin))
        {
            key_value = m_KeyValue[i];
            break;
        }
    }
    
    return key_value;
}