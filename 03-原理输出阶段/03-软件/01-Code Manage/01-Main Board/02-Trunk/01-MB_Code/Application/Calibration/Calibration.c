/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: Calibration.c
* 摘    要: 校准模块程序
*
* 当前版本: 1.0
* 作    者: 曾健
* 完成日期: 2015-09-18
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
#include "Calibration.h"
#include "KeyApp.h"
#include "DataSamp.h"
#include "StartingUp.h"
/*
********************************************************************************
*                               宏定义
********************************************************************************
*/
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
/*      捕获按键的次数     */
static INT32U m_CaKeyCount = 0;
/*      校准过程中按校准键的次数       */
static INT32U m_CaKeyAdjNum = 0;
/*      校准标志        */
static BOOLEAN  m_CaAdjFlag = DEF_CAL_OFF;
/*      进入校准模式标志        */
static BOOLEAN  m_CaAdjMode = DEF_CAL_OFF;
/*      保存校准过程中获取到的AD值      */
static INT8U  m_CaTempVoltFiO2[4] = {0};
static INT8U  m_CaTempVoltPAW[26] = {0};
/*      报警状态表数组下标      */
static INT8U m_CaCount = 0;
/*      计数      */
static INT32U m_CaTicked = 0;
/*      报警状态表       */
static INT8U  m_CaStatus[4] = {'b', 0, 'b', 0};
/*      报警成功标志      */
static INT8U  m_CalibrationSucFlag = DEF_CAL_OFF;
/*
********************************************************************************
*                               功能函数
********************************************************************************
*/
/*
********************************************************************************
********************************************************************************
*                               接口函数
********************************************************************************
********************************************************************************
*/
/*
******************************************************************************
函数名称:   CALIBRATION_FiO2
函数功能:   校准氧浓度
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-09-29
注    意:   none
*******************************************************************************
*/
static void CALIBRATION_FiO2(void)
{
    /*校准过程 氧浓度*/
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
                /*      双击退出氧浓度校准      */		  
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
函数名称:   CALIBRATION_PAW
函数功能:   校准气道压
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-09-29
注    意:   none
*******************************************************************************
*/
static void CALIBRATION_PAW(void)
{
    /*校准过程 气道压*/
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
        
        /*长按退出气道压校准*/
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
函数名称:   CALIBRATION_Exec
函数功能:   校准任务执行函数
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-09-15
注    意:   none
*******************************************************************************
*/
void CALIBRATION_Exec(void)
{ 
    /*      当前不处在校准模式，不在设置报警设定值过程中，不在氧电池错误之后，在这三种情况下进入校准        */
    if((0 == m_CaAdjFlag) && (KEYAPP_GetAlarmSetFlag() == 0) && (STARTINGUP_FiO2ErrorFlag() == 0))
    {
        /*      双击进入氧浓度校准      */
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
        
        /*      长按3秒进入气道压校准        */
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
    
    /*      氧浓度校准过程     */
    CALIBRATION_FiO2();
    /*      气道压校准过程     */
    CALIBRATION_PAW();
}
/*
******************************************************************************
函数名称:   CALIBRATION_IsAdj
函数功能:   获取校准状态
输入参数:   none
输出参数:   none
返 回 值:   0，非校准      1，校准
创建日期:   2015-09-15
注    意:   none
*******************************************************************************
*/
INT8U CALIBRATION_IsAdj(void)
{
    return m_CaAdjMode;
}
/*
******************************************************************************
函数名称:   CALIBRATION_GetCalSucflag
函数功能:   获取校准成功标志
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-09-18
注    意:   none
*******************************************************************************
*/
INT8U CALIBRATION_GetCalSucflag(void)
{   
    INT8U flag = 0;
    
    flag = m_CalibrationSucFlag;
    m_CalibrationSucFlag = DEF_CAL_OFF;
    
    return flag;    
}