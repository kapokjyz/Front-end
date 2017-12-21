/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: Buzzer.c
* ժ    Ҫ: ��������������
*
* ��ǰ�汾: 1.0
* ��    ��: ����
* �������: 2015-08-29
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
#include "Buzzer.h"
/*
********************************************************************************
*                               �궨��
********************************************************************************
*/
/*      PC8-----  ���������ؿ���*/
#define          DEF_VOICE_CONTROL_PORT      (GPIOB)
#define          DEF_VOICE_CONTROL_PIN       (GPIO_Pin_8)

#define          DEF_VOICE_OFF   0 
#define          DEF_VOICE_ON  1
/*
********************************************************************************
*                               �ڲ�����
********************************************************************************
*/
static TIM_TimeBaseInitTypeDef  TIM3_TimeBaseStructure; 
/*
********************************************************************************
********************************************************************************
*                               �ӿں���
********************************************************************************
********************************************************************************
*/
static void BUZZER_TIM3VoiceOutPut(void);
static void BUZZER_TIM3Handle(void);
static void BUZZER_VoiceCtrl(INT8U flag);
/*
********************************************************************************
* ��������: BUZZER_TIM3VoiceOutPut
* ��������: �������������ʱ��3����
* ��ڲ���: void
* ���ڲ���: void
* �� �� ֵ: int
* ��    ��: 2015-08-29
* �޸�����:
* �޸�����:
* ע    ��:
********************************************************************************
*/
static void BUZZER_TIM3VoiceOutPut(void)
{
    static CPU_INT08U voiceflag = DEF_OFF;
    
    voiceflag = (~voiceflag) & DEF_ON;
    BUZZER_VoiceCtrl(voiceflag);
}
/*
********************************************************************************
* ��������: BUZZER_TIM3Handle
* ��������: ��ʱ��3�жϷ���
* ��ڲ���: void
* ���ڲ���: ��
* �� �� ֵ: void
* ��    ��: 2015-08-29
* �޸�����:
* �޸�����:
* ע    ��:
********************************************************************************
*/
static void BUZZER_TIM3Handle(void)
{
    BUZZER_TIM3VoiceOutPut();
    
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update );
    }  
}
/*
********************************************************************************
* ��������: BUZZER_TIM3Init
* ��������: ��ʱ��3��ʼ��
* ��ڲ���: void
* ���ڲ���: ��
* �� �� ֵ: void
* ��    ��: 2015-08-29
* �޸�����:
* �޸�����:
* ע    ��:
********************************************************************************
*/
void BUZZER_TIM3Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    TIM3_TimeBaseStructure.TIM_Period = 39;
    /* 72M/3600 = 5us*/
    TIM3_TimeBaseStructure.TIM_Prescaler = 3999;
    TIM3_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM3_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseInit(TIM3, &TIM3_TimeBaseStructure);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    BSP_IntVectSet(BSP_INT_ID_TIM3,BUZZER_TIM3Handle);
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update );
    /* TIM2 enable counter */
    TIM_Cmd(TIM3, DISABLE);
}
/*
********************************************************************************
* ��������: BUZZER_Config
* ��������: ���÷�����
* ��ڲ���: void
* ���ڲ���: void
* �� �� ֵ: void
* ��    ��: 2015-08-29
* �޸�����:
* �޸�����:
********************************************************************************
*/
void BUZZER_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = DEF_VOICE_CONTROL_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEF_VOICE_CONTROL_PORT, &GPIO_InitStructure);
}
/*
********************************************************************************
* ��������: BUZZER_VoiceCtrl
* ��������: �������������Ƴ�ʼ��
* ��ڲ���: flag    DEF_VOICE_ON,�򿪷�����   DEF_VOICE_OFF,�رշ�����
* ���ڲ���: void
* �� �� ֵ: void
* ��    ��: 2015-08-29
* �޸�����:
* �޸�����:
********************************************************************************
*/
static void BUZZER_VoiceCtrl(INT8U flag)
{
    if(flag)
    {        
        GPIO_SetBits(DEF_VOICE_CONTROL_PORT, DEF_VOICE_CONTROL_PIN);
        return;
    }
    
    GPIO_ResetBits(DEF_VOICE_CONTROL_PORT, DEF_VOICE_CONTROL_PIN);
}
/*
********************************************************************************
* ��������: BUZZER_Voice_Output
* ��������: ��������������
* ��ڲ���: flag    DEF_VOICE_ON,�򿪷�����   DEF_VOICE_OFF,�رշ�����
* ���ڲ���: void
* �� �� ֵ: void
* ��    ��: 2015-08-29
* �޸�����:
* �޸�����:
********************************************************************************
*/
void BUZZER_VoiceOutput(INT8U flag)
{
    if(flag) 
    {
        TIM_Cmd(TIM3, ENABLE);
        return;
    }
    
    TIM_Cmd(TIM3, DISABLE);
    BUZZER_VoiceCtrl(DEF_OFF);
}