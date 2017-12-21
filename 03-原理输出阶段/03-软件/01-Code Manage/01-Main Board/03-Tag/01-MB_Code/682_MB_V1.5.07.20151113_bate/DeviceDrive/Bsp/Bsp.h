/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: Bsp.h
* 摘    要: STM32F10X BSP驱动程序模块头文件
*
* 当前版本: 1.0
* 作    者: 曾健
* 完成日期: 2019-09-03
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

#ifndef  _BSP_H
#define  _BSP_H

/*
********************************************************************************
*                            包含头文件
********************************************************************************
*/
#include "Share.h"
/*
********************************************************************************
*                            宏定义
********************************************************************************
*/

/*
********************************************************************************
*                            外部函数声明
********************************************************************************
*/
extern void BSP_Init (void);
extern void BSP_IntDisAll (void);
extern CPU_INT32U BSP_CPU_ClkFreq (void);
extern void BSP_IntInit (void);


extern void BSP_IntEn(CPU_DATA int_id);
extern void BSP_IntDis(CPU_DATA int_id);
extern void BSP_IntClr(CPU_DATA int_id);
extern void BSP_IntVectSet(CPU_DATA int_id, CPU_FNCT_VOID isr);
extern void BSP_IntPrioSet(CPU_DATA int_id, CPU_INT08U prio);
extern void BSP_PeriphEn(CPU_DATA pwr_clk_id);
extern void BSP_PeriphDis(CPU_DATA pwr_clk_id);
extern CPU_INT32U BSP_PeriphClkFreqGet(CPU_DATA pwr_clk_id);


void FeedWatchDog(void);


#endif 
