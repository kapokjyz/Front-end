/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: Alarm.c
* 摘    要: 报警模块执行文件
*
* 当前版本: 1.0
* 作    者: 曾健
* 完成日期: 2015-09-24
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
*                               宏定义
********************************************************************************
*/
/*      报警静音标志      */
#define DEF_ALARM_SOUND_DISABLE    1
#define DEF_ALARM_SOUND_ENABLE     0

/*		报警优先级：高中低		*/
#define DEF_ALARM_HIGH_PRIO				0
#define DEF_ALARM_MID_PRIO					1
	
/*		高报警蜂鸣器输入脉冲数量		*/
#define DEF_ALARM_HIGH_BUZZER_COUNT		21

/*		高报警LED闪烁频率：250ms=5*50ms		*/
#define DEF_ALARM_HIGH_LED_FLICKER_RATE		5

/*		报警静音时间：30s=600*50sm		*/
#define DEF_ALARM_SOUND_DISABLE_TIME			600

/*
********************************************************************************
*                               结构定义
********************************************************************************
*/
/*		声音报警控制器		*/
typedef struct 
{
	INT16U Ticked;				/*		报警周期计数器		*/
	INT16U TotalProgress;		/*		报警总进度		*/
	INT16U CurProgress;			/*		报警当前进度		*/
	INT16U *pTable;				/*		报警进度表指针		*/
	INT8U Type;					/*		报警类型：0，蜂鸣器报警，1，语音播报报警		*/
	INT8U OutputStatus;			/*		报警输出状态		*/
	INT8U Prio;					/*		报警优先级		*/
	INT8U Start;				/*		启动标志		*/
	INT8U Open;					/*		报警开启标志		*/
}VOICE_ALARM_CTR;

/*		LED报警控制器		*/
typedef struct 
{
	INT16U Ticked;				/*		报警周期计数器		*/
	INT16U TotalProgress;		/*		报警总进度		*/
	INT16U CurProgress;			/*		报警进度		*/
	INT16U FlickerRate;			/*		闪烁频率*/
	INT8U OutputStatus;			/*		报警输出状态		*/
	INT8U Prio;					/*		报警优先级		*/
	INT8U Start;				/*		报警启动标志		*/
	INT8U Open;					/*		报警开启标志		*/
}LED_ALARM_CTR;


/*		报警控制器		*/
typedef struct
{	
	ALARM_Table TotalAlarmTable;        /*      报警表       */
	TIMER_HANDLE hAlarmTimer;           /*  	报警输出定时器	    */	  
	TIMER_HANDLE hSoundDisableTimer;    /*      报警静音定时器     */
	INT8U DisableSound;                 /*      报警静音状态      */
	INT8U CurHighAlarmPrio;             /*      当前最高报警优先级       */
	VOICE_ALARM_CTR VoiceAlarmCtr;      /*      声音报警控制器     */
	LED_ALARM_CTR LedAlarmCtr[7];       /*      LED指示灯报警控制器     */
}ALARM_CTR;
/*
********************************************************************************
*                               内部变量
********************************************************************************
*/
/*		
	高报警-蜂鸣器滴滴声周期	
*/
static INT16U m_AlHighAlarmBuzzer[DEF_ALARM_HIGH_BUZZER_COUNT]=
{	
/*
*******************************************
			
*********************************************
*/
	/*		
			第一个脉冲群：滴-滴-滴，滴-滴（单位：百毫秒）
			  --2--         1       --2--     1       --2--                  3             --2--       1      --2--			     10
			________	    ________		 ________			      ________        ________
			|		|_____|	          |_____|		 |_______________|		 |_____|		    |_____________________________
			  --滴--            --滴--          --滴--                              --滴--           --滴--
	*/    
		    0,	    4,    6,	     10,    12,     16,              22,     26,   28,        32,
	/*	
			第二个脉冲群：滴-滴-滴，滴-滴（单位：百毫秒）
			  --2--    1    --2--     1    --2--         3           --2--    1    --2--			                         70
			________	    ________		 ________				  ________            ________
			|		|_____|	          |_____|		 |_______________|		   |_____|		  |______________________________________________________|
			 --滴--         --滴--         --滴--                                  --滴--           --滴--
	*/  
		    52,   56,    58,        62,    64,      68,              74,     78,     80,     84,																		    224     		
/*
********************************************
	
*********************************************
*/
};
/*	报警控制器   */
static ALARM_CTR m_AlarmCtr;
/*  报警静音时间  */
static INT32U    m_AlSoundDisableTime = DEF_ALARM_SOUND_DISABLE_TIME;
/*
********************************************************************************
*                               内部函数
********************************************************************************
*/

/*
********************************************************************************
********************************************************************************
*                               内部函数
********************************************************************************
********************************************************************************
*/
/*
******************************************************************************
函数名称:	ALARM_VoiceOutput
函数功能:   声音报警输出函数
输入参数:	none
输出参数:	none
返 回 值:   none
创建日期:  2015-09-24
注    意:    
*******************************************************************************
*/
static void ALARM_VoiceOutput(void)
{
    /*      初始化报警初值     */
	if (!m_AlarmCtr.VoiceAlarmCtr.Start && m_AlarmCtr.VoiceAlarmCtr.Open)
	{
		m_AlarmCtr.VoiceAlarmCtr.Open = 0;
		m_AlarmCtr.VoiceAlarmCtr.Start = 1;
		m_AlarmCtr.VoiceAlarmCtr.Type = 0;
		m_AlarmCtr.VoiceAlarmCtr.Prio = DEF_ALARM_HIGH_PRIO;
		m_AlarmCtr.VoiceAlarmCtr.Ticked = 0;
		m_AlarmCtr.VoiceAlarmCtr.CurProgress = 0;
		m_AlarmCtr.VoiceAlarmCtr.OutputStatus = 0;
		
		/*		高优先级蜂鸣器报警		*/
		m_AlarmCtr.VoiceAlarmCtr.TotalProgress = DEF_ALARM_HIGH_BUZZER_COUNT;
		m_AlarmCtr.VoiceAlarmCtr.pTable =  m_AlHighAlarmBuzzer;		
	}
    
	if (m_AlarmCtr.VoiceAlarmCtr.Start)
	{          
            /*		当报警声音没有被禁止时，进行报警声音的输出		*/
        if(DEF_ALARM_SOUND_ENABLE == m_AlarmCtr.DisableSound)
        {
            if (m_AlarmCtr.VoiceAlarmCtr.Ticked++ == *m_AlarmCtr.VoiceAlarmCtr.pTable)
            {
                m_AlarmCtr.VoiceAlarmCtr.pTable++;
                
                /*		进度表中最后一个点不响		*/
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
                    
                /*      在当前报警进度等于总进度时，Start置0，从而重新初始化报警初值       */
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
函数名称:	ALARM_LedOutput
函数功能:   Led报警输出函数
输入参数:	none
输出参数:	none
返 回 值:   none
创建日期:   2015-09-24
注    意:    
*******************************************************************************
*/
static void ALARM_LedOutput(INT32U LED_Num)
{
    /*      初始化报警初值     */
	if (!m_AlarmCtr.LedAlarmCtr[LED_Num].Start && m_AlarmCtr.LedAlarmCtr[LED_Num].Open)
	{
		m_AlarmCtr.LedAlarmCtr[LED_Num].Start = 1;
		m_AlarmCtr.LedAlarmCtr[LED_Num].Open = 0;

		m_AlarmCtr.LedAlarmCtr[LED_Num].Prio = m_AlarmCtr.CurHighAlarmPrio;
		m_AlarmCtr.LedAlarmCtr[LED_Num].Ticked = 0;
		m_AlarmCtr.LedAlarmCtr[LED_Num].OutputStatus = DEF_LED_CLOSE;
		m_AlarmCtr.LedAlarmCtr[LED_Num].CurProgress = 0;
		m_AlarmCtr.LedAlarmCtr[LED_Num].TotalProgress = 2;

        /*		Led报警		*/
		m_AlarmCtr.LedAlarmCtr[LED_Num].FlickerRate = DEF_ALARM_HIGH_LED_FLICKER_RATE;
	}

	if(m_AlarmCtr.LedAlarmCtr[LED_Num].Start)
	{
        if(SHUTDOWN_Is() == DEF_SHUTDOWN_OFF)
        {
            if(m_AlarmCtr.LedAlarmCtr[LED_Num].Ticked++ == m_AlarmCtr.LedAlarmCtr[LED_Num].FlickerRate)
            {
                m_AlarmCtr.LedAlarmCtr[LED_Num].Ticked = 0;
                /*		LED报警输出		*/
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
                
                /*      在当前报警进度等于总进度时，Start置0，从而重新初始化报警初值       */
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
函数名称:	ALARM_Output
函数功能:   报警定时输出服务函数
输入参数:	none
输出参数:	none
返 回 值:   none
创建日期:   2015-09-24
注    意:    
*******************************************************************************
*/
static void ALARM_Output(void *parg)  
{
	/*		Voice报警		*/
	ALARM_VoiceOutput();

	/*		LED报警		*/
	ALARM_LedOutput(0);
	ALARM_LedOutput(1);
	ALARM_LedOutput(2);
	ALARM_LedOutput(3);
	ALARM_LedOutput(4);
    
    /*      定时器复位       */
	TIME_TimerReset(*(TIMER_HANDLE *)parg,DEF_TIMER_NORMAL_TYPE);
}
/*
******************************************************************************
函数名称:	ALARM_SoundDisable
函数功能:   报警静音定时服务函数
输入参数:	none
输出参数:	none
返 回 值:   none
创建日期:   2015-09-24
注    意:    
*******************************************************************************
*/
static void ALARM_SoundDisable(void *parg)
{
    if(m_AlarmCtr.DisableSound == 1)
    {
        /*      进入静音后，就进行倒计时        */
        if(m_AlSoundDisableTime > 0)
        {
            m_AlSoundDisableTime--;
        }
        /*      到时间后，重置静音标志和静音时间        */
        else
        {  
            MONITOR_SetSoundDisable();
            m_AlSoundDisableTime = DEF_ALARM_SOUND_DISABLE_TIME;
        }
    }
    else
    {
        /*       不在静音状态时，重置静音时间     */
        m_AlSoundDisableTime = DEF_ALARM_SOUND_DISABLE_TIME;
    }
    
	TIME_TimerReset(*(TIMER_HANDLE *)parg,DEF_TIMER_NORMAL_TYPE);  
}
/*
******************************************************************************
函数名称:	ALARM_Write
函数功能:   写报警
输入参数:	code：报警代码
输出参数:	none
返 回 值:   none
创建日期:  2015-09-24
注    意:    
*******************************************************************************
*/
void ALARM_Write(ALARM_Table code)
{
	m_AlarmCtr.TotalAlarmTable = code;
}
/*
******************************************************************************
函数名称:	ALARM_Init
函数功能:   报警初始化函数
输入参数:	none
输出参数:	none
返 回 值:   none
创建日期:  2015-09-24
注    意:    
*******************************************************************************
*/
void ALARM_Init(void)
{	
	/*		初始化报警控制器		*/
	memset(&m_AlarmCtr, 0, sizeof(m_AlarmCtr));

	/*		创建报警定时器		*/
	m_AlarmCtr.hAlarmTimer = TIME_TimerCreate(ALARM_Output, (void *)&m_AlarmCtr.hAlarmTimer,DEF_TIMER_NORMAL_TYPE);
	/*      定时器的轮训周期改为50ms,所以这里的设置就是1*50=50ms        */
    TIME_TimerSet(m_AlarmCtr.hAlarmTimer, 0, 0, 0, 1);
	TIME_TimerStart(m_AlarmCtr.hAlarmTimer,DEF_TIMER_NORMAL_TYPE);
	
	/*		创建报警静音定时器		*/
	m_AlarmCtr.hSoundDisableTimer = TIME_TimerCreate(ALARM_SoundDisable, (void *)&m_AlarmCtr.hSoundDisableTimer,DEF_TIMER_NORMAL_TYPE);
	TIME_TimerSet(m_AlarmCtr.hSoundDisableTimer, 0, 0, 0, 1);
    TIME_TimerStart(m_AlarmCtr.hSoundDisableTimer,DEF_TIMER_NORMAL_TYPE);
}
/*
******************************************************************************
函数名称:	ALARM_Exec
函数功能:   报警执行函数
输入参数:	none
输出参数:	none
返 回 值:   none
创建日期:  2015-09-24
注    意:    
*******************************************************************************
*/
void ALARM_Exec(void)
{	
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
	OS_CPU_SR  cpu_sr = 0;
#endif
			
	OS_ENTER_CRITICAL();
    
    /*      开机自检过程中蜂鸣器响一个周期     */
    if(STARTINGUP_GetCheckFlag() == 1)
    {
        /*		开启Voice报警,关闭静音		*/      
        m_AlarmCtr.VoiceAlarmCtr.Open = 1;
        m_AlarmCtr.DisableSound = 0;
    }
    
    /*      氧电池没出错、开机后、非校准过程、非关机状态的情况下，报警再正常运行        */
    else if( (DEF_STARTUP_OFF == STARTINGUP_FiO2ErrorFlag()) && (DEF_STARTUP_ON == STARTINGUP_GetFlag()) 
      && (DEF_CAL_OFF == CALIBRATION_IsAdj()) && (DEF_SHUTDOWN_OFF == SHUTDOWN_Is()) )
    {
        /*		如果报警存在  */
        if ((m_AlarmCtr.TotalAlarmTable.FiO2_High_Alarm == 1) || (m_AlarmCtr.TotalAlarmTable.FiO2_Low_Alarm == 1)
            || (m_AlarmCtr.TotalAlarmTable.Paw_High_Alarm == 1) || (m_AlarmCtr.TotalAlarmTable.Paw_Low_Alarm == 1)
            || (m_AlarmCtr.TotalAlarmTable.Bat_Red_Alarm == 1))   
        {
            /*		开启Voice报警		*/
            m_AlarmCtr.VoiceAlarmCtr.Open = 1;
        }
        else
        {
            /*		关闭Voice报警		*/
            m_AlarmCtr.VoiceAlarmCtr.Open = 0;
        }

        /*		开启Led报警		*/            
        m_AlarmCtr.LedAlarmCtr[DEF_ALARM_CODE_HIGH_FiO2].Open = m_AlarmCtr.TotalAlarmTable.FiO2_High_Alarm;
        m_AlarmCtr.LedAlarmCtr[DEF_ALARM_CODE_LOW_FiO2].Open = m_AlarmCtr.TotalAlarmTable.FiO2_Low_Alarm;
        m_AlarmCtr.LedAlarmCtr[DEF_ALARM_CODE_HIGH_PAW].Open = m_AlarmCtr.TotalAlarmTable.Paw_High_Alarm;
        m_AlarmCtr.LedAlarmCtr[DEF_ALARM_CODE_LOW_PAW].Open = m_AlarmCtr.TotalAlarmTable.Paw_Low_Alarm;
        m_AlarmCtr.LedAlarmCtr[DEF_ALARM_CODE_SOUND_DESIABLE].Open = m_AlarmCtr.TotalAlarmTable.Sound_Disable_Alarm;
        /*      报警静音        */
        m_AlarmCtr.DisableSound = m_AlarmCtr.TotalAlarmTable.Sound_Disable_Alarm;  
    }
    else
    {
        /*		关闭报警，开启静音（让声音立马停止）		*/
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
函数名称:	ALARM_GetIsDisableSound
函数功能:   获取静音标志
输入参数:	none
输出参数:	1，静音；  0，没有静音
返 回 值:   none
创建日期:  2015-09-28
注    意:    
*******************************************************************************
*/
INT8U ALARM_GetIsDisableSound(void)
{
    return m_AlarmCtr.DisableSound;
}