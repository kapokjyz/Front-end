/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: Alarm.h
* ժ    Ҫ: ����ģ��
*
* ��ǰ�汾: 1.0
* ��    ��: ����
* �������: 2015-09-24
* ��    ��:
* ע    ��: none                                                                   
*******************************************************************************
*/
#ifndef ALARM_H
#define ALARM_H
/*
*******************************************************************************
*                               ����ͷ�ļ�
*******************************************************************************
*/
#include "includes.h"
/*
********************************************************************************
*                               �ṹ����
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
*                               �ӿں���
********************************************************************************
*/
extern void ALARM_Exec(void);
extern void ALARM_Init(void);
extern void ALARM_Write(ALARM_Table code);
extern INT8U ALARM_GetIsDisableSound(void);
#endif 
