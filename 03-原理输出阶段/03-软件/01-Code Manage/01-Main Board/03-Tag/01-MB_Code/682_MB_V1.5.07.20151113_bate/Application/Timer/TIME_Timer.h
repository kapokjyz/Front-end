/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: TIME_Timer.h
* 摘    要: 定时器接口
*
* 当前版本: 1.0
* 作    者: 曾健    
* 完成日期: 2015-09-14
* 内    容:
* 注    意: none                                                                   
*******************************************************************************
*/
#ifndef TIME_TIMER_H
#define TIME_TIMER_H
/*
*******************************************************************************
*                               包含头文件
*******************************************************************************
*/
#include "includes.h"
/*
********************************************************************************
*                               宏定义
********************************************************************************
*/
#define DEF_TIMER_NORMAL_TYPE		0
#define DEF_TIMER_FAST_TYPE				1
#define TIMER_HANDLE INT8S
/*
********************************************************************************
*                               接口函数
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
