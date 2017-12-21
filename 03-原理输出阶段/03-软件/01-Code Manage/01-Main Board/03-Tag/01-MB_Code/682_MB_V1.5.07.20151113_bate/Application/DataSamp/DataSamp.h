
/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: DataSamp.h
* 摘    要: 数据采样模块程序头文件
*
* 当前版本: 1.0
* 作    者: 曾健
* 完成日期: 2015-09-08
* 内    容:
* 注    意: none                                                                   
*******************************************************************************
*/
#ifndef _DATA_SAMP_H
#define _DATA_SAMP_H
/*
*******************************************************************************
*                               包含头文件
*******************************************************************************
*/
#include "includes.h"
/*
********************************************************************************
*                              类型定义
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
*                               接口函数
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



