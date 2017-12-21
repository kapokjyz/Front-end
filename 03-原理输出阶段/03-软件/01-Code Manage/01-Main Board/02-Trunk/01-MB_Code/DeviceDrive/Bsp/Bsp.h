/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: Bsp.h
* ժ    Ҫ: STM32F10X BSP��������ģ��ͷ�ļ�
*
* ��ǰ�汾: 1.0
* ��    ��: ����
* �������: 2019-09-03
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

#ifndef  _BSP_H
#define  _BSP_H

/*
********************************************************************************
*                            ����ͷ�ļ�
********************************************************************************
*/
#include "Share.h"
/*
********************************************************************************
*                            �궨��
********************************************************************************
*/

/*
********************************************************************************
*                            �ⲿ��������
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
