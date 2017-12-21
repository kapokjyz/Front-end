/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: Calibration.c
* ժ    Ҫ: У׼ģ�����
*
* ��ǰ�汾: 1.0
* ��    ��: ����
* �������: 2015-09-18
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
#include "Calibration.h"
#include "KeyApp.h"
#include "DataSamp.h"
#include "StartingUp.h"
/*
********************************************************************************
*                               �궨��
********************************************************************************
*/
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
/*      ���񰴼��Ĵ���     */
static INT32U m_CaKeyCount = 0;
/*      У׼�����а�У׼���Ĵ���       */
static INT32U m_CaKeyAdjNum = 0;
/*      У׼��־        */
static BOOLEAN  m_CaAdjFlag = DEF_CAL_OFF;
/*      ����У׼ģʽ��־        */
static BOOLEAN  m_CaAdjMode = DEF_CAL_OFF;
/*      ����У׼�����л�ȡ����ADֵ      */
static INT8U  m_CaTempVoltFiO2[4] = {0};
static INT8U  m_CaTempVoltPAW[26] = {0};
/*      ����״̬�������±�      */
static INT8U m_CaCount = 0;
/*      ����      */
static INT32U m_CaTicked = 0;
/*      ����״̬��       */
static INT8U  m_CaStatus[4] = {'b', 0, 'b', 0};
/*      �����ɹ���־      */
static INT8U  m_CalibrationSucFlag = DEF_CAL_OFF;
/*
********************************************************************************
*                               ���ܺ���
********************************************************************************
*/
/*
********************************************************************************
********************************************************************************
*                               �ӿں���
********************************************************************************
********************************************************************************
*/
/*
******************************************************************************
��������:   CALIBRATION_FiO2
��������:   У׼��Ũ��
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-09-29
ע    ��:   none
*******************************************************************************
*/
static void CALIBRATION_FiO2(void)
{
    /*У׼���� ��Ũ��*/
    if(m_CaAdjFlag == DEF_CAL_FIO2)
    {
        if(KEY_Scan() == m_CaStatus[0])
        {   
            m_CaKeyAdjNum++;
            if(m_CaKeyAdjNum == 1)
            {
                m_CaTempVoltFiO2[0] = (DATASAMP_AdValue(ENUM_O2)>>8) & 0x00ff;
                m_CaTempVoltFiO2[1] = DATASAMP_AdValue(ENUM_O2) & 0x00ff;	
                OLED_Display(100);
            }
            if(m_CaKeyAdjNum == 2)
            {
                m_CaTempVoltFiO2[2] = (DATASAMP_AdValue(ENUM_O2)>>8) & 0x00ff;
                m_CaTempVoltFiO2[3] = DATASAMP_AdValue(ENUM_O2) & 0x00ff;
                I2C_EE_BufferWrite(EEPROM_DEVICE_ADDRESS, &m_CaTempVoltFiO2[0],0, 4);
                OLED_Display(888);
            }
        }      
        
        if(m_CaCount > 0)
        {
            if(m_CaTicked > 9)
            {    
                m_CaCount = 0;
                m_CaTicked = 0;
            }
                    
            m_CaTicked++;
        }
         
        if(KEY_Scan() == m_CaStatus[m_CaCount])
        {
            m_CaCount++;
            if(m_CaCount > 3 )
            {
                /*      ˫���˳���Ũ��У׼      */		  
                m_CaCount = 0;
                m_CaAdjFlag = DEF_CAL_OFF;
                m_CaTicked = 0;
                m_CaKeyAdjNum = 0;
                m_CaAdjMode = DEF_CAL_OFF;
                m_CalibrationSucFlag = DEF_CAL_ON;
            }
        }        
    }
}
/*
******************************************************************************
��������:   CALIBRATION_PAW
��������:   У׼����ѹ
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-09-29
ע    ��:   none
*******************************************************************************
*/
static void CALIBRATION_PAW(void)
{
    /*У׼���� ����ѹ*/
    if(m_CaAdjFlag == DEF_CAL_PAW)
    {
        if(KEY_Scan() == m_CaStatus[m_CaCount])
        {   
            m_CaCount++;
            
            if(m_CaCount == 4)
            {
                m_CaCount = 0;
            }
            if(m_CaCount % 2 == 0)
            {
                m_CaKeyAdjNum++;
                
                if(m_CaKeyAdjNum < 13)
                {
                    m_CaTempVoltPAW[(m_CaKeyAdjNum-1)*2] = (DATASAMP_AdValue(ENUM_PAW)>>8) & 0x00ff;
                    m_CaTempVoltPAW[(m_CaKeyAdjNum-1)*2+1] = DATASAMP_AdValue(ENUM_PAW) & 0x00ff;	
                    PAW_LedSet(m_CaKeyAdjNum);
                    
                }
                else if(m_CaKeyAdjNum == 13)
                {
                    m_CaTempVoltPAW[(m_CaKeyAdjNum-1)*2] = (DATASAMP_AdValue(ENUM_PAW)>>8) & 0x00ff;
                    m_CaTempVoltPAW[(m_CaKeyAdjNum-1)*2+1] = DATASAMP_AdValue(ENUM_PAW) & 0x00ff;	
                    I2C_EE_BufferWrite(EEPROM_DEVICE_ADDRESS, &m_CaTempVoltPAW[0],4, 26);
                    OLED_Display(888);
                }
            }
        }
        
        /*�����˳�����ѹУ׼*/
        if(KEY_Scan() == m_CaStatus[0])
        {
            m_CaKeyCount++;
        }
        else if(KEY_Scan() == m_CaStatus[1])
        {
            m_CaKeyCount = 0;
        }
        
        if(m_CaKeyCount > 19)
        {
            m_CaKeyAdjNum = 0;
            m_CaKeyCount = 0;
            m_CaAdjFlag = DEF_CAL_OFF;
            m_CaAdjMode = DEF_CAL_OFF;
            m_CaCount = 0;
            m_CalibrationSucFlag = DEF_CAL_ON;
        }   
    }
}
/*
******************************************************************************
��������:   CALIBRATION_Exec
��������:   У׼����ִ�к���
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-09-15
ע    ��:   none
*******************************************************************************
*/
void CALIBRATION_Exec(void)
{ 
    /*      ��ǰ������У׼ģʽ���������ñ����趨ֵ�����У���������ش���֮��������������½���У׼        */
    if((0 == m_CaAdjFlag) && (KEYAPP_GetAlarmSetFlag() == 0) && (STARTINGUP_FiO2ErrorFlag() == 0))
    {
        /*      ˫��������Ũ��У׼      */
        if(m_CaCount > 0)
        {
            if(m_CaTicked > 19)
            {    
              m_CaCount = 0;
              m_CaTicked = 0;
            }
            
            m_CaTicked++;
        }
         
        if(KEY_Scan() == m_CaStatus[m_CaCount])
        {
            m_CaCount++;
            if(m_CaCount > 3 )
            {            
                m_CaAdjMode = DEF_CAL_ON;
                m_CaAdjFlag = DEF_CAL_FIO2;
                m_CaCount = 0;
                m_CaTicked = 0;
                OSTimeDlyHMSM(0, 0, 0, 100);
                OLED_Display(21);
            }
        }
        
        /*      ����3���������ѹУ׼        */
        if(m_CaAdjMode == DEF_CAL_OFF)
        {
            if(KEY_Scan() == m_CaStatus[0])
            {
                m_CaKeyCount++;
            }
            if(KEY_Scan() == m_CaStatus[1])
            {
                m_CaKeyCount = 0;
            }
        }
        
        if(m_CaKeyCount > 9)
        {
            m_CaAdjMode = DEF_CAL_ON;
            PAW_LedSet(0);
            if(KEYAPP_Takeoff(DEF_KEY_CODE_ADJUST) == 1)
            {
                m_CaKeyCount = 0;
                m_CaAdjFlag = DEF_CAL_PAW;
            }
        }
    }
    
    /*      ��Ũ��У׼����     */
    CALIBRATION_FiO2();
    /*      ����ѹУ׼����     */
    CALIBRATION_PAW();
}
/*
******************************************************************************
��������:   CALIBRATION_IsAdj
��������:   ��ȡУ׼״̬
�������:   none
�������:   none
�� �� ֵ:   0����У׼      1��У׼
��������:   2015-09-15
ע    ��:   none
*******************************************************************************
*/
INT8U CALIBRATION_IsAdj(void)
{
    return m_CaAdjMode;
}
/*
******************************************************************************
��������:   CALIBRATION_GetCalSucflag
��������:   ��ȡУ׼�ɹ���־
�������:   none
�������:   none
�� �� ֵ:   none
��������:   2015-09-18
ע    ��:   none
*******************************************************************************
*/
INT8U CALIBRATION_GetCalSucflag(void)
{   
    INT8U flag = 0;
    
    flag = m_CalibrationSucFlag;
    m_CalibrationSucFlag = DEF_CAL_OFF;
    
    return flag;    
}