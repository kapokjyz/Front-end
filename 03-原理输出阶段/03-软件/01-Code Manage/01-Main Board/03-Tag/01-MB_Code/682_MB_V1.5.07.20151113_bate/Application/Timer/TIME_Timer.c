/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: TIME_Timer.c
* 摘    要: 定时器模块实现
*
* 当前版本: 1.0
* 作    者: 曾健
* 完成日期: 2015-09-14
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
#include "TIME_Timer.h"
/*
********************************************************************************
*                               宏定义
********************************************************************************
*/
#define DEF_TIMER_COUNT			5
#define DEF_TIMER_DISABLE			0
#define DEF_TIMER_ENABLE			1

#define DEF_TIMER_FREE		        0
#define DEF_TIMER_BUZY				1
/*
********************************************************************************
*                               结构定义
********************************************************************************
*/
/*
	定时器结构
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
	定时器管理
*/
typedef struct {  
	TIMER	NormalTimer[DEF_TIMER_COUNT];
	TIMER	FastTimer[DEF_TIMER_COUNT];
} TIMER_MANAGE;

/*
********************************************************************************
*                               内部变量
********************************************************************************
*/
static TIMER_MANAGE m_TimerMng;
/*
********************************************************************************
*                               内部函数
********************************************************************************
*/
/*
******************************************************************************
函数名称:	TIME_GetTimer
函数功能:   通过句柄和类型获得定时器
输入参数:	htimer：定时器句柄
				type：定时器类型
输出参数:	none
返 回 值:   定时器描述符
创建日期:   2015-09-14
注    意:    
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
*                               功能函数
********************************************************************************
********************************************************************************
*/
/*
******************************************************************************
函数名称:	TIME_TIMERCreate
函数功能:   创建定时器
输入参数:	fnct：指向定时服务函数的指针
				arg：传入服务函数的参数
				type：定时器类型
输出参数:	none
返 回 值:   定时器描述符
创建日期:  2015-09-14
注    意:    
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
函数名称:	TIME_TimerDelete
函数功能:   删除定时器
输入参数:	htimer：定时器句柄
				type：定时器类型
输出参数:	none
返 回 值:   none
创建日期:  2015-09-14
注    意:    
*******************************************************************************
*/
void  TIME_TimerDelete(TIMER_HANDLE htimer, INT8U type)
{
	TIMER *ptmr;
    
	/*		判断句柄是否合法		*/
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
函数名称:	TIME_TimerSet
函数功能:   定时设置
输入参数:	htimer：定时器句柄
				type：定时器类型
				min：分
				sec：秒
				tenths：百毫秒
输出参数:	none
返 回 值:   none
创建日期:  2015-09-14
注    意:    
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
 函数名称:	TIME_TimerStop
 函数功能:   停止定时器
 输入参数:	htimer：定时器句柄
 				type：定时器类型
 输出参数:	none
 返 回 值:   none
 创建日期:  2015-09-14
 注    意:    
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
函数名称:	TIME_TimerStart
函数功能:   启动定时器
输入参数:	htimer：定时器句柄
				type：定时器类型
输出参数:	none
返 回 值:   none
创建日期:  2015-09-14
注    意:    
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
函数名称:	TIME_TimerReset
函数功能:   定时器重置
输入参数:	htimer：定时器句柄
				type：定时器类型
输出参数:	none
返 回 值:   none
创建日期:  2015-09-14
注    意:    
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
函数名称:	TIME_Init
函数功能:   时钟模块初始化
输入参数:	none
输出参数:	none
返 回 值:   none
创建日期:  2015-09-14
注    意:    
*******************************************************************************
*/
void  TIME_Init (void)
{
	INT8U  i;
	TIMER   *pntmr;
	TIMER   *pftmr;
	
	/*		初始化定时器		*/
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
函数名称:	TIME_TimerScan
函数功能:   定时器扫描函数
输入参数:	type：定时器类型
输出参数:	none
返 回 值:   none
创建日期:  2015-09-14
注    意:    
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

	/*		扫描定时器列表		*/
	for (i = 0; i < DEF_TIMER_COUNT; i++) 
	{
		OS_ENTER_CRITICAL();
		/*		判断定时器是否启用		*/
		if (ptmr->TmrEn == DEF_TIMER_ENABLE && ptmr->IsFree == DEF_TIMER_BUZY) 
		{
			/*		判断计数器是否归零，则定时时间到		*/
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
		/*		执行定时服务函数	*/
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
函数名称:	TIME_NormalTimerExec
函数功能:   一般定时器执行主函数
输入参数:	none
输出参数:	none
返 回 值:   none
创建日期:  2015-09-14
注    意:    
*******************************************************************************
*/
void TIME_NormalTimerExec(void)
{	  
	TIME_TimerScan(DEF_TIMER_NORMAL_TYPE);
}
/*
******************************************************************************
函数名称:	TIME_FastTimerExec
函数功能:   快速定时器执行主函数
输入参数:	none
输出参数:	none
返 回 值:   none
创建日期:  2015-09-14
注    意:    
*******************************************************************************
*/
void TIME_FastTimerExec(void)
{	
	TIME_TimerScan(DEF_TIMER_FAST_TYPE);
}

