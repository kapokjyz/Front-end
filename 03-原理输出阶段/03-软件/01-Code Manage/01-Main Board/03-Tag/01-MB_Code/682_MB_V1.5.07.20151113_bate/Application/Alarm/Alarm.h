/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: Alarm.h
* 摘    要: 报警模块
*
* 当前版本: 1.0
* 作    者: 曾健
* 完成日期: 2015-09-24
* 内    容:
* 注    意: none                                                                   
*******************************************************************************
*/
#ifndef ALARM_H
#define ALARM_H
/*
*******************************************************************************
*                               包含头文件
*******************************************************************************
*/
#include "includes.h"
/*
********************************************************************************
*                               结构定义
********************************************************************************
*/
enum
{
	DEF_ALARM_CODE_HIGH_FiO2 = 0,
	DEF_ALARM_CODE_LOW_FiO2,
	DEF_ALARM_CODE_HIGH_PAW,
	DEF_ALARM_CODE_LOW_PAW,
    DEF_ALARM_CODE_SOUND_DESIABLE,    
};

typedef struct
{
    INT8U FiO2_High_Alarm;
    INT8U FiO2_Low_Alarm;
    INT8U Paw_High_Alarm;
    INT8U Paw_Low_Alarm;
    INT8U Sound_Disable_Alarm;
    INT8U Bat_Red_Alarm;
}ALARM_Table;
/*
********************************************************************************
*                               接口函数
********************************************************************************
*/
extern void ALARM_Exec(void);
extern void ALARM_Init(void);
extern void ALARM_Write(ALARM_Table code);
extern INT8U ALARM_GetIsDisableSound(void);
#endif 
