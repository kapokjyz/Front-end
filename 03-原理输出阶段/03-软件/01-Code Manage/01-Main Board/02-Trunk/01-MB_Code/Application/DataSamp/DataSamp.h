
/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: DataSamp.h
* ժ    Ҫ: ���ݲ���ģ�����ͷ�ļ�
*
* ��ǰ�汾: 1.0
* ��    ��: ����
* �������: 2015-09-08
* ��    ��:
* ע    ��: none                                                                   
*******************************************************************************
*/
#ifndef _DATA_SAMP_H
#define _DATA_SAMP_H
/*
*******************************************************************************
*                               ����ͷ�ļ�
*******************************************************************************
*/
#include "includes.h"
/*
********************************************************************************
*                              ���Ͷ���
********************************************************************************
*/
enum {
    ENUM_ADAPTOR = 0,
    ENUM_BAT,        
    ENUM_PAW, 
    ENUM_O2,
    DEF_ADC1_CHANNALS                
};
/*
********************************************************************************
*                               �ӿں���
********************************************************************************
*/
extern void DATASAMP_Init(void);
extern void DATASAMP_Exec(void);  
extern FP32 DATASAMP_GetVoltage(INT8U type_id);
extern INT16U DATASAMP_AdValue(INT8U type_id);
extern INT8U DATASAMP_AppCalO2(INT16U AD_CAL_O2);
extern INT8U DATASAMP_AppCalPaw(INT16U AD_CAL_Paw);
extern void  DATASAMP_AppAdjustO2(FP32 *k, FP32 *b, INT16U x1,INT16U x2,INT16U y1,INT16U y2);
#endif 



