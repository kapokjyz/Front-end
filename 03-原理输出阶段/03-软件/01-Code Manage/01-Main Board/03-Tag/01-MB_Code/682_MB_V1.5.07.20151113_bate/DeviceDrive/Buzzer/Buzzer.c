/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: Buzzer.c
* 摘    要: 蜂鸣器驱动程序
*
* 当前版本: 1.0
* 作    者: 曾健
* 完成日期: 2015-08-29
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
#include "Share.h"
#include "Buzzer.h"
/*
********************************************************************************
*                               宏定义
********************************************************************************
*/
/*      PC8-----  扬声器开关控制*/
#define          DEF_VOICE_CONTROL_PORT      (GPIOB)
#define          DEF_VOICE_CONTROL_PIN       (GPIO_Pin_8)

#define          DEF_VOICE_OFF   0 
#define          DEF_VOICE_ON  1
/*
********************************************************************************
*                               内部变量
********************************************************************************
*/
static TIM_TimeBaseInitTypeDef  TIM3_TimeBaseStructure; 
/*
********************************************************************************
********************************************************************************
*                               接口函数
********************************************************************************
********************************************************************************
*/
static void BUZZER_TIM3VoiceOutPut(void);
static void BUZZER_TIM3Handle(void);
static void BUZZER_VoiceCtrl(INT8U flag);
/*
********************************************************************************
* 函数名称: BUZZER_TIM3VoiceOutPut
* 函数功能: 声音输出，被定时器3调用
* 入口参数: void
* 出口参数: void
* 返 回 值: int
* 日    期: 2015-08-29
* 修改问题:
* 修改内容:
* 注    意:
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
* 函数名称: BUZZER_TIM3Handle
* 函数功能: 定时器3中断服务
* 入口参数: void
* 出口参数: 无
* 返 回 值: void
* 日    期: 2015-08-29
* 修改问题:
* 修改内容:
* 注    意:
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
* 函数名称: BUZZER_TIM3Init
* 函数功能: 定时器3初始化
* 入口参数: void
* 出口参数: 无
* 返 回 值: void
* 日    期: 2015-08-29
* 修改问题:
* 修改内容:
* 注    意:
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
* 函数名称: BUZZER_Config
* 函数功能: 配置蜂鸣器
* 入口参数: void
* 出口参数: void
* 返 回 值: void
* 日    期: 2015-08-29
* 修改问题:
* 修改内容:
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
* 函数名称: BUZZER_VoiceCtrl
* 函数功能: 蜂鸣器声音控制初始化
* 入口参数: flag    DEF_VOICE_ON,打开蜂鸣器   DEF_VOICE_OFF,关闭蜂鸣器
* 出口参数: void
* 返 回 值: void
* 日    期: 2015-08-29
* 修改问题:
* 修改内容:
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
* 函数名称: BUZZER_Voice_Output
* 函数功能: 蜂鸣器声音控制
* 入口参数: flag    DEF_VOICE_ON,打开蜂鸣器   DEF_VOICE_OFF,关闭蜂鸣器
* 出口参数: void
* 返 回 值: void
* 日    期: 2015-08-29
* 修改问题:
* 修改内容:
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