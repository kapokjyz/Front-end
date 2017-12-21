/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: Main.c
* ժ    Ҫ: ϵͳ���ģ��
*
* ��ǰ�汾: 1.0
* ��    ��: ����
* �������: 2015-08-28
* ��    ��:
* ע    ��: none                                                                  
*******************************************************************************
*/

/*
*******************************************************************************
*                               ����ͷ�ļ�
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
*                               �궨��
********************************************************************************
*/
/*		�����ջ��С		*/
#define TASK_STK_SIZE				256u

/*		 �������ȼ�		*/
#define DEF_TIMER_STK_PRIO              4u
#define DEF_DATASAMP_STK_PRIO			6u
#define DEF_DISP_STK_PRIO               8u
#define DEF_MONITOR_STK_PRIO            9u
#define DEF_KEY_STK_PRIO                12u

#define DEF_CALIBRATION_STK_PRIO        14u
#define DEF_SHUT_DOWN_STK_PRIO		    16u
#define DEF_STARTING_UP_STK_PRIO		18u

/*		 ������ѯ����		*/
#define DEF_TIMER_STK_DELAY_TIME              50u
#define DEF_DATASAMP_STK_DELAY_TIME			  10u
#define DEF_DISP_STK_DELAY_TIME               200u
#define DEF_MONITOR_STK_DELAY_TIME           100u
#define DEF_KEY_STK_DELAY_TIME                100u
#define DEF_CALIBRATION_STK_DELAY_TIME        100u
#define DEF_SHUT_DOWN_STK_DELAY_TIME		  200u

/*
********************************************************************************
*                               �ڲ�����
********************************************************************************
*/
/*		�����ջ		*/
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
*                               �ڲ�����
********************************************************************************
*/
/*
********************************************************************************
********************************************************************************
*                               ���ܺ���
********************************************************************************
********************************************************************************
*/
/*		����ִ�к���		*/
static void MAIN_TimerTask(void *p_arg);
static void MAIN_DataSampTask(void *p_arg);
static void MAIN_MonitorTask(void *p_arg);
static void MAIN_ShutDownTask(void *p_arg);
static void MAIN_StartingUpTask(void *p_arg);
static void MAIN_DispTask(void *p_arg);
static void MAIN_CalibrationTask(void *p_arg);
static void MAIN_KeyTask(void *p_arg);
/*		������������		*/
static void MAIN_CreateServeTask(void);
/*
******************************************************************************
��������:	main
��������:   ϵͳ��ں���
�������:	none
�������:	none
�� �� ֵ:   none
��������:   2015-08-28
ע    ��:   ��������ǰ��Ҫ���е�����  
*******************************************************************************
*/
int main(void)
{ 
    /*      ��ʼ������ϵͳ     */
	OSInit();  
    
    /*      ��ʼ���豸����     */
    BSP_Init();

    /*      ������ʱ������            */
    OSTaskCreate(MAIN_TimerTask, NULL, (OS_STK *)&m_TimerStk[TASK_STK_SIZE-1], (INT8U)DEF_TIMER_STK_PRIO);
    
	/*		�������ݲɼ�����		*/
	OSTaskCreate(MAIN_DataSampTask, NULL, (OS_STK *)&m_DataSampStk[TASK_STK_SIZE-1], (INT8U)DEF_DATASAMP_STK_PRIO);
    
    /*      �����������            */
    OSTaskCreate(MAIN_MonitorTask, NULL, (OS_STK *)&m_MonitorStk[TASK_STK_SIZE-1], (INT8U)DEF_MONITOR_STK_PRIO);
        
    /*		������������		    */
	OSTaskCreate(MAIN_StartingUpTask, NULL, (OS_STK *)&m_StartingUpStk[TASK_STK_SIZE-1], (INT8U)DEF_STARTING_UP_STK_PRIO);		
    
    /*      ��������ϵͳ      */
	OSStart();
    
    return (1);
}
/*
******************************************************************************
��������:	MAIN_CreateServeTask
��������:   ������������
�������:	none
�������:	none
�� �� ֵ:   none
��������:   2015-08-28
ע    ��:   �������ǿ�������е����� 
*******************************************************************************
*/
static void MAIN_CreateServeTask(void)
{
    /*		������ʾ����		*/
	OSTaskCreate(MAIN_DispTask, NULL, (OS_STK *)&m_DispStk[TASK_STK_SIZE-1], (INT8U)DEF_DISP_STK_PRIO);
	/*		������������		*/
	OSTaskCreate(MAIN_KeyTask, NULL, (OS_STK *)&m_KeyStk[TASK_STK_SIZE-1], (INT8U)DEF_KEY_STK_PRIO);
    /*      ����У׼����        */
    OSTaskCreate(MAIN_CalibrationTask, NULL, (OS_STK *)&m_CalibrationStk[TASK_STK_SIZE-1], (INT8U)DEF_CALIBRATION_STK_PRIO);
	/*		�����ػ�����		*/
	OSTaskCreate(MAIN_ShutDownTask, NULL, (OS_STK *)&m_ShutDownStk[TASK_STK_SIZE-1], (INT8U)DEF_SHUT_DOWN_STK_PRIO);	
}
/*
*****************************************************
��������:	MAIN_StartingUpTask
��������:   ��������ִ�к���
�������:	png���������������ָ��
�������:	none
�� �� ֵ:   none
��������:   2015-08-28
ע    ��:    
*******************************************************************************
*/
static void MAIN_StartingUpTask(void *p_arg)
{
	p_arg = p_arg;
    
	for(;;)
	{
        /*      
            ִ�п���ģ�飬
            �����쳣ʱ������������STARTINGUP_Doing()�޷�����
        */
        STARTINGUP_Exec();
        
        /*      ��������������������������       */
        MAIN_CreateServeTask();
        
        /*      ���𿪻�����      */
        OSTaskSuspend((INT8U)DEF_STARTING_UP_STK_PRIO);
	}
}
/*
******************************************************************************
��������:	MAIN_ShutDownTask
��������:   �ػ�����ִ�к���
�������:	png���������������ָ��
�������:	none
�� �� ֵ:   none
��������:   2015-08-28
ע    ��:    
*******************************************************************************
*/
static void MAIN_ShutDownTask(void *p_arg)
{
	p_arg = p_arg;
    
	for (;;)
	{
        /*ִ�йػ�ģ��*/
        SHUTDOWN_Exec();
		OSTimeDlyHMSM(0, 0, 0, DEF_SHUT_DOWN_STK_DELAY_TIME); 
	}	
}
/*
******************************************************************************
��������:	MAIN_TimerTask
��������:   ��ʱ������
�������:	png���������������ָ��
�������:	none
�� �� ֵ:   none
��������:   2015-09-01
ע    ��:    
*******************************************************************************
*/
static void MAIN_TimerTask(void *p_arg)
{
    p_arg = p_arg;
    
    TIME_Init();
    
	for(;;)
	{
        /*ִ�ж�ʱ��ģ��*/
        TIME_NormalTimerExec();        
        OSTimeDlyHMSM(0, 0, 0, DEF_TIMER_STK_DELAY_TIME);   
    }
}

/*
******************************************************************************
��������:	MAIN_DataSampTask
��������:   ���ݲ�������
�������:	png���������������ָ��
�������:	none
�� �� ֵ:   none
��������:   2015-08-28
ע    ��:    
*******************************************************************************
*/
static void MAIN_DataSampTask(void *p_arg)
{
    p_arg = p_arg;
    
    DATASAMP_Init();
    
	for(;;)
	{
        /*ִ�����ݲɼ�ģ��*/
        DATASAMP_Exec();
        OSTimeDlyHMSM(0, 0, 0, DEF_DATASAMP_STK_DELAY_TIME);   
    }
}
/*
******************************************************************************
��������:	MAIN_MonitorTask
��������:   �������
�������:	png���������������ָ��
�������:	none
�� �� ֵ:   none
��������:   2015-09-01
ע    ��:    
*******************************************************************************
*/
static void MAIN_MonitorTask(void *p_arg)
{
    p_arg = p_arg;
    
	for(;;)
	{
        /*ִ�м��ģ��*/
        MONITOR_Exec();
        OSTimeDlyHMSM(0, 0, 0, DEF_MONITOR_STK_DELAY_TIME);   
    }
}
/*
******************************************************************************
��������:	MAIN_DispTask
��������:   ��ʾ����
�������:	png���������������ָ��
�������:	none
�� �� ֵ:   none
��������:   2015-09-01
ע    ��:    
*******************************************************************************
*/
static void MAIN_DispTask(void *p_arg)
{
    p_arg = p_arg;
    
	for(;;)
	{
        /*ִ����ʾģ��*/
        DISPLAY_Exec();
        OSTimeDlyHMSM(0, 0, 0, DEF_DISP_STK_DELAY_TIME);   
    }
}
/*
******************************************************************************
��������:	MAIN_KeyTask
��������:   ��������
�������:	png���������������ָ��
�������:	none
�� �� ֵ:   none
��������:   2015-09-01
ע    ��:    
*******************************************************************************
*/
static void MAIN_KeyTask(void *p_arg)
{
    p_arg = p_arg;   
    
	for(;;)
	{
        /*ִ�а���ģ��*/
        KEYAPP_Exec();
        OSTimeDlyHMSM(0, 0, 0, DEF_KEY_STK_DELAY_TIME);   
    }
}
/*
******************************************************************************
��������:	MAIN_CalibrationTask
��������:   У׼����
�������:	png���������������ָ��
�������:	none
�� �� ֵ:   none
��������:   2015-09-01
ע    ��:    
*******************************************************************************
*/
static void MAIN_CalibrationTask(void *p_arg)
{
    p_arg = p_arg;
  
	for(;;)
	{
        /*ִ��У׼ģ��*/
        CALIBRATION_Exec();
        OSTimeDlyHMSM(0, 0, 0, DEF_CALIBRATION_STK_DELAY_TIME);   
    }
}
