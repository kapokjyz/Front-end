/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: Main.c
* 摘    要: 系统入口模块
*
* 当前版本: 1.0
* 作    者: 曾健
* 完成日期: 2015-08-28
* 内    容:
* 注    意: none                                                                  
*******************************************************************************
*/

/*
*******************************************************************************
*                               包含头文件
*******************************************************************************
*/
#include "includes.h"
#include "DataSamp.h"
#include "Monitor.h"
#include "ShutDown.h"
#include "StartingUp.h"
#include "Display.h"
#include "Calibration.h"
#include "KeyApp.h"

/*
********************************************************************************
*                               宏定义
********************************************************************************
*/
/*		任务堆栈大小		*/
#define TASK_STK_SIZE				256u

/*		 任务优先级		*/
#define DEF_TIMER_STK_PRIO              4u
#define DEF_DATASAMP_STK_PRIO			6u
#define DEF_DISP_STK_PRIO               8u
#define DEF_MONITOR_STK_PRIO            9u
#define DEF_KEY_STK_PRIO                12u

#define DEF_CALIBRATION_STK_PRIO        14u
#define DEF_SHUT_DOWN_STK_PRIO		    16u
#define DEF_STARTING_UP_STK_PRIO		18u

/*		 任务轮询周期		*/
#define DEF_TIMER_STK_DELAY_TIME              50u
#define DEF_DATASAMP_STK_DELAY_TIME			  10u
#define DEF_DISP_STK_DELAY_TIME               200u
#define DEF_MONITOR_STK_DELAY_TIME           100u
#define DEF_KEY_STK_DELAY_TIME                100u
#define DEF_CALIBRATION_STK_DELAY_TIME        100u
#define DEF_SHUT_DOWN_STK_DELAY_TIME		  200u

/*
********************************************************************************
*                               内部变量
********************************************************************************
*/
/*		任务堆栈		*/
static OS_STK m_TimerStk[TASK_STK_SIZE];
static OS_STK m_DataSampStk[TASK_STK_SIZE];
static OS_STK m_DispStk[TASK_STK_SIZE];
static OS_STK m_MonitorStk[TASK_STK_SIZE];
static OS_STK m_KeyStk[TASK_STK_SIZE];
static OS_STK m_CalibrationStk[TASK_STK_SIZE];
static OS_STK m_ShutDownStk[TASK_STK_SIZE];
static OS_STK m_StartingUpStk[TASK_STK_SIZE];
/*
********************************************************************************
*                               内部函数
********************************************************************************
*/
/*
********************************************************************************
********************************************************************************
*                               功能函数
********************************************************************************
********************************************************************************
*/
/*		任务执行函数		*/
static void MAIN_TimerTask(void *p_arg);
static void MAIN_DataSampTask(void *p_arg);
static void MAIN_MonitorTask(void *p_arg);
static void MAIN_ShutDownTask(void *p_arg);
static void MAIN_StartingUpTask(void *p_arg);
static void MAIN_DispTask(void *p_arg);
static void MAIN_CalibrationTask(void *p_arg);
static void MAIN_KeyTask(void *p_arg);
/*		创建服务任务		*/
static void MAIN_CreateServeTask(void);
/*
******************************************************************************
函数名称:	main
函数功能:   系统入口函数
输入参数:	none
输出参数:	none
返 回 值:   none
创建日期:   2015-08-28
注    意:   创建开机前就要进行的任务  
*******************************************************************************
*/
int main(void)
{ 
    /*      初始化操作系统     */
	OSInit();  
    
    /*      初始化设备驱动     */
    BSP_Init();

    /*      创建定时器任务            */
    OSTaskCreate(MAIN_TimerTask, NULL, (OS_STK *)&m_TimerStk[TASK_STK_SIZE-1], (INT8U)DEF_TIMER_STK_PRIO);
    
	/*		创建数据采集任务		*/
	OSTaskCreate(MAIN_DataSampTask, NULL, (OS_STK *)&m_DataSampStk[TASK_STK_SIZE-1], (INT8U)DEF_DATASAMP_STK_PRIO);
    
    /*      创建检测任务            */
    OSTaskCreate(MAIN_MonitorTask, NULL, (OS_STK *)&m_MonitorStk[TASK_STK_SIZE-1], (INT8U)DEF_MONITOR_STK_PRIO);
        
    /*		创建开机任务		    */
	OSTaskCreate(MAIN_StartingUpTask, NULL, (OS_STK *)&m_StartingUpStk[TASK_STK_SIZE-1], (INT8U)DEF_STARTING_UP_STK_PRIO);		
    
    /*      启动操作系统      */
	OSStart();
    
    return (1);
}
/*
******************************************************************************
函数名称:	MAIN_CreateServeTask
函数功能:   创建服务任务
输入参数:	none
输出参数:	none
返 回 值:   none
创建日期:   2015-08-28
注    意:   创建的是开机后进行的任务 
*******************************************************************************
*/
static void MAIN_CreateServeTask(void)
{
    /*		创建显示任务		*/
	OSTaskCreate(MAIN_DispTask, NULL, (OS_STK *)&m_DispStk[TASK_STK_SIZE-1], (INT8U)DEF_DISP_STK_PRIO);
	/*		创建按键任务		*/
	OSTaskCreate(MAIN_KeyTask, NULL, (OS_STK *)&m_KeyStk[TASK_STK_SIZE-1], (INT8U)DEF_KEY_STK_PRIO);
    /*      创建校准任务        */
    OSTaskCreate(MAIN_CalibrationTask, NULL, (OS_STK *)&m_CalibrationStk[TASK_STK_SIZE-1], (INT8U)DEF_CALIBRATION_STK_PRIO);
	/*		创建关机任务		*/
	OSTaskCreate(MAIN_ShutDownTask, NULL, (OS_STK *)&m_ShutDownStk[TASK_STK_SIZE-1], (INT8U)DEF_SHUT_DOWN_STK_PRIO);	
}
/*
*****************************************************
函数名称:	MAIN_StartingUpTask
函数功能:   开机任务执行函数
输入参数:	png，传入任务的数据指针
输出参数:	none
返 回 值:   none
创建日期:   2015-08-28
注    意:    
*******************************************************************************
*/
static void MAIN_StartingUpTask(void *p_arg)
{
	p_arg = p_arg;
    
	for(;;)
	{
        /*      
            执行开机模块，
            开机异常时，程序将锁死在STARTINGUP_Doing()无法跳出
        */
        STARTINGUP_Exec();
        
        /*      正常开机，创建其他服务任务       */
        MAIN_CreateServeTask();
        
        /*      挂起开机任务      */
        OSTaskSuspend((INT8U)DEF_STARTING_UP_STK_PRIO);
	}
}
/*
******************************************************************************
函数名称:	MAIN_ShutDownTask
函数功能:   关机任务执行函数
输入参数:	png，传入任务的数据指针
输出参数:	none
返 回 值:   none
创建日期:   2015-08-28
注    意:    
*******************************************************************************
*/
static void MAIN_ShutDownTask(void *p_arg)
{
	p_arg = p_arg;
    
	for (;;)
	{
        /*执行关机模块*/
        SHUTDOWN_Exec();
		OSTimeDlyHMSM(0, 0, 0, DEF_SHUT_DOWN_STK_DELAY_TIME); 
	}	
}
/*
******************************************************************************
函数名称:	MAIN_TimerTask
函数功能:   定时器任务
输入参数:	png，传入任务的数据指针
输出参数:	none
返 回 值:   none
创建日期:   2015-09-01
注    意:    
*******************************************************************************
*/
static void MAIN_TimerTask(void *p_arg)
{
    p_arg = p_arg;
    
    TIME_Init();
    
	for(;;)
	{
        /*执行定时器模块*/
        TIME_NormalTimerExec();        
        OSTimeDlyHMSM(0, 0, 0, DEF_TIMER_STK_DELAY_TIME);   
    }
}

/*
******************************************************************************
函数名称:	MAIN_DataSampTask
函数功能:   数据采样任务
输入参数:	png，传入任务的数据指针
输出参数:	none
返 回 值:   none
创建日期:   2015-08-28
注    意:    
*******************************************************************************
*/
static void MAIN_DataSampTask(void *p_arg)
{
    p_arg = p_arg;
    
    DATASAMP_Init();
    
	for(;;)
	{
        /*执行数据采集模块*/
        DATASAMP_Exec();
        OSTimeDlyHMSM(0, 0, 0, DEF_DATASAMP_STK_DELAY_TIME);   
    }
}
/*
******************************************************************************
函数名称:	MAIN_MonitorTask
函数功能:   监测任务
输入参数:	png，传入任务的数据指针
输出参数:	none
返 回 值:   none
创建日期:   2015-09-01
注    意:    
*******************************************************************************
*/
static void MAIN_MonitorTask(void *p_arg)
{
    p_arg = p_arg;
    
	for(;;)
	{
        /*执行监测模块*/
        MONITOR_Exec();
        OSTimeDlyHMSM(0, 0, 0, DEF_MONITOR_STK_DELAY_TIME);   
    }
}
/*
******************************************************************************
函数名称:	MAIN_DispTask
函数功能:   显示任务
输入参数:	png，传入任务的数据指针
输出参数:	none
返 回 值:   none
创建日期:   2015-09-01
注    意:    
*******************************************************************************
*/
static void MAIN_DispTask(void *p_arg)
{
    p_arg = p_arg;
    
	for(;;)
	{
        /*执行显示模块*/
        DISPLAY_Exec();
        OSTimeDlyHMSM(0, 0, 0, DEF_DISP_STK_DELAY_TIME);   
    }
}
/*
******************************************************************************
函数名称:	MAIN_KeyTask
函数功能:   按键任务
输入参数:	png，传入任务的数据指针
输出参数:	none
返 回 值:   none
创建日期:   2015-09-01
注    意:    
*******************************************************************************
*/
static void MAIN_KeyTask(void *p_arg)
{
    p_arg = p_arg;   
    
	for(;;)
	{
        /*执行按键模块*/
        KEYAPP_Exec();
        OSTimeDlyHMSM(0, 0, 0, DEF_KEY_STK_DELAY_TIME);   
    }
}
/*
******************************************************************************
函数名称:	MAIN_CalibrationTask
函数功能:   校准任务
输入参数:	png，传入任务的数据指针
输出参数:	none
返 回 值:   none
创建日期:   2015-09-01
注    意:    
*******************************************************************************
*/
static void MAIN_CalibrationTask(void *p_arg)
{
    p_arg = p_arg;
  
	for(;;)
	{
        /*执行校准模块*/
        CALIBRATION_Exec();
        OSTimeDlyHMSM(0, 0, 0, DEF_CALIBRATION_STK_DELAY_TIME);   
    }
}
