/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: KeyApp.c
* ժ    Ҫ: ����Ӧ��ģ�����
*
* ��ǰ�汾: 1.0
* ��    ��: ����
* �������: 2015-08-28
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
#include "KeyApp.h"
#include "Alarm.h"
#include "Display.h"
#include "Calibration.h"
#include "StartingUp.h"
/*
********************************************************************************
*                               ���Ͷ���
********************************************************************************
*/
/*
********************************************************************************
*                                       �ڲ�����
********************************************************************************
*/
/*      �������       */
static INT32U m_KeCount = 0;
/*      �������ñ�־      */
static BOOLEAN  m_KeAlarmSetFlag = DEF_KEY_OFF;
/*      �������óɹ���־       */
static BOOLEAN  m_KeAlarmSetSucFlag = DEF_KEY_OFF;
/*      ���汨������ֵ     */
static INT8U  m_KeAlarmValue[2] = {0};
/*
********************************************************************************
********************************************************************************
*                               �ӿں���
********************************************************************************
********************************************************************************
*/
/*
********************************************************************************
* ��������: KEYAPP_Exec
* ��������: ��������ִ�к���
* ��ڲ���: void
* ���ڲ���: void
* �� �� ֵ: void
* ��    ��: 2015-08-28
* �޸�����:
* �޸�����:
********************************************************************************
*/
void KEYAPP_Exec(void)
{
   OSTimeDlyHMSM(0,0,0,100);
   /*       �Ǿ���״̬���������ʱ���ܲ����˹���     */
   if(0 == ALARM_GetIsDisableSound() && (STARTINGUP_FiO2ErrorFlag() == 0))
   {
       /*       �������뱨������        */
       if((KEY_Scan() == DEF_KEY_CODE_ALARM_SOUND) && (CALIBRATION_IsAdj() == 0)) 
       {
           m_KeCount++;
           /*    ����ʱ�䵽�ˣ������ñ������ñ�־Ϊ1    */
           if(m_KeCount > 9)
           {
               m_KeAlarmSetFlag = DEF_KEY_ON;
               LED_Set(DEF_LED_ALARM_SOUND,DEF_LED_OPEN);              
           }
       }
       else
       {
            /*      ����ʱ�䵽�����ɿ���      */
           if(m_KeCount > 9)
           {
               m_KeAlarmSetFlag  = DEF_KEY_OFF;
               m_KeAlarmValue[0] = DISPLAY_GetFiO2();
               m_KeAlarmValue[1] = DISPLAY_GetPaw();
               LED_Set(DEF_LED_ALARM_SOUND,DEF_LED_CLOSE);
               m_KeAlarmSetSucFlag = DEF_KEY_ON;
           }
           m_KeCount = 0;
       }
   }   
}
/*
********************************************************************************
* ��������: KEYAPP_GetAlarmSetFlag
* ��������: ��ñ������ñ�־
* ��ڲ���: void
* ���ڲ���: void
* �� �� ֵ: void
* ��    ��: 2015-08-28
* �޸�����:
* �޸�����:
********************************************************************************
*/
INT8U KEYAPP_GetAlarmSetFlag(void)
{
    return m_KeAlarmSetFlag;
}
/*
********************************************************************************
* ��������: KEYAPP_Takeoff
* ��������: ��ⰴ���Ƿ��ɿ�
* ��ڲ���: void
* ���ڲ���: void
* �� �� ֵ: 1,�ɿ���0��û�ɿ�
* ��    ��: 2015-08-28
* �޸�����:
* �޸�����:
********************************************************************************
*/
INT8U KEYAPP_Takeoff(INT8U key_type)
{
    INT8U Key_flag = DEF_KEY_TAKEOFF;
    
    switch(key_type)
    {
    /*      ��Դ��        */
	case  DEF_KEY_CODE_PPOWER_SWITCH:
    {
       /*       ��ⰴ���Ƿ��ɿ�        */ 
       if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)==1)  
       {
            Key_flag = DEF_KEY_TAKEON;
       }
       else 
       {
            Key_flag=DEF_KEY_TAKEOFF; 
       }
    }          
	break;	
    
    /*      У׼��        */
 	case  DEF_KEY_CODE_ADJUST:
    {
       if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)==1)  
       {
            Key_flag = DEF_KEY_TAKEON;
       }
       else 
       {
            Key_flag=DEF_KEY_TAKEOFF; 
       }
    }          
	break;	
    
    /*      ����������        */
	case  DEF_KEY_CODE_ALARM_SOUND:
	{
       if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)==1)  
       {
            Key_flag = DEF_KEY_TAKEON;
       }
       else 
       {
            Key_flag=DEF_KEY_TAKEOFF; 
       }
    } 
		   
	default:
		break;
	}
    
	return Key_flag;
}
/*
********************************************************************************
* ��������: KEYAPP_GetAlarmSetValue
* ��������: ��ñ�����������ֵ
* ��ڲ���: void
* ���ڲ���: void
* �� �� ֵ: void
* ��    ��: 2015-08-28
* �޸�����:
* �޸�����:
********************************************************************************
*/
INT8U KEYAPP_GetAlarmSetValue(INT16U num)
{
    return m_KeAlarmValue[num];
}
/*
********************************************************************************
* ��������: KEYAPP_GetAlarmSetSucflag
* ��������: ��ñ�����������ֵ���óɹ���־
* ��ڲ���: void
* ���ڲ���: void
* �� �� ֵ: void
* ��    ��: 2015-08-28
* �޸�����:
* �޸�����:
********************************************************************************
*/
INT8U KEYAPP_GetAlarmSetSucflag(void)
{
    INT8U flag = 0;
    
    flag = m_KeAlarmSetSucFlag;
    m_KeAlarmSetSucFlag = DEF_KEY_OFF;
    
    return flag;
}