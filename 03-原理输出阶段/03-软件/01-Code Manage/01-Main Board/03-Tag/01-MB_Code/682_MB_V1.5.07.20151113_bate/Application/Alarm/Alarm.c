/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: Alarm.c
* ժ    Ҫ: ����ģ��ִ���ļ�
*
* ��ǰ�汾: 1.0
* ��    ��: ����
* �������: 2015-09-24
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
#include "includes.h"
#include "Alarm.h"
#include "TIME_Timer.h"
#include "Monitor.h"
#include "Calibration.h"
#include "KeyApp.h"
#include "ShutDown.h"
#include "StartingUp.h"
/*
********************************************************************************
*                               �궨��
********************************************************************************
*/
/*      ����������־      */
#define DEF_ALARM_SOUND_DISABLE    1
#define DEF_ALARM_SOUND_ENABLE     0

/*		�������ȼ������е�		*/
#define DEF_ALARM_HIGH_PRIO				0
#define DEF_ALARM_MID_PRIO					1
	
/*		�߱���������������������		*/
#define DEF_ALARM_HIGH_BUZZER_COUNT		21

/*		�߱���LED��˸Ƶ�ʣ�250ms=5*50ms		*/
#define DEF_ALARM_HIGH_LED_FLICKER_RATE		5

/*		��������ʱ�䣺30s=600*50sm		*/
#define DEF_ALARM_SOUND_DISABLE_TIME			600

/*
********************************************************************************
*                               �ṹ����
********************************************************************************
*/
/*		��������������		*/
typedef struct 
{
	INT16U Ticked;				/*		�������ڼ�����		*/
	INT16U TotalProgress;		/*		�����ܽ���		*/
	INT16U CurProgress;			/*		������ǰ����		*/
	INT16U *pTable;				/*		�������ȱ�ָ��		*/
	INT8U Type;					/*		�������ͣ�0��������������1��������������		*/
	INT8U OutputStatus;			/*		�������״̬		*/
	INT8U Prio;					/*		�������ȼ�		*/
	INT8U Start;				/*		������־		*/
	INT8U Open;					/*		����������־		*/
}VOICE_ALARM_CTR;

/*		LED����������		*/
typedef struct 
{
	INT16U Ticked;				/*		�������ڼ�����		*/
	INT16U TotalProgress;		/*		�����ܽ���		*/
	INT16U CurProgress;			/*		��������		*/
	INT16U FlickerRate;			/*		��˸Ƶ��*/
	INT8U OutputStatus;			/*		�������״̬		*/
	INT8U Prio;					/*		�������ȼ�		*/
	INT8U Start;				/*		����������־		*/
	INT8U Open;					/*		����������־		*/
}LED_ALARM_CTR;


/*		����������		*/
typedef struct
{	
	ALARM_Table TotalAlarmTable;        /*      ������       */
	TIMER_HANDLE hAlarmTimer;           /*  	���������ʱ��	    */	  
	TIMER_HANDLE hSoundDisableTimer;    /*      ����������ʱ��     */
	INT8U DisableSound;                 /*      ��������״̬      */
	INT8U CurHighAlarmPrio;             /*      ��ǰ��߱������ȼ�       */
	VOICE_ALARM_CTR VoiceAlarmCtr;      /*      ��������������     */
	LED_ALARM_CTR LedAlarmCtr[7];       /*      LEDָʾ�Ʊ���������     */
}ALARM_CTR;
/*
********************************************************************************
*                               �ڲ�����
********************************************************************************
*/
/*		
	�߱���-�������ε�������	
*/
static INT16U m_AlHighAlarmBuzzer[DEF_ALARM_HIGH_BUZZER_COUNT]=
{	
/*
*******************************************
			
*********************************************
*/
	/*		
			��һ������Ⱥ����-��-�Σ���-�Σ���λ���ٺ��룩
			  --2--         1       --2--     1       --2--                  3             --2--       1      --2--			     10
			________	    ________		 ________			      ________        ________
			|		|_____|	          |_____|		 |_______________|		 |_____|		    |_____________________________
			  --��--            --��--          --��--                              --��--           --��--
	*/    
		    0,	    4,    6,	     10,    12,     16,              22,     26,   28,        32,
	/*	
			�ڶ�������Ⱥ����-��-�Σ���-�Σ���λ���ٺ��룩
			  --2--    1    --2--     1    --2--         3           --2--    1    --2--			                         70
			________	    ________		 ________				  ________            ________
			|		|_____|	          |_____|		 |_______________|		   |_____|		  |______________________________________________________|
			 --��--         --��--         --��--                                  --��--           --��--
	*/  
		    52,   56,    58,        62,    64,      68,              74,     78,     80,     84,																		    224     		
/*
********************************************
	
*********************************************
*/
};
/*	����������   */
static ALARM_CTR m_AlarmCtr;
/*  ��������ʱ��  */
static INT32U    m_AlSoundDisableTime = DEF_ALARM_SOUND_DISABLE_TIME;
/*
********************************************************************************
*                               �ڲ�����
********************************************************************************
*/

/*
********************************************************************************
********************************************************************************
*                               �ڲ�����
********************************************************************************
********************************************************************************
*/
/*
******************************************************************************
��������:	ALARM_VoiceOutput
��������:   ���������������
�������:	none
�������:	none
�� �� ֵ:   none
��������:  2015-09-24
ע    ��:    
*******************************************************************************
*/
static void ALARM_VoiceOutput(void)
{
    /*      ��ʼ��������ֵ     */
	if (!m_AlarmCtr.VoiceAlarmCtr.Start && m_AlarmCtr.VoiceAlarmCtr.Open)
	{
		m_AlarmCtr.VoiceAlarmCtr.Open = 0;
		m_AlarmCtr.VoiceAlarmCtr.Start = 1;
		m_AlarmCtr.VoiceAlarmCtr.Type = 0;
		m_AlarmCtr.VoiceAlarmCtr.Prio = DEF_ALARM_HIGH_PRIO;
		m_AlarmCtr.VoiceAlarmCtr.Ticked = 0;
		m_AlarmCtr.VoiceAlarmCtr.CurProgress = 0;
		m_AlarmCtr.VoiceAlarmCtr.OutputStatus = 0;
		
		/*		�����ȼ�����������		*/
		m_AlarmCtr.VoiceAlarmCtr.TotalProgress = DEF_ALARM_HIGH_BUZZER_COUNT;
		m_AlarmCtr.VoiceAlarmCtr.pTable =  m_AlHighAlarmBuzzer;		
	}
    
	if (m_AlarmCtr.VoiceAlarmCtr.Start)
	{          
            /*		����������û�б���ֹʱ�����б������������		*/
        if(DEF_ALARM_SOUND_ENABLE == m_AlarmCtr.DisableSound)
        {
            if (m_AlarmCtr.VoiceAlarmCtr.Ticked++ == *m_AlarmCtr.VoiceAlarmCtr.pTable)
            {
                m_AlarmCtr.VoiceAlarmCtr.pTable++;
                
                /*		���ȱ������һ���㲻��		*/
                if(m_AlarmCtr.VoiceAlarmCtr.Ticked == 225)
                {
                    m_AlarmCtr.VoiceAlarmCtr.OutputStatus = 0 ;
                    BUZZER_VoiceOutput(m_AlarmCtr.VoiceAlarmCtr.OutputStatus);
                }
                else
                {
                    m_AlarmCtr.VoiceAlarmCtr.OutputStatus = !m_AlarmCtr.VoiceAlarmCtr.OutputStatus ;
                    BUZZER_VoiceOutput(m_AlarmCtr.VoiceAlarmCtr.OutputStatus);
                }                   
                    
                /*      �ڵ�ǰ�������ȵ����ܽ���ʱ��Start��0���Ӷ����³�ʼ��������ֵ       */
                m_AlarmCtr.VoiceAlarmCtr.CurProgress++;                
                if ( m_AlarmCtr.VoiceAlarmCtr.CurProgress == m_AlarmCtr.VoiceAlarmCtr.TotalProgress)
                {
                    m_AlarmCtr.VoiceAlarmCtr.Start = 0;
                }	
            }
        }
        else
        {
            BUZZER_VoiceOutput(DEF_VOICE_OFF);
            m_AlarmCtr.VoiceAlarmCtr.Start = 0;
        }
    }
}
/*
******************************************************************************
��������:	ALARM_LedOutput
��������:   Led�����������
�������:	none
�������:	none
�� �� ֵ:   none
��������:   2015-09-24
ע    ��:    
*******************************************************************************
*/
static void ALARM_LedOutput(INT32U LED_Num)
{
    /*      ��ʼ��������ֵ     */
	if (!m_AlarmCtr.LedAlarmCtr[LED_Num].Start && m_AlarmCtr.LedAlarmCtr[LED_Num].Open)
	{
		m_AlarmCtr.LedAlarmCtr[LED_Num].Start = 1;
		m_AlarmCtr.LedAlarmCtr[LED_Num].Open = 0;

		m_AlarmCtr.LedAlarmCtr[LED_Num].Prio = m_AlarmCtr.CurHighAlarmPrio;
		m_AlarmCtr.LedAlarmCtr[LED_Num].Ticked = 0;
		m_AlarmCtr.LedAlarmCtr[LED_Num].OutputStatus = DEF_LED_CLOSE;
		m_AlarmCtr.LedAlarmCtr[LED_Num].CurProgress = 0;
		m_AlarmCtr.LedAlarmCtr[LED_Num].TotalProgress = 2;

        /*		Led����		*/
		m_AlarmCtr.LedAlarmCtr[LED_Num].FlickerRate = DEF_ALARM_HIGH_LED_FLICKER_RATE;
	}

	if(m_AlarmCtr.LedAlarmCtr[LED_Num].Start)
	{
        if(SHUTDOWN_Is() == DEF_SHUTDOWN_OFF)
        {
            if(m_AlarmCtr.LedAlarmCtr[LED_Num].Ticked++ == m_AlarmCtr.LedAlarmCtr[LED_Num].FlickerRate)
            {
                m_AlarmCtr.LedAlarmCtr[LED_Num].Ticked = 0;
                /*		LED�������		*/
                m_AlarmCtr.LedAlarmCtr[LED_Num].OutputStatus = !m_AlarmCtr.LedAlarmCtr[LED_Num].OutputStatus;
                switch(LED_Num)
                {
                    case DEF_ALARM_CODE_HIGH_FiO2:
                    {
                        LED_Set(DEF_LED_HIGH_FIO2,m_AlarmCtr.LedAlarmCtr[LED_Num].OutputStatus);
                        break;
                    }
                    case DEF_ALARM_CODE_LOW_FiO2:
                    {
                        LED_Set(DEF_LED_LOW_FIO2,m_AlarmCtr.LedAlarmCtr[LED_Num].OutputStatus);
                        break;
                    }
                    case DEF_ALARM_CODE_HIGH_PAW:
                    {
                        PAW_LedSetOne(DEF_PAW_HIGH_LED,m_AlarmCtr.LedAlarmCtr[LED_Num].OutputStatus);
                        break;
                    }
                    case DEF_ALARM_CODE_LOW_PAW:
                    {
                        PAW_LedSetOne(DEF_PAW_LOW_LED,m_AlarmCtr.LedAlarmCtr[LED_Num].OutputStatus);
                        break;
                    }
                    case DEF_ALARM_CODE_SOUND_DESIABLE:
                    {
                        LED_Set(DEF_LED_ALARM_SOUND,m_AlarmCtr.LedAlarmCtr[LED_Num].OutputStatus);
                    }               
                }
                
                /*      �ڵ�ǰ�������ȵ����ܽ���ʱ��Start��0���Ӷ����³�ʼ��������ֵ       */
                m_AlarmCtr.LedAlarmCtr[LED_Num].CurProgress++;
                if (m_AlarmCtr.LedAlarmCtr[LED_Num].CurProgress == m_AlarmCtr.LedAlarmCtr[LED_Num].TotalProgress)
                {
                    m_AlarmCtr.LedAlarmCtr[LED_Num].Start = 0;
                }
            }
        }
        else
        {
            LED_Set(DEF_LED_HIGH_FIO2, DEF_LED_CLOSE);
            LED_Set(DEF_LED_LOW_FIO2, DEF_LED_CLOSE);
        }
	}

}
/*
******************************************************************************
��������:	ALARM_Output
��������:   ������ʱ���������
�������:	none
�������:	none
�� �� ֵ:   none
��������:   2015-09-24
ע    ��:    
*******************************************************************************
*/
static void ALARM_Output(void *parg)  
{
	/*		Voice����		*/
	ALARM_VoiceOutput();

	/*		LED����		*/
	ALARM_LedOutput(0);
	ALARM_LedOutput(1);
	ALARM_LedOutput(2);
	ALARM_LedOutput(3);
	ALARM_LedOutput(4);
    
    /*      ��ʱ����λ       */
	TIME_TimerReset(*(TIMER_HANDLE *)parg,DEF_TIMER_NORMAL_TYPE);
}
/*
******************************************************************************
��������:	ALARM_SoundDisable
��������:   ����������ʱ������
�������:	none
�������:	none
�� �� ֵ:   none
��������:   2015-09-24
ע    ��:    
*******************************************************************************
*/
static void ALARM_SoundDisable(void *parg)
{
    if(m_AlarmCtr.DisableSound == 1)
    {
        /*      ���뾲���󣬾ͽ��е���ʱ        */
        if(m_AlSoundDisableTime > 0)
        {
            m_AlSoundDisableTime--;
        }
        /*      ��ʱ������þ�����־�;���ʱ��        */
        else
        {  
            MONITOR_SetSoundDisable();
            m_AlSoundDisableTime = DEF_ALARM_SOUND_DISABLE_TIME;
        }
    }
    else
    {
        /*       ���ھ���״̬ʱ�����þ���ʱ��     */
        m_AlSoundDisableTime = DEF_ALARM_SOUND_DISABLE_TIME;
    }
    
	TIME_TimerReset(*(TIMER_HANDLE *)parg,DEF_TIMER_NORMAL_TYPE);  
}
/*
******************************************************************************
��������:	ALARM_Write
��������:   д����
�������:	code����������
�������:	none
�� �� ֵ:   none
��������:  2015-09-24
ע    ��:    
*******************************************************************************
*/
void ALARM_Write(ALARM_Table code)
{
	m_AlarmCtr.TotalAlarmTable = code;
}
/*
******************************************************************************
��������:	ALARM_Init
��������:   ������ʼ������
�������:	none
�������:	none
�� �� ֵ:   none
��������:  2015-09-24
ע    ��:    
*******************************************************************************
*/
void ALARM_Init(void)
{	
	/*		��ʼ������������		*/
	memset(&m_AlarmCtr, 0, sizeof(m_AlarmCtr));

	/*		����������ʱ��		*/
	m_AlarmCtr.hAlarmTimer = TIME_TimerCreate(ALARM_Output, (void *)&m_AlarmCtr.hAlarmTimer,DEF_TIMER_NORMAL_TYPE);
	/*      ��ʱ������ѵ���ڸ�Ϊ50ms,������������þ���1*50=50ms        */
    TIME_TimerSet(m_AlarmCtr.hAlarmTimer, 0, 0, 0, 1);
	TIME_TimerStart(m_AlarmCtr.hAlarmTimer,DEF_TIMER_NORMAL_TYPE);
	
	/*		��������������ʱ��		*/
	m_AlarmCtr.hSoundDisableTimer = TIME_TimerCreate(ALARM_SoundDisable, (void *)&m_AlarmCtr.hSoundDisableTimer,DEF_TIMER_NORMAL_TYPE);
	TIME_TimerSet(m_AlarmCtr.hSoundDisableTimer, 0, 0, 0, 1);
    TIME_TimerStart(m_AlarmCtr.hSoundDisableTimer,DEF_TIMER_NORMAL_TYPE);
}
/*
******************************************************************************
��������:	ALARM_Exec
��������:   ����ִ�к���
�������:	none
�������:	none
�� �� ֵ:   none
��������:  2015-09-24
ע    ��:    
*******************************************************************************
*/
void ALARM_Exec(void)
{	
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
	OS_CPU_SR  cpu_sr = 0;
#endif
			
	OS_ENTER_CRITICAL();
    
    /*      �����Լ�����з�������һ������     */
    if(STARTINGUP_GetCheckFlag() == 1)
    {
        /*		����Voice����,�رվ���		*/      
        m_AlarmCtr.VoiceAlarmCtr.Open = 1;
        m_AlarmCtr.DisableSound = 0;
    }
    
    /*      �����û���������󡢷�У׼���̡��ǹػ�״̬������£���������������        */
    else if( (DEF_STARTUP_OFF == STARTINGUP_FiO2ErrorFlag()) && (DEF_STARTUP_ON == STARTINGUP_GetFlag()) 
      && (DEF_CAL_OFF == CALIBRATION_IsAdj()) && (DEF_SHUTDOWN_OFF == SHUTDOWN_Is()) )
    {
        /*		�����������  */
        if ((m_AlarmCtr.TotalAlarmTable.FiO2_High_Alarm == 1) || (m_AlarmCtr.TotalAlarmTable.FiO2_Low_Alarm == 1)
            || (m_AlarmCtr.TotalAlarmTable.Paw_High_Alarm == 1) || (m_AlarmCtr.TotalAlarmTable.Paw_Low_Alarm == 1)
            || (m_AlarmCtr.TotalAlarmTable.Bat_Red_Alarm == 1))   
        {
            /*		����Voice����		*/
            m_AlarmCtr.VoiceAlarmCtr.Open = 1;
        }
        else
        {
            /*		�ر�Voice����		*/
            m_AlarmCtr.VoiceAlarmCtr.Open = 0;
        }

        /*		����Led����		*/            
        m_AlarmCtr.LedAlarmCtr[DEF_ALARM_CODE_HIGH_FiO2].Open = m_AlarmCtr.TotalAlarmTable.FiO2_High_Alarm;
        m_AlarmCtr.LedAlarmCtr[DEF_ALARM_CODE_LOW_FiO2].Open = m_AlarmCtr.TotalAlarmTable.FiO2_Low_Alarm;
        m_AlarmCtr.LedAlarmCtr[DEF_ALARM_CODE_HIGH_PAW].Open = m_AlarmCtr.TotalAlarmTable.Paw_High_Alarm;
        m_AlarmCtr.LedAlarmCtr[DEF_ALARM_CODE_LOW_PAW].Open = m_AlarmCtr.TotalAlarmTable.Paw_Low_Alarm;
        m_AlarmCtr.LedAlarmCtr[DEF_ALARM_CODE_SOUND_DESIABLE].Open = m_AlarmCtr.TotalAlarmTable.Sound_Disable_Alarm;
        /*      ��������        */
        m_AlarmCtr.DisableSound = m_AlarmCtr.TotalAlarmTable.Sound_Disable_Alarm;  
    }
    else
    {
        /*		�رձ�������������������������ֹͣ��		*/
        m_AlarmCtr.VoiceAlarmCtr.Open = 0;
        m_AlarmCtr.LedAlarmCtr[DEF_ALARM_CODE_HIGH_FiO2].Open = 0;
        m_AlarmCtr.LedAlarmCtr[DEF_ALARM_CODE_LOW_FiO2].Open = 0;
        m_AlarmCtr.LedAlarmCtr[DEF_ALARM_CODE_HIGH_PAW].Open = 0;
        m_AlarmCtr.LedAlarmCtr[DEF_ALARM_CODE_LOW_PAW].Open = 0;
        m_AlarmCtr.LedAlarmCtr[DEF_ALARM_CODE_SOUND_DESIABLE].Open = 0;
        m_AlarmCtr.DisableSound = 1;
    }
	OS_EXIT_CRITICAL();
}
/*
******************************************************************************
��������:	ALARM_GetIsDisableSound
��������:   ��ȡ������־
�������:	none
�������:	1��������  0��û�о���
�� �� ֵ:   none
��������:  2015-09-28
ע    ��:    
*******************************************************************************
*/
INT8U ALARM_GetIsDisableSound(void)
{
    return m_AlarmCtr.DisableSound;
}