/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: Monitoring.c
* 摘    要: 监测模块程序
*
* 当前版本: 1.0
* 作    者: 曾健
* 完成日期: 2015-9-03
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
#include "Monitor.h"
#include "DataSamp.h"
#include "includes.h"
#include "ShutDown.h"
#include "Alarm.h"
#include "Display.h"
#include "KeyApp.h"
#include "Calibration.h"
#include "StartingUp.h"
/*
********************************************************************************
*                               宏定义
********************************************************************************
*/
/*       充电监测                          */
#define          DEF_CHG_CHECK_PORT          (GPIOA)
#define          DEF_CHG_CHECK_PIN           (GPIO_Pin_3)

#define          DEF_BAT_CHARGE               0
#define          DEF_BAT_NOT_CHARGE           1
#define          DEF_BAT_NORMAL               0
#define          DEF_BAT_LOW                  1
#define          DEF_BAT_TOOLOW               1
 
/*      版本号     */
#define          DEF_VERSION                  1507
/*
********************************************************************************
*                               类型定义
********************************************************************************
*/
/*
********************************************************************************
*                               内部变量
********************************************************************************
*/ 
/*      控制电池指示灯闪烁       */
static BOOLEAN  m_MoOnOff = 0;
/*      保存报警内容的报警表      */
static ALARM_Table m_MoAlarmTable = {0, 0, 0, 0, 0, 0};
/*      备份报警静音时的报警情况        */
static ALARM_Table m_MoAlarmTableBak = {0, 0, 0, 0, 0, 0};
/*      按键的状态表       */
static INT8U  m_MoStatus[] = {'c', 0};
/*      按键状态表数组下标       */
static INT32U m_MoCount = 0;
/*      计算次数        */
static INT32U m_MoTicked = 0;
/*      保存氧浓度和气道压的报警上下限值        */
static INT8U  m_MoAlarmValue[4] = {0};
/*      保存是否设定报警设置值     */
static INT8U  m_MonitorAlarmSet = 0;
/*      记录电池单独供电低电量标志       */
static INT8U  m_MoBatLowFlag = DEF_BAT_NORMAL;
/*      保存氧浓度值      */
static INT8U  m_MoFiO2Value = 0;
/*      保存气道压值      */
static INT8U  m_MoPawValue = 0;
/*      保存按键标志      */
static INT8U m_MoKeyFlag = 0;
/*      保存显示版本号标志       */
static INT8U m_MoVerFlag = 0;
/*      电池电压过低标志        */
static INT8U m_MoBatTooLow = 0;
/*
********************************************************************************
*                               内部函数
********************************************************************************
*/
/*
********************************************************************************
* 函数名称: MONITOR_ChargeBAT
* 函数功能: 检查电池充电状态
* 入口参数: gpio_pin
* 出口参数: void
* 返 回 值: 1,  不充电      0, 正在充电
* 日    期: 2015-09-03
* 修改问题:
* 修改内容:
********************************************************************************
*/
static INT8U  MONITOR_ChargeBAT(INT16U gpio_pin) 
{
    CPU_INT08U flag = 0;

    if(GPIO_ReadInputDataBit(DEF_CHG_CHECK_PORT,gpio_pin) == 1)
        flag = 1;

    return flag;
}
/*
******************************************************************************
函数名称:   MONITOR_Voltage
函数功能:   监测电源电压。
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-10-20
注    意:   
*******************************************************************************
*/
static void MONITOR_Voltage(void)
{
    INT8U ChargeFlag = 0;
    FP32 ADAPTOR_Voltage = 0.0;
    FP32 BAT_Voltage = 0.0;
    
    ADAPTOR_Voltage = DATASAMP_GetVoltage(ENUM_ADAPTOR);
    BAT_Voltage = DATASAMP_GetVoltage(ENUM_BAT);
    
    /*避免一开始数据采集的还没稳定时，获取到的较小的电池电压值让仪器关机*/
    if(m_MoTicked < 6)
    {
        m_MoTicked++;
    }
    else
    {  
        /*      屏蔽自检时适配器和电池指示灯的影响       */
        if(STARTINGUP_GetCheckFlag() == DEF_STARTUP_OFF)
        {
            /*适配器电压大于4V，认为适配器存在*/
            if(ADAPTOR_Voltage >= 4.0)
            {
                LED_Set(DEF_LED_ADAPT, DEF_LED_OPEN);
                
                /*电池在位则充电，否则不充电*/
                if(BAT_Voltage >=  2.0)
                {
                    m_MoAlarmTable.Bat_Red_Alarm = 0;
                    m_MoBatLowFlag = DEF_BAT_NORMAL;
                    /*使能充电*/
                    POWER_SwitchSet(DEF_EN_CHG, DEF_POWER_OPEN);
                    /*检测电池充电状态*/
                    ChargeFlag = MONITOR_ChargeBAT(DEF_CHG_CHECK_PIN);
                    
                    /*电池正在充电*/
                    if(DEF_BAT_CHARGE == ChargeFlag) 
                    {
                        LED_Set(DEF_LED_BATERRY_RED, DEF_LED_CLOSE);

                        m_MoOnOff = !m_MoOnOff;

                        LED_Set(DEF_LED_BATERRY_GREEN, m_MoOnOff);
                        OSTimeDlyHMSM(0,0,0,150) ;  
                    }
                    else /*电池充满*/
                    {
                        
                        LED_Set(DEF_LED_BATERRY_GREEN, DEF_LED_OPEN);
                        LED_Set(DEF_LED_BATERRY_RED, DEF_LED_CLOSE);
                    }      
                }
                /*      电池不在位，则把电池指示灯灭掉      by zj 20150930*/
                else
                {
                    LED_Set(DEF_LED_BATERRY_GREEN, DEF_LED_CLOSE);
                    LED_Set(DEF_LED_BATERRY_RED, DEF_LED_CLOSE);
                }
            }        
            else
            {
                LED_Set(DEF_LED_ADAPT,DEF_LED_CLOSE);
                /*电池单独供电时，判断是否是低电量*/
                if((BAT_Voltage >=  3.6) && (BAT_Voltage <= 3.75))
                {
                    m_MoBatTooLow = 0;
                    m_MoBatLowFlag = DEF_BAT_LOW;                   
                }
                
                /*小于3.65伏，认为电池不存在，自动关机*/
                else if(BAT_Voltage < 3.6)
                { 
                    /*      连续监测到电池电压过低，再关机，防止电压波动      */
                    m_MoBatTooLow++;
                    if(m_MoBatTooLow > 10)
                    {
                        POWER_SwitchSet(DEF_EN_DC, DEF_POWER_CLOSE);
                        NVIC_GenerateSystemReset();
                    }
                }
                else
                {
                    /*      当电池单独供电时，在电池电压刚刚降到3.75V以下时，避免电压波动短暂出现高于3.75V时
                            出现红绿灯交替闪烁的情况。
                    */
                    if(DEF_BAT_NORMAL == m_MoBatLowFlag)
                    {                        
                        /*      在电池单独供电时，进入关机就关闭电池指示灯       */
                        if(DEF_SHUTDOWN_ON == SHUTDOWN_Is())
                        {
                           LED_Set(DEF_LED_BATERRY_GREEN, DEF_LED_CLOSE); 
                        }
                        else
                        {
                            /*      在电池电量充足时，红灯灭，绿灯常亮       */
                            LED_Set(DEF_LED_BATERRY_RED, DEF_LED_CLOSE);
                            LED_Set(DEF_LED_BATERRY_GREEN, DEF_LED_OPEN);
                        }
                    }
                }
                
                /*       当电池单独供电时，在电池电压刚刚降到3.75V以下时，为避免电压波动短暂出现高于3.75V时
                         出现红灯亮或者灭较长的时间不按固定频率亮灭的情况，在监测到电池电压过低时，适当
                         扩大电池低电压范围。（特别是蜂鸣器响停时，电池电压会出现明显波动）  
                */
                if( (DEF_BAT_LOW == m_MoBatLowFlag) && (DEF_SHUTDOWN_OFF == SHUTDOWN_Is()) )
                {
                    m_MoAlarmTable.Bat_Red_Alarm = 1;
                     /*      低电量时，绿灯灭，红灯闪烁       */
                    LED_Set(DEF_LED_BATERRY_GREEN, DEF_LED_CLOSE);

                    m_MoOnOff = !m_MoOnOff;
                        
                    LED_Set(DEF_LED_BATERRY_RED, m_MoOnOff);
                    OSTimeDlyHMSM(0,0,0,150);
                }
                else
                {
                    /*      电池单独供电，关机过程中，按键超过3秒没弹起就关闭电池指示灯      */
                    LED_Set(DEF_LED_BATERRY_RED, DEF_LED_CLOSE);
                }
            }
        }
        ALARM_Write(m_MoAlarmTable);
    }
}
/*
******************************************************************************
函数名称:   MONITOR_Alarm
函数功能:   监测报警情况。
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-10-20
注    意:   
*******************************************************************************
*/
static void MONITOR_Alarm(void)
{
    INT16U Ad_Paw = 0;
    INT16U Ad_FiO2 = 0;
    
    /*获取AD值*/
    Ad_Paw = DATASAMP_AdValue(ENUM_PAW);
    Ad_FiO2 = DATASAMP_AdValue(ENUM_O2);
    
    /*计算实际值*/
    m_MoPawValue = DATASAMP_AppCalPaw(Ad_Paw);
    m_MoFiO2Value = DATASAMP_AppCalO2(Ad_FiO2);
    
    if((DEF_STARTUP_ON == STARTINGUP_GetFlag()) && (DEF_SHUTDOWN_OFF == SHUTDOWN_Is()))
    {  
        /*设置报警限值成功时，读取报警上下限值*/
        if(KEYAPP_GetAlarmSetSucflag() == DEF_KEY_ON)
        {
            m_MonitorAlarmSet = 1;
            if(DEF_KEY_OFF == KEYAPP_GetAlarmSetValue(1))
            {
                m_MoAlarmValue[0] = KEYAPP_GetAlarmSetValue(0) - 5;
                m_MoAlarmValue[1] = KEYAPP_GetAlarmSetValue(0) + 5;
                m_MoAlarmValue[2] = 1;
                m_MoAlarmValue[3] = KEYAPP_GetAlarmSetValue(1) + 2;
            }
            else
            {
                m_MoAlarmValue[0] = KEYAPP_GetAlarmSetValue(0) - 5;
                m_MoAlarmValue[1] = KEYAPP_GetAlarmSetValue(0) + 5;
                m_MoAlarmValue[2] = KEYAPP_GetAlarmSetValue(1) - 2;
                m_MoAlarmValue[3] = KEYAPP_GetAlarmSetValue(1) + 2;
            }
        }
        
        /*氧浓度报警判断*/
        if(m_MoFiO2Value < 18)
        {
            m_MoAlarmTable.FiO2_High_Alarm = 0;
            m_MoAlarmTable.FiO2_Low_Alarm = 1;
        }
        else if(m_MoFiO2Value > 100)
        {
            m_MoAlarmTable.FiO2_High_Alarm = 1;
            m_MoAlarmTable.FiO2_Low_Alarm = 0;
        }
        else if(m_MonitorAlarmSet == 1)
        {
            if(m_MoFiO2Value < m_MoAlarmValue[0])
            {
                m_MoAlarmTable.FiO2_High_Alarm = 0;
                m_MoAlarmTable.FiO2_Low_Alarm = 1;
            }
            else if(m_MoFiO2Value > m_MoAlarmValue[1])
            {
                m_MoAlarmTable.FiO2_High_Alarm = 1;
                m_MoAlarmTable.FiO2_Low_Alarm = 0;
            }
            else
            {
                m_MoAlarmTable.FiO2_High_Alarm = 0;
                m_MoAlarmTable.FiO2_Low_Alarm = 0;
            }
        }
        else
        {
            m_MoAlarmTable.FiO2_High_Alarm = 0;
            m_MoAlarmTable.FiO2_Low_Alarm = 0;
        }
           
        /*气道压报警判断*/
        if(m_MoPawValue <= 0)
        {
            m_MoAlarmTable.Paw_High_Alarm = 0;
            m_MoAlarmTable.Paw_Low_Alarm = 1;
        }
        else if(m_MoPawValue >= 12)
        {
            /*      当气道压过高时，进行泄压        */
            POWER_SwitchSet(DEF_PRE_CONTROL, DEF_POWER_CLOSE);
            m_MoAlarmTable.Paw_High_Alarm = 1;
            m_MoAlarmTable.Paw_Low_Alarm = 0;
        }
        else if(m_MonitorAlarmSet == 1)
        {
            if(m_MoPawValue < m_MoAlarmValue[2])
            {
                m_MoAlarmTable.Paw_High_Alarm = 0;
                m_MoAlarmTable.Paw_Low_Alarm = 1;
            }
            else if(m_MoPawValue > m_MoAlarmValue[3])
            {
                /*      当气道压过高时，进行泄压        */
                POWER_SwitchSet(DEF_PRE_CONTROL, DEF_POWER_CLOSE);
                m_MoAlarmTable.Paw_High_Alarm = 1;
                m_MoAlarmTable.Paw_Low_Alarm = 0;
            }
            else
            {
                POWER_SwitchSet(DEF_PRE_CONTROL, DEF_POWER_OPEN);
                m_MoAlarmTable.Paw_High_Alarm = 0;
                m_MoAlarmTable.Paw_Low_Alarm = 0;
            }
        }
        else
        {
            m_MoAlarmTable.Paw_High_Alarm = 0;
            m_MoAlarmTable.Paw_Low_Alarm = 0;
        }
            
            
        /*静音和停止静音的监测和实行*/
        if((KEYAPP_GetAlarmSetFlag() != DEF_KEY_OFF) || ((CALIBRATION_IsAdj() != DEF_CAL_OFF)))
        {
            m_MoCount = 0;
        }
        else
        {
            INT8U key_i = KEY_Scan(); 
                
            if(key_i == m_MoStatus[m_MoCount])
            {
                m_MoCount++;
                  
                if(m_MoCount > 1)
                {
                    m_MoCount = 0;
                    m_MoAlarmTable.Sound_Disable_Alarm = !m_MoAlarmTable.Sound_Disable_Alarm;
                    if(1 == m_MoAlarmTable.Sound_Disable_Alarm)
                    {
                        m_MoAlarmTableBak = m_MoAlarmTable;
                    }
                }   
            }
        }
           
        /*      报警静音过程中，如果产生新的报警，报警静音失效，报警声音重新开启        */
        if(1 == m_MoAlarmTable.Sound_Disable_Alarm)
        {
            if( ((m_MoAlarmTable.FiO2_High_Alarm != m_MoAlarmTableBak.FiO2_High_Alarm)
                  && (1 == m_MoAlarmTable.FiO2_High_Alarm))
               || ((m_MoAlarmTable.FiO2_Low_Alarm != m_MoAlarmTableBak.FiO2_Low_Alarm)
                  && (1 == m_MoAlarmTable.FiO2_Low_Alarm))
               || ((m_MoAlarmTable.Paw_High_Alarm != m_MoAlarmTableBak.Paw_High_Alarm)
                  && (1 == m_MoAlarmTable.Paw_High_Alarm))
               || ((m_MoAlarmTable.Paw_Low_Alarm != m_MoAlarmTableBak.Paw_Low_Alarm)
                  && (1 == m_MoAlarmTable.Paw_Low_Alarm))
               || ((m_MoAlarmTable.Bat_Red_Alarm != m_MoAlarmTableBak.Bat_Red_Alarm)
                  && (1 == m_MoAlarmTable.Bat_Red_Alarm)) )
            {
                m_MoAlarmTable.Sound_Disable_Alarm = 0;
            }
        }
        ALARM_Write(m_MoAlarmTable);
    }
}
/*
********************************************************************************
*                               接口函数
********************************************************************************
*/
/*
******************************************************************************
函数名称:   MONITOR_Version
函数功能:   自检过程中，按报警静音键和校准键显示软件版本号。
输入参数:   none
输出参数:   none
返 回 值:   返回0表示没显示版本号，返回1表示显示了版本号
创建日期:   2015-09-01
注    意:   
*******************************************************************************
*/
INT8U MONITOR_Version(void)
{
    if(STARTINGUP_FiO2ErrorFlag() == DEF_STARTUP_OFF)
    {
        /*      在自检过程中显示版本号        */
        if(STARTINGUP_GetCheckFlag() == DEF_STARTUP_ON)
        {
            /*      扫描是否按了报警静音键或者校准键        */
            if(0 == m_MoKeyFlag)
            {
                if(KEY_Scan() == DEF_KEY_CODE_ALARM_SOUND)
                {
                    m_MoKeyFlag = 1;
                }
                else if(KEY_Scan() == DEF_KEY_CODE_ADJUST)
                {
                    m_MoKeyFlag = 2;
                }
            }
            
            /*      在按了报警静音键或者校准键其中之一的情况下，是否再次按了其中的另一个
                    按键。当两个按键都按了时，显示版本号。
            */
            if((1 == m_MoKeyFlag) && (KEY_Scan() == DEF_KEY_CODE_ADJUST))
            {
                OLED_Version(DEF_VERSION);
                m_MoKeyFlag = 0;
                m_MoVerFlag = 1;
            }
            else if((2 == m_MoKeyFlag) && (KEY_Scan() == DEF_KEY_CODE_ALARM_SOUND))
            {
                m_MoKeyFlag = 0;
                OLED_Version(DEF_VERSION);
                m_MoVerFlag = 1;
            }
        }
    }
    
    return m_MoVerFlag;
}
/*
******************************************************************************
函数名称:   MONITOR_Exec
函数功能:   监测任务执行函数
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-09-03
注    意:   none
*******************************************************************************
*/
void MONITOR_Exec(void)
{
    /*      自检过程中显示版本号      */
    MONITOR_Version();
     
    /*      喂狗      */
    FeedWatchDog();
    
    /*      电源监测        */
    MONITOR_Voltage();
    
    /*      报警监测        */
    MONITOR_Alarm();    
}
/*
******************************************************************************
函数名称:   MONITOR_GetFiO2Value
函数功能:   获取氧浓度值
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-10-14
注    意:   none
*******************************************************************************
*/
INT8U MONITOR_GetFiO2Value(void)
{
    return m_MoFiO2Value;
}
/*
******************************************************************************
函数名称:   MONITOR_GetPawValue
函数功能:   获取气道压值
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-10-14
注    意:   none
*******************************************************************************
*/
INT8U MONITOR_GetPawValue(void)
{    
    return m_MoPawValue;
}
/*
******************************************************************************
函数名称:   MONITOR_SetSoundDisable
函数功能:   设置报警静音的状态为关闭报警静音
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-09-03
注    意:   none
*******************************************************************************
*/
void MONITOR_SetSoundDisable(void)
{
    m_MoAlarmTable.Sound_Disable_Alarm = 0;
}