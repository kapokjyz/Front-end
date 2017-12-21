/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: ShartingUp.c
* ժ    Ҫ: ��Դ����ģ�����
*
* ��ǰ�汾: 1.0
* ��    ��: ����
* �������: 2015-09-01
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
#include "ShutDown.h"
#include "StartingUp.h"
#include "Alarm.h"
#include "Key.h"
#include "DataSamp.h"

/*
********************************************************************************
*                               �궨��
********************************************************************************
*/
/*		�����ջ��С		*/
#define TASK_STK_SIZE				256u

/*      �������ȼ�       */
#define DEF_ALARM_STK_PRIO              10u

/*      ��ѯʱ��        */
#define DEF_ALARM_STK_DELAY_TIME              100u
/*
********************************************************************************
*                               �ڲ�����
********************************************************************************
*/
/*      ���������ջ      */
static OS_STK         m_StAlarmStk[TASK_STK_SIZE];
/*      ������ɱ�־,1�ǿ�����ɣ�0����δ���        */
static BOOLEAN        m_StStaringFlag = DEF_STARTUP_OFF;
/*      ����У׼����      */
static INT8U          m_StBufferCalibration[30] = {0};
/*      �����Լ��־��1���Լ쿪����0���Լ����      */
static BOOLEAN        m_StStaringUpCheckFlag = DEF_STARTUP_OFF;
/*      �Լ��������ش����ʾ     */
static BOOLEAN        m_StFiO2ErrorFlag = DEF_STARTUP_OFF;
/*
********************************************************************************
********************************************************************************
*                               �ڲ�����
********************************************************************************
********************************************************************************
*/
static void STARTINGUP_AlarmTask(void *p_arg);
static void STARTINTUP_Check(void);
/*
******************************************************************************
��������:   STARTINGUP_Check
��������:   �����Լ캯��
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-09-01
ע    ��:   
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
        
        /*      ���ָʾ����˸     */
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
        
        /*      ��������������������������������        */
        LED_Set(DEF_LED_ADAPT,DEF_LED_OPEN);
        LED_Set(DEF_LED_HIGH_FIO2,DEF_LED_OPEN);
        LED_Set(DEF_LED_LOW_FIO2,DEF_LED_OPEN);
        LED_Set(DEF_LED_ALARM_SOUND,DEF_LED_OPEN);
               
        OSTimeDlyHMSM(0, 0, 0, 100);      
        
        /*      ����ѹLED��ѭ����˸      */
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
    /*      ������������������������ء�������������ѹ��ѹLED����        */
    LED_Set(DEF_LED_HIGH_FIO2,DEF_LED_CLOSE);
    LED_Set(DEF_LED_LOW_FIO2,DEF_LED_CLOSE);
    LED_Set(DEF_LED_ALARM_SOUND,DEF_LED_CLOSE);
    LED_Set(DEF_LED_BATERRY_RED,DEF_LED_CLOSE);
    LED_Set(DEF_LED_ADAPT,DEF_LED_CLOSE);
    PAW_LedSetOne(14, DEF_PAW_LED_OFF);
    
    /*      �����Ũ�ȵ��     */
    if(DATASAMP_GetVoltage(ENUM_O2) < 0.4)
    {
        /*      ��ʾ������ʾ��Ϣ001     */
        OLED_Display(1);
        m_StFiO2ErrorFlag = DEF_STARTUP_ON;
    }
}
/*
********************************************************************************
********************************************************************************
*                               �ӿں���
********************************************************************************
********************************************************************************
*/
/*
******************************************************************************
��������:   STARTINGUP_Exec
��������:   ����ִ�к���
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-09-01
ע    ��:   none
*******************************************************************************
*/
void STARTINGUP_Exec(void)
{
    /*      ���ȴ��������İ���      */
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
    
    /*      ��ͨ��·��Դ           */
    POWER_SwitchSet(DEF_EN_DC, DEF_POWER_OPEN);
    POWER_SwitchSet(DEF_DC_DC, DEF_POWER_OPEN);
    POWER_SwitchSet(DEF_ON_STABLE, DEF_POWER_OPEN);
    POWER_SwitchSet(DEF_PRE_CONTROL, DEF_POWER_OPEN);
    OSTimeDlyHMSM(0, 0, 0, 50); 
    
    /*		������������		*/
	OSTaskCreate(STARTINGUP_AlarmTask, NULL, (OS_STK *)&m_StAlarmStk[TASK_STK_SIZE-1], (INT8U)DEF_ALARM_STK_PRIO);
    
    /*      �Լ��־��1���÷�������������     */
    m_StStaringUpCheckFlag = DEF_STARTUP_ON;
    OSTimeDlyHMSM(0, 0, 0, 50);
    
    /*      �����Լ�        */
    STARTINTUP_Check();
    
    /*      �Լ����֮����0���رշ���������        */
    m_StStaringUpCheckFlag = DEF_STARTUP_OFF;

    /*      ��ȡEEPROM�е�У׼�Ĳ���ֵ        */
    I2C_EE_BufferRead(EEPROM_DEVICE_ADDRESS,m_StBufferCalibration, 0, 30);
    
    /*      ������ɱ�־      */
    m_StStaringFlag = DEF_STARTUP_ON;
}
/*
******************************************************************************
��������:	STARTINGUP_AlarmTask
��������:   ��������
�������:	png���������������ָ��
�������:	none
�� �� ֵ:   none
��������:   2015-09-01
ע    ��:    
*******************************************************************************
*/
static void STARTINGUP_AlarmTask(void *p_arg)
{
    p_arg = p_arg;

    ALARM_Init();
    
	for(;;)
	{
        /*����ִ��ģ��*/
        ALARM_Exec();          
        OSTimeDlyHMSM(0, 0, 0, DEF_ALARM_STK_DELAY_TIME);   
    }
}
/*
******************************************************************************
��������:   STARTINGUP_GetFlag
��������:   ��ȡ������־
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-09-01
ע    ��:   none
*******************************************************************************
*/
INT8U STARTINGUP_GetFlag(void)
{
    return m_StStaringFlag;
}
/*
******************************************************************************
��������:   STARTINGUP_GetcCalibValue
��������:   ���У׼������ֵ��
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-09-02
ע    ��:   none
*******************************************************************************
*/
INT8U STARTINGUP_GetCalibValue(INT16U num)
{
    return m_StBufferCalibration[num];
}
/*
******************************************************************************
��������:   STARTINGUP_GetCheckFlag
��������:   ����Լ��־��ֵ��
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-09-02
ע    ��:   none
*******************************************************************************
*/
INT8U STARTINGUP_GetCheckFlag(void)
{
    return m_StStaringUpCheckFlag;
}
/*
******************************************************************************
��������:   STARTINGUP_FiO2ErrorFlag
��������:   ����Լ��־��ֵ��
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-10-19
ע    ��:   none
*******************************************************************************
*/
BOOLEAN STARTINGUP_FiO2ErrorFlag(void)
{
    return m_StFiO2ErrorFlag;
}