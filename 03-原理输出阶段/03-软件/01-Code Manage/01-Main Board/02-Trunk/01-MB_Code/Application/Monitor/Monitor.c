/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: Monitoring.c
* ժ    Ҫ: ���ģ�����
*
* ��ǰ�汾: 1.0
* ��    ��: ����
* �������: 2015-9-03
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
*                               �궨��
********************************************************************************
*/
/*       �����                          */
#define          DEF_CHG_CHECK_PORT          (GPIOA)
#define          DEF_CHG_CHECK_PIN           (GPIO_Pin_3)

#define          DEF_BAT_CHARGE               0
#define          DEF_BAT_NOT_CHARGE           1
#define          DEF_BAT_NORMAL               0
#define          DEF_BAT_LOW                  1
#define          DEF_BAT_TOOLOW               1
 
/*      �汾��     */
#define          DEF_VERSION                  1507
/*
********************************************************************************
*                               ���Ͷ���
********************************************************************************
*/
/*
********************************************************************************
*                               �ڲ�����
********************************************************************************
*/ 
/*      ���Ƶ��ָʾ����˸       */
static BOOLEAN  m_MoOnOff = 0;
/*      ���汨�����ݵı�����      */
static ALARM_Table m_MoAlarmTable = {0, 0, 0, 0, 0, 0};
/*      ���ݱ�������ʱ�ı������        */
static ALARM_Table m_MoAlarmTableBak = {0, 0, 0, 0, 0, 0};
/*      ������״̬��       */
static INT8U  m_MoStatus[] = {'c', 0};
/*      ����״̬�������±�       */
static INT32U m_MoCount = 0;
/*      �������        */
static INT32U m_MoTicked = 0;
/*      ������Ũ�Ⱥ�����ѹ�ı���������ֵ        */
static INT8U  m_MoAlarmValue[4] = {0};
/*      �����Ƿ��趨��������ֵ     */
static INT8U  m_MonitorAlarmSet = 0;
/*      ��¼��ص�������͵�����־       */
static INT8U  m_MoBatLowFlag = DEF_BAT_NORMAL;
/*      ������Ũ��ֵ      */
static INT8U  m_MoFiO2Value = 0;
/*      ��������ѹֵ      */
static INT8U  m_MoPawValue = 0;
/*      ���水����־      */
static INT8U m_MoKeyFlag = 0;
/*      ������ʾ�汾�ű�־       */
static INT8U m_MoVerFlag = 0;
/*      ��ص�ѹ���ͱ�־        */
static INT8U m_MoBatTooLow = 0;
/*
********************************************************************************
*                               �ڲ�����
********************************************************************************
*/
/*
********************************************************************************
* ��������: MONITOR_ChargeBAT
* ��������: ����س��״̬
* ��ڲ���: gpio_pin
* ���ڲ���: void
* �� �� ֵ: 1,  �����      0, ���ڳ��
* ��    ��: 2015-09-03
* �޸�����:
* �޸�����:
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
��������:   MONITOR_Voltage
��������:   ����Դ��ѹ��
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-10-20
ע    ��:   
*******************************************************************************
*/
static void MONITOR_Voltage(void)
{
    INT8U ChargeFlag = 0;
    FP32 ADAPTOR_Voltage = 0.0;
    FP32 BAT_Voltage = 0.0;
    
    ADAPTOR_Voltage = DATASAMP_GetVoltage(ENUM_ADAPTOR);
    BAT_Voltage = DATASAMP_GetVoltage(ENUM_BAT);
    
    /*����һ��ʼ���ݲɼ��Ļ�û�ȶ�ʱ����ȡ���Ľ�С�ĵ�ص�ѹֵ�������ػ�*/
    if(m_MoTicked < 6)
    {
        m_MoTicked++;
    }
    else
    {  
        /*      �����Լ�ʱ�������͵��ָʾ�Ƶ�Ӱ��       */
        if(STARTINGUP_GetCheckFlag() == DEF_STARTUP_OFF)
        {
            /*��������ѹ����4V����Ϊ����������*/
            if(ADAPTOR_Voltage >= 4.0)
            {
                LED_Set(DEF_LED_ADAPT, DEF_LED_OPEN);
                
                /*�����λ���磬���򲻳��*/
                if(BAT_Voltage >=  2.0)
                {
                    m_MoAlarmTable.Bat_Red_Alarm = 0;
                    m_MoBatLowFlag = DEF_BAT_NORMAL;
                    /*ʹ�ܳ��*/
                    POWER_SwitchSet(DEF_EN_CHG, DEF_POWER_OPEN);
                    /*����س��״̬*/
                    ChargeFlag = MONITOR_ChargeBAT(DEF_CHG_CHECK_PIN);
                    
                    /*������ڳ��*/
                    if(DEF_BAT_CHARGE == ChargeFlag) 
                    {
                        LED_Set(DEF_LED_BATERRY_RED, DEF_LED_CLOSE);

                        m_MoOnOff = !m_MoOnOff;

                        LED_Set(DEF_LED_BATERRY_GREEN, m_MoOnOff);
                        OSTimeDlyHMSM(0,0,0,150) ;  
                    }
                    else /*��س���*/
                    {
                        
                        LED_Set(DEF_LED_BATERRY_GREEN, DEF_LED_OPEN);
                        LED_Set(DEF_LED_BATERRY_RED, DEF_LED_CLOSE);
                    }      
                }
                /*      ��ز���λ����ѵ��ָʾ�����      by zj 20150930*/
                else
                {
                    LED_Set(DEF_LED_BATERRY_GREEN, DEF_LED_CLOSE);
                    LED_Set(DEF_LED_BATERRY_RED, DEF_LED_CLOSE);
                }
            }        
            else
            {
                LED_Set(DEF_LED_ADAPT,DEF_LED_CLOSE);
                /*��ص�������ʱ���ж��Ƿ��ǵ͵���*/
                if((BAT_Voltage >=  3.6) && (BAT_Voltage <= 3.75))
                {
                    m_MoBatTooLow = 0;
                    m_MoBatLowFlag = DEF_BAT_LOW;                   
                }
                
                /*С��3.65������Ϊ��ز����ڣ��Զ��ػ�*/
                else if(BAT_Voltage < 3.6)
                { 
                    /*      ������⵽��ص�ѹ���ͣ��ٹػ�����ֹ��ѹ����      */
                    m_MoBatTooLow++;
                    if(m_MoBatTooLow > 10)
                    {
                        POWER_SwitchSet(DEF_EN_DC, DEF_POWER_CLOSE);
                        NVIC_GenerateSystemReset();
                    }
                }
                else
                {
                    /*      ����ص�������ʱ���ڵ�ص�ѹ�ոս���3.75V����ʱ�������ѹ�������ݳ��ָ���3.75Vʱ
                            ���ֺ��̵ƽ�����˸�������
                    */
                    if(DEF_BAT_NORMAL == m_MoBatLowFlag)
                    {                        
                        /*      �ڵ�ص�������ʱ������ػ��͹رյ��ָʾ��       */
                        if(DEF_SHUTDOWN_ON == SHUTDOWN_Is())
                        {
                           LED_Set(DEF_LED_BATERRY_GREEN, DEF_LED_CLOSE); 
                        }
                        else
                        {
                            /*      �ڵ�ص�������ʱ��������̵Ƴ���       */
                            LED_Set(DEF_LED_BATERRY_RED, DEF_LED_CLOSE);
                            LED_Set(DEF_LED_BATERRY_GREEN, DEF_LED_OPEN);
                        }
                    }
                }
                
                /*       ����ص�������ʱ���ڵ�ص�ѹ�ոս���3.75V����ʱ��Ϊ�����ѹ�������ݳ��ָ���3.75Vʱ
                         ���ֺ����������ϳ���ʱ�䲻���̶�Ƶ�������������ڼ�⵽��ص�ѹ����ʱ���ʵ�
                         �����ص͵�ѹ��Χ�����ر��Ƿ�������ͣʱ����ص�ѹ��������Բ�����  
                */
                if( (DEF_BAT_LOW == m_MoBatLowFlag) && (DEF_SHUTDOWN_OFF == SHUTDOWN_Is()) )
                {
                    m_MoAlarmTable.Bat_Red_Alarm = 1;
                     /*      �͵���ʱ���̵��𣬺����˸       */
                    LED_Set(DEF_LED_BATERRY_GREEN, DEF_LED_CLOSE);

                    m_MoOnOff = !m_MoOnOff;
                        
                    LED_Set(DEF_LED_BATERRY_RED, m_MoOnOff);
                    OSTimeDlyHMSM(0,0,0,150);
                }
                else
                {
                    /*      ��ص������磬�ػ������У���������3��û����͹رյ��ָʾ��      */
                    LED_Set(DEF_LED_BATERRY_RED, DEF_LED_CLOSE);
                }
            }
        }
        ALARM_Write(m_MoAlarmTable);
    }
}
/*
******************************************************************************
��������:   MONITOR_Alarm
��������:   ��ⱨ�������
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-10-20
ע    ��:   
*******************************************************************************
*/
static void MONITOR_Alarm(void)
{
    INT16U Ad_Paw = 0;
    INT16U Ad_FiO2 = 0;
    
    /*��ȡADֵ*/
    Ad_Paw = DATASAMP_AdValue(ENUM_PAW);
    Ad_FiO2 = DATASAMP_AdValue(ENUM_O2);
    
    /*����ʵ��ֵ*/
    m_MoPawValue = DATASAMP_AppCalPaw(Ad_Paw);
    m_MoFiO2Value = DATASAMP_AppCalO2(Ad_FiO2);
    
    if((DEF_STARTUP_ON == STARTINGUP_GetFlag()) && (DEF_SHUTDOWN_OFF == SHUTDOWN_Is()))
    {  
        /*���ñ�����ֵ�ɹ�ʱ����ȡ����������ֵ*/
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
        
        /*��Ũ�ȱ����ж�*/
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
           
        /*����ѹ�����ж�*/
        if(m_MoPawValue <= 0)
        {
            m_MoAlarmTable.Paw_High_Alarm = 0;
            m_MoAlarmTable.Paw_Low_Alarm = 1;
        }
        else if(m_MoPawValue >= 12)
        {
            /*      ������ѹ����ʱ������йѹ        */
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
                /*      ������ѹ����ʱ������йѹ        */
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
            
            
        /*������ֹͣ�����ļ���ʵ��*/
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
           
        /*      �������������У���������µı�������������ʧЧ�������������¿���        */
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
*                               �ӿں���
********************************************************************************
*/
/*
******************************************************************************
��������:   MONITOR_Version
��������:   �Լ�����У���������������У׼����ʾ����汾�š�
�������:   none
�������:   none
�� �� ֵ:   ����0��ʾû��ʾ�汾�ţ�����1��ʾ��ʾ�˰汾��
��������:   2015-09-01
ע    ��:   
*******************************************************************************
*/
INT8U MONITOR_Version(void)
{
    if(STARTINGUP_FiO2ErrorFlag() == DEF_STARTUP_OFF)
    {
        /*      ���Լ��������ʾ�汾��        */
        if(STARTINGUP_GetCheckFlag() == DEF_STARTUP_ON)
        {
            /*      ɨ���Ƿ��˱�������������У׼��        */
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
            
            /*      �ڰ��˱�������������У׼������֮һ������£��Ƿ��ٴΰ������е���һ��
                    ����������������������ʱ����ʾ�汾�š�
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
��������:   MONITOR_Exec
��������:   �������ִ�к���
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-09-03
ע    ��:   none
*******************************************************************************
*/
void MONITOR_Exec(void)
{
    /*      �Լ��������ʾ�汾��      */
    MONITOR_Version();
     
    /*      ι��      */
    FeedWatchDog();
    
    /*      ��Դ���        */
    MONITOR_Voltage();
    
    /*      �������        */
    MONITOR_Alarm();    
}
/*
******************************************************************************
��������:   MONITOR_GetFiO2Value
��������:   ��ȡ��Ũ��ֵ
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-10-14
ע    ��:   none
*******************************************************************************
*/
INT8U MONITOR_GetFiO2Value(void)
{
    return m_MoFiO2Value;
}
/*
******************************************************************************
��������:   MONITOR_GetPawValue
��������:   ��ȡ����ѹֵ
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-10-14
ע    ��:   none
*******************************************************************************
*/
INT8U MONITOR_GetPawValue(void)
{    
    return m_MoPawValue;
}
/*
******************************************************************************
��������:   MONITOR_SetSoundDisable
��������:   ���ñ���������״̬Ϊ�رձ�������
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-09-03
ע    ��:   none
*******************************************************************************
*/
void MONITOR_SetSoundDisable(void)
{
    m_MoAlarmTable.Sound_Disable_Alarm = 0;
}