/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: TIME_Timer.h
* ժ    Ҫ: ��ʱ���ӿ�
*
* ��ǰ�汾: 1.0
* ��    ��: ����    
* �������: 2015-09-14
* ��    ��:
* ע    ��: none                                                                   
*******************************************************************************
*/
#ifndef TIME_TIMER_H
#define TIME_TIMER_H
/*
*******************************************************************************
*                               ����ͷ�ļ�
*******************************************************************************
*/
#include "includes.h"
/*
********************************************************************************
*                               �궨��
********************************************************************************
*/
#define DEF_TIMER_NORMAL_TYPE		0
#define DEF_TIMER_FAST_TYPE				1
#define TIMER_HANDLE INT8S
/*
********************************************************************************
*                               �ӿں���
********************************************************************************
*/
extern void TIME_NormalTimerExec(void);
extern void TIME_FastTimerExec(void);
extern void TIME_Init(void);

extern TIMER_HANDLE TIME_TimerCreate(void (*fnct)(void *), void *arg, INT8U type);
extern void TIME_TimerDelete(TIMER_HANDLE htimer, INT8U type);
extern void TIME_TimerSet(TIMER_HANDLE htimer, INT8U type, INT8U min, INT8U sec, INT8U tenths);
extern void TIME_TimerStart(TIMER_HANDLE htimer, INT8U type);
extern void TIME_TimerStop(TIMER_HANDLE htimer, INT8U type);
extern void TIME_TimerReset (TIMER_HANDLE htimer, INT8U type);
#endif 
