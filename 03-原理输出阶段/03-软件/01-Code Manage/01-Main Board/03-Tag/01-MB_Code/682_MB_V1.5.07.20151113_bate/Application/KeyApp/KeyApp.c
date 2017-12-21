/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: KeyApp.c
* 摘    要: 按键应用模块程序
*
* 当前版本: 1.0
* 作    者: 曾健
* 完成日期: 2015-08-28
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
#include "KeyApp.h"
#include "Alarm.h"
#include "Display.h"
#include "Calibration.h"
#include "StartingUp.h"
/*
********************************************************************************
*                               类型定义
********************************************************************************
*/
/*
********************************************************************************
*                                       内部变量
********************************************************************************
*/
/*      计算次数       */
static INT32U m_KeCount = 0;
/*      报警设置标志      */
static BOOLEAN  m_KeAlarmSetFlag = DEF_KEY_OFF;
/*      报警设置成功标志       */
static BOOLEAN  m_KeAlarmSetSucFlag = DEF_KEY_OFF;
/*      保存报警设置值     */
static INT8U  m_KeAlarmValue[2] = {0};
/*
********************************************************************************
********************************************************************************
*                               接口函数
********************************************************************************
********************************************************************************
*/
/*
********************************************************************************
* 函数名称: KEYAPP_Exec
* 函数功能: 按键任务执行函数
* 入口参数: void
* 出口参数: void
* 返 回 值: void
* 日    期: 2015-08-28
* 修改问题:
* 修改内容:
********************************************************************************
*/
void KEYAPP_Exec(void)
{
   OSTimeDlyHMSM(0,0,0,100);
   /*       非静音状态氧电池正常时才能操作此功能     */
   if(0 == ALARM_GetIsDisableSound() && (STARTINGUP_FiO2ErrorFlag() == 0))
   {
       /*       长按进入报警设置        */
       if((KEY_Scan() == DEF_KEY_CODE_ALARM_SOUND) && (CALIBRATION_IsAdj() == 0)) 
       {
           m_KeCount++;
           /*    长按时间到了，就设置报警设置标志为1    */
           if(m_KeCount > 9)
           {
               m_KeAlarmSetFlag = DEF_KEY_ON;
               LED_Set(DEF_LED_ALARM_SOUND,DEF_LED_OPEN);              
           }
       }
       else
       {
            /*      长按时间到了且松开了      */
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
* 函数名称: KEYAPP_GetAlarmSetFlag
* 函数功能: 获得报警设置标志
* 入口参数: void
* 出口参数: void
* 返 回 值: void
* 日    期: 2015-08-28
* 修改问题:
* 修改内容:
********************************************************************************
*/
INT8U KEYAPP_GetAlarmSetFlag(void)
{
    return m_KeAlarmSetFlag;
}
/*
********************************************************************************
* 函数名称: KEYAPP_Takeoff
* 函数功能: 检测按键是否松开
* 入口参数: void
* 出口参数: void
* 返 回 值: 1,松开；0，没松开
* 日    期: 2015-08-28
* 修改问题:
* 修改内容:
********************************************************************************
*/
INT8U KEYAPP_Takeoff(INT8U key_type)
{
    INT8U Key_flag = DEF_KEY_TAKEOFF;
    
    switch(key_type)
    {
    /*      电源键        */
	case  DEF_KEY_CODE_PPOWER_SWITCH:
    {
       /*       检测按键是否松开        */ 
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
    
    /*      校准键        */
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
    
    /*      报警静音键        */
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
* 函数名称: KEYAPP_GetAlarmSetValue
* 函数功能: 获得报警的上下限值
* 入口参数: void
* 出口参数: void
* 返 回 值: void
* 日    期: 2015-08-28
* 修改问题:
* 修改内容:
********************************************************************************
*/
INT8U KEYAPP_GetAlarmSetValue(INT16U num)
{
    return m_KeAlarmValue[num];
}
/*
********************************************************************************
* 函数名称: KEYAPP_GetAlarmSetSucflag
* 函数功能: 获得报警的上下限值设置成功标志
* 入口参数: void
* 出口参数: void
* 返 回 值: void
* 日    期: 2015-08-28
* 修改问题:
* 修改内容:
********************************************************************************
*/
INT8U KEYAPP_GetAlarmSetSucflag(void)
{
    INT8U flag = 0;
    
    flag = m_KeAlarmSetSucFlag;
    m_KeAlarmSetSucFlag = DEF_KEY_OFF;
    
    return flag;
}