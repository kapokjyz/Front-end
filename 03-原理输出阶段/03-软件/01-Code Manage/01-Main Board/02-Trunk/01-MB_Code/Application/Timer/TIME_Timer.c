/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: TIME_Timer.c
* ժ    Ҫ: ��ʱ��ģ��ʵ��
*
* ��ǰ�汾: 1.0
* ��    ��: ����
* �������: 2015-09-14
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
#include "TIME_Timer.h"
/*
********************************************************************************
*                               �궨��
********************************************************************************
*/
#define DEF_TIMER_COUNT			5
#define DEF_TIMER_DISABLE			0
#define DEF_TIMER_ENABLE			1

#define DEF_TIMER_FREE		        0
#define DEF_TIMER_BUZY				1
/*
********************************************************************************
*                               �ṹ����
********************************************************************************
*/
/*
	��ʱ���ṹ
*/
typedef struct {  
	BOOLEAN   TmrEn;                      
	INT32U    TmrCtr; 
	INT32U    TmrInit; 
	INT8U	  IsFree;
	void    (*TmrFnct)(void *);  
	void     *TmrFnctArg; 
} TIMER;	
/*
	��ʱ������
*/
typedef struct {  
	TIMER	NormalTimer[DEF_TIMER_COUNT];
	TIMER	FastTimer[DEF_TIMER_COUNT];
} TIMER_MANAGE;

/*
********************************************************************************
*                               �ڲ�����
********************************************************************************
*/
static TIMER_MANAGE m_TimerMng;
/*
********************************************************************************
*                               �ڲ�����
********************************************************************************
*/
/*
******************************************************************************
��������:	TIME_GetTimer
��������:   ͨ����������ͻ�ö�ʱ��
�������:	htimer����ʱ�����
				type����ʱ������
�������:	none
�� �� ֵ:   ��ʱ��������
��������:   2015-09-14
ע    ��:    
*******************************************************************************
*/
static TIMER*  TIME_GetTimer(TIMER_HANDLE htimer, INT8U type)
{
	TIMER *pt;

	switch(type)
	{
	case  DEF_TIMER_NORMAL_TYPE:
		pt = &m_TimerMng.NormalTimer[htimer];
		break;

	case  DEF_TIMER_FAST_TYPE:
		pt = &m_TimerMng.FastTimer[htimer];
		break;

	default:
		pt = (void *)0;
	}

	return pt;
}
/*
********************************************************************************
********************************************************************************
*                               ���ܺ���
********************************************************************************
********************************************************************************
*/
/*
******************************************************************************
��������:	TIME_TIMERCreate
��������:   ������ʱ��
�������:	fnct��ָ��ʱ��������ָ��
				arg������������Ĳ���
				type����ʱ������
�������:	none
�� �� ֵ:   ��ʱ��������
��������:  2015-09-14
ע    ��:    
*******************************************************************************
*/
TIMER_HANDLE  TIME_TimerCreate(void (*fnct)(void *), void *arg, INT8U type)
{
	TIMER *ptmr;
	INT8U i;
    
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
   
	ptmr =  TIME_GetTimer(0, type);
	if (ptmr == (void *)0)
	{
		return -1;
	}

	i = 0;
	while (i < DEF_TIMER_COUNT)
	{
		if (DEF_TIMER_FREE == ptmr->IsFree)
		{
			ptmr->IsFree = DEF_TIMER_BUZY;
			break;
		}
        
		ptmr++;
		i++;
	}

	if (DEF_TIMER_COUNT == i)
	{
		return -1;
	}

	OS_ENTER_CRITICAL();
	ptmr->TmrFnct    = fnct; 
	ptmr->IsFree = DEF_TIMER_BUZY;
	ptmr->TmrFnctArg = arg; 
	OS_EXIT_CRITICAL();

	return i;
}
/*
******************************************************************************
��������:	TIME_TimerDelete
��������:   ɾ����ʱ��
�������:	htimer����ʱ�����
				type����ʱ������
�������:	none
�� �� ֵ:   none
��������:  2015-09-14
ע    ��:    
*******************************************************************************
*/
void  TIME_TimerDelete(TIMER_HANDLE htimer, INT8U type)
{
	TIMER *ptmr;
    
	/*		�жϾ���Ƿ�Ϸ�		*/
	if (htimer>=0 && htimer<DEF_TIMER_COUNT)
	{
		ptmr =  TIME_GetTimer(htimer, type);
		if (ptmr == (void *)0)
		{
			return;
		}

		ptmr->IsFree = DEF_TIMER_FREE;
	}
}
/*
******************************************************************************
��������:	TIME_TimerSet
��������:   ��ʱ����
�������:	htimer����ʱ�����
				type����ʱ������
				min����
				sec����
				tenths���ٺ���
�������:	none
�� �� ֵ:   none
��������:  2015-09-14
ע    ��:    
*******************************************************************************
*/
void TIME_TimerSet(TIMER_HANDLE htimer, INT8U type, INT8U min, INT8U sec, INT8U tenths)
{
	TIMER *ptmr;
	INT16U value;
    
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
    
	ptmr =  TIME_GetTimer(htimer, type);
	if (ptmr == (void *)0)
	{
		return;
	}

	OS_ENTER_CRITICAL();
	value   = (INT16U)min * 600 + (INT16U)sec * 10 + (INT16U)tenths;
	ptmr->TmrCtr = value;
	ptmr->TmrInit = value;
	OS_EXIT_CRITICAL();
}
 /*
 ******************************************************************************
 ��������:	TIME_TimerStop
 ��������:   ֹͣ��ʱ��
 �������:	htimer����ʱ�����
 				type����ʱ������
 �������:	none
 �� �� ֵ:   none
 ��������:  2015-09-14
 ע    ��:    
 *******************************************************************************
 */
 void TIME_TimerStop(TIMER_HANDLE htimer, INT8U type)
 {
	TIMER *ptmr;
     
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	ptmr =  TIME_GetTimer(htimer, type);
	if (ptmr == (void *)0)
	{
		return;
	}

	OS_ENTER_CRITICAL();
	ptmr->TmrEn = DEF_TIMER_DISABLE;
	OS_EXIT_CRITICAL();
 }
 /*
******************************************************************************
��������:	TIME_TimerStart
��������:   ������ʱ��
�������:	htimer����ʱ�����
				type����ʱ������
�������:	none
�� �� ֵ:   none
��������:  2015-09-14
ע    ��:    
*******************************************************************************
*/
void TIME_TimerStart(TIMER_HANDLE htimer, INT8U type)
{
	TIMER *ptmr;
    
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	ptmr = TIME_GetTimer(htimer, type);
	if (ptmr == (void *)0)
	{
		return;
	}

	OS_ENTER_CRITICAL();
	ptmr->TmrEn = DEF_TIMER_ENABLE;
	OS_EXIT_CRITICAL();
}
/*
******************************************************************************
��������:	TIME_TimerReset
��������:   ��ʱ������
�������:	htimer����ʱ�����
				type����ʱ������
�������:	none
�� �� ֵ:   none
��������:  2015-09-14
ע    ��:    
*******************************************************************************
*/
void  TIME_TimerReset (TIMER_HANDLE htimer, INT8U type)
{
	TIMER *ptmr;
    
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif

	ptmr =  TIME_GetTimer(htimer, type);
	if (ptmr == (void *)0)
	{
		return;
	}

	OS_ENTER_CRITICAL();
	ptmr->TmrCtr = ptmr->TmrInit;
	OS_EXIT_CRITICAL();
	TIME_TimerStart(htimer, type);
}
/*
******************************************************************************
��������:	TIME_Init
��������:   ʱ��ģ���ʼ��
�������:	none
�������:	none
�� �� ֵ:   none
��������:  2015-09-14
ע    ��:    
*******************************************************************************
*/
void  TIME_Init (void)
{
	INT8U  i;
	TIMER   *pntmr;
	TIMER   *pftmr;
	
	/*		��ʼ����ʱ��		*/
	pntmr = &m_TimerMng.NormalTimer[0];
	pftmr = &m_TimerMng.FastTimer[0];
	for (i = 0; i < DEF_TIMER_COUNT; i++) 
	{
		pntmr->TmrEn   = DEF_TIMER_DISABLE;
		pntmr->TmrCtr  = 0;
		pntmr->TmrInit = 0;
		pntmr->IsFree = DEF_TIMER_FREE;
		pntmr->TmrFnct = (void (*)(void *))0;
		pntmr++;

		pftmr->TmrEn   = DEF_TIMER_DISABLE;
		pftmr->TmrCtr  = 0;
		pftmr->TmrInit = 0;
		pftmr->IsFree = DEF_TIMER_FREE;
		pftmr->TmrFnct = (void (*)(void *))0;
		pftmr++;
	}
}
/*
******************************************************************************
��������:	TIME_TimerScan
��������:   ��ʱ��ɨ�躯��
�������:	type����ʱ������
�������:	none
�� �� ֵ:   none
��������:  2015-09-14
ע    ��:    
*******************************************************************************
*/
static void TIME_TimerScan(INT8U type)
{
	TIMER    *ptmr;
	void  (*pfnct)(void *);         
	void   *parg; 
	INT8U   i;
    
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
	OS_CPU_SR  cpu_sr = 0;
#endif

	pfnct = (void (*)(void *))0;     
	parg  = (void *)0;
	ptmr = TIME_GetTimer(0, type); 

	/*		ɨ�趨ʱ���б�		*/
	for (i = 0; i < DEF_TIMER_COUNT; i++) 
	{
		OS_ENTER_CRITICAL();
		/*		�ж϶�ʱ���Ƿ�����		*/
		if (ptmr->TmrEn == DEF_TIMER_ENABLE && ptmr->IsFree == DEF_TIMER_BUZY) 
		{
			/*		�жϼ������Ƿ���㣬��ʱʱ�䵽		*/
			if (ptmr->TmrCtr > 0)
			{
				ptmr->TmrCtr--;
				if (ptmr->TmrCtr == 0)
				{   
					ptmr->TmrEn = DEF_TIMER_DISABLE; 
					pfnct = ptmr->TmrFnct;  
					parg  = ptmr->TmrFnctArg; 
				}
			}
		}
		OS_EXIT_CRITICAL();
		/*		ִ�ж�ʱ������	*/
		if (pfnct != (void (*)(void *))0) 
		{  
			(*pfnct)(parg); 
			pfnct = (void (*)(void *))0;
		}
		ptmr++;
	}
}
/*
******************************************************************************
��������:	TIME_NormalTimerExec
��������:   һ�㶨ʱ��ִ��������
�������:	none
�������:	none
�� �� ֵ:   none
��������:  2015-09-14
ע    ��:    
*******************************************************************************
*/
void TIME_NormalTimerExec(void)
{	  
	TIME_TimerScan(DEF_TIMER_NORMAL_TYPE);
}
/*
******************************************************************************
��������:	TIME_FastTimerExec
��������:   ���ٶ�ʱ��ִ��������
�������:	none
�������:	none
�� �� ֵ:   none
��������:  2015-09-14
ע    ��:    
*******************************************************************************
*/
void TIME_FastTimerExec(void)
{	
	TIME_TimerScan(DEF_TIMER_FAST_TYPE);
}

