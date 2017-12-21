/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: ShartingUp.c
* 摘    要: 电源开机模块程序
*
* 当前版本: 1.0
* 作    者: 曾健
* 完成日期: 2015-09-01
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
#include "ShutDown.h"
#include "StartingUp.h"
#include "Alarm.h"
#include "Key.h"
#include "DataSamp.h"

/*
********************************************************************************
*                               宏定义
********************************************************************************
*/
/*		任务堆栈大小		*/
#define TASK_STK_SIZE				256u

/*      任务优先级       */
#define DEF_ALARM_STK_PRIO              10u

/*      轮询时间        */
#define DEF_ALARM_STK_DELAY_TIME              100u
/*
********************************************************************************
*                               内部变量
********************************************************************************
*/
/*      报警任务堆栈      */
static OS_STK         m_StAlarmStk[TASK_STK_SIZE];
/*      开机完成标志,1是开机完成，0开机未完成        */
static BOOLEAN        m_StStaringFlag = DEF_STARTUP_OFF;
/*      保存校准参数      */
static INT8U          m_StBufferCalibration[30] = {0};
/*      开机自检标志，1是自检开启，0是自检完成      */
static BOOLEAN        m_StStaringUpCheckFlag = DEF_STARTUP_OFF;
/*      自检过程氧电池错误表示     */
static BOOLEAN        m_StFiO2ErrorFlag = DEF_STARTUP_OFF;
/*
********************************************************************************
********************************************************************************
*                               内部函数
********************************************************************************
********************************************************************************
*/
static void STARTINGUP_AlarmTask(void *p_arg);
static void STARTINTUP_Check(void);
/*
******************************************************************************
函数名称:   STARTINGUP_Check
函数功能:   开机自检函数
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-09-01
注    意:   
*******************************************************************************
*/
static void STARTINTUP_Check(void)
{
    INT32U cnt = 4;
    INT32U i = 0;
    
    OLED_Display(0);
    
    while(cnt > 1)
    {
        cnt--;
        
        /*      电池指示灯闪烁     */
        if(cnt % 2 == 1)
        {
            LED_Set(DEF_LED_BATERRY_RED,DEF_LED_OPEN);
            LED_Set(DEF_LED_BATERRY_GREEN,DEF_LED_CLOSE);
        }
        else
        {
            LED_Set(DEF_LED_BATERRY_RED,DEF_LED_CLOSE);
            LED_Set(DEF_LED_BATERRY_GREEN,DEF_LED_OPEN);
        }
        
        /*      适配器、高氧、低氧、报警静音灯亮        */
        LED_Set(DEF_LED_ADAPT,DEF_LED_OPEN);
        LED_Set(DEF_LED_HIGH_FIO2,DEF_LED_OPEN);
        LED_Set(DEF_LED_LOW_FIO2,DEF_LED_OPEN);
        LED_Set(DEF_LED_ALARM_SOUND,DEF_LED_OPEN);
               
        OSTimeDlyHMSM(0, 0, 0, 100);      
        
        /*      气道压LED灯循环闪烁      */
        for(i = 0; i < 15; i++)
        {
            PAW_LedSetOne(i, DEF_PAW_LED_ON);

            if(0 == i)
            {
                PAW_LedSetOne(14, DEF_PAW_LED_OFF);
            }
            else
            {
                PAW_LedSetOne((i - 1), DEF_PAW_LED_OFF);
            }
            OSTimeDlyHMSM(0, 0, 0, 100);
        }
        
    }
    /*      高氧、低氧、报警静音、电池、适配器、气道压低压LED灯灭        */
    LED_Set(DEF_LED_HIGH_FIO2,DEF_LED_CLOSE);
    LED_Set(DEF_LED_LOW_FIO2,DEF_LED_CLOSE);
    LED_Set(DEF_LED_ALARM_SOUND,DEF_LED_CLOSE);
    LED_Set(DEF_LED_BATERRY_RED,DEF_LED_CLOSE);
    LED_Set(DEF_LED_ADAPT,DEF_LED_CLOSE);
    PAW_LedSetOne(14, DEF_PAW_LED_OFF);
    
    /*      监测氧浓度电池     */
    if(DATASAMP_GetVoltage(ENUM_O2) < 0.4)
    {
        /*      显示错误提示信息001     */
        OLED_Display(1);
        m_StFiO2ErrorFlag = DEF_STARTUP_ON;
    }
}
/*
********************************************************************************
********************************************************************************
*                               接口函数
********************************************************************************
********************************************************************************
*/
/*
******************************************************************************
函数名称:   STARTINGUP_Exec
函数功能:   开机执行函数
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-09-01
注    意:   none
*******************************************************************************
*/
void STARTINGUP_Exec(void)
{
    /*      监测等待开机键的按下      */
    INT8U key_value = 0;
    
    while(1)
    {
        key_value = KEY_Scan();
        if(key_value == DEF_KEY_CODE_PPOWER_SWITCH)
        {
          OSTimeDlyHMSM(0, 0, 0, 100);
          break;
        }
        OSTimeDlyHMSM(0, 0, 0, 100); 
    }
    
    /*      开通各路电源           */
    POWER_SwitchSet(DEF_EN_DC, DEF_POWER_OPEN);
    POWER_SwitchSet(DEF_DC_DC, DEF_POWER_OPEN);
    POWER_SwitchSet(DEF_ON_STABLE, DEF_POWER_OPEN);
    POWER_SwitchSet(DEF_PRE_CONTROL, DEF_POWER_OPEN);
    OSTimeDlyHMSM(0, 0, 0, 50); 
    
    /*		创建报警任务		*/
	OSTaskCreate(STARTINGUP_AlarmTask, NULL, (OS_STK *)&m_StAlarmStk[TASK_STK_SIZE-1], (INT8U)DEF_ALARM_STK_PRIO);
    
    /*      自检标志置1，让蜂鸣器报警开启     */
    m_StStaringUpCheckFlag = DEF_STARTUP_ON;
    OSTimeDlyHMSM(0, 0, 0, 50);
    
    /*      开机自检        */
    STARTINTUP_Check();
    
    /*      自检完成之后置0，关闭蜂鸣器报警        */
    m_StStaringUpCheckFlag = DEF_STARTUP_OFF;

    /*      获取EEPROM中的校准的参数值        */
    I2C_EE_BufferRead(EEPROM_DEVICE_ADDRESS,m_StBufferCalibration, 0, 30);
    
    /*      开机完成标志      */
    m_StStaringFlag = DEF_STARTUP_ON;
}
/*
******************************************************************************
函数名称:	STARTINGUP_AlarmTask
函数功能:   报警任务
输入参数:	png，传入任务的数据指针
输出参数:	none
返 回 值:   none
创建日期:   2015-09-01
注    意:    
*******************************************************************************
*/
static void STARTINGUP_AlarmTask(void *p_arg)
{
    p_arg = p_arg;

    ALARM_Init();
    
	for(;;)
	{
        /*报警执行模块*/
        ALARM_Exec();          
        OSTimeDlyHMSM(0, 0, 0, DEF_ALARM_STK_DELAY_TIME);   
    }
}
/*
******************************************************************************
函数名称:   STARTINGUP_GetFlag
函数功能:   获取开机标志
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-09-01
注    意:   none
*******************************************************************************
*/
INT8U STARTINGUP_GetFlag(void)
{
    return m_StStaringFlag;
}
/*
******************************************************************************
函数名称:   STARTINGUP_GetcCalibValue
函数功能:   获得校准参数的值。
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-09-02
注    意:   none
*******************************************************************************
*/
INT8U STARTINGUP_GetCalibValue(INT16U num)
{
    return m_StBufferCalibration[num];
}
/*
******************************************************************************
函数名称:   STARTINGUP_GetCheckFlag
函数功能:   获得自检标志的值。
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-09-02
注    意:   none
*******************************************************************************
*/
INT8U STARTINGUP_GetCheckFlag(void)
{
    return m_StStaringUpCheckFlag;
}
/*
******************************************************************************
函数名称:   STARTINGUP_FiO2ErrorFlag
函数功能:   获得自检标志的值。
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-10-19
注    意:   none
*******************************************************************************
*/
BOOLEAN STARTINGUP_FiO2ErrorFlag(void)
{
    return m_StFiO2ErrorFlag;
}