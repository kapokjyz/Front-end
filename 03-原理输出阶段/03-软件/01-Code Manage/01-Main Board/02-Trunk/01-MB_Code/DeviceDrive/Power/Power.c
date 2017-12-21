/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: Power.c
* 摘    要: PowerSTM32F10XX驱动程序
*
* 当前版本: 1.0
* 作    者: 曾健
* 完成日期: 2015-09-10
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
#include "Share.h"
#include "Power.h"
/*
********************************************************************************
*                               宏定义
********************************************************************************
*/
/*
    Power Switch count
*/
#define DEF_POWER_SWITCH_COUNT   5
/*
    Power Status count
*/
#define DEF_POWER_STATUS_COUNT   2
/*
    Power Value count
*/
#define DEF_POWER_VALUE_COUNT    7

/*
********************************************************************************
*                               类型定义
********************************************************************************
*/
/*      Power Source      */
typedef struct
{
  PORT_TypeDef *Port;
  PIN_TypeDef  Pin;
  CLK_TypeDef  Clk;
  RCC_XXXPeriphClockCmd ClockCmd; 
}POWER_SOURCE;
/*
********************************************************************************
*                               内部变量
********************************************************************************
*/
/*      
    Power Switch Source      
*/
static POWER_SOURCE m_PowerSwithSource[DEF_POWER_SWITCH_COUNT]=
{   
    /*       电源使能  */
    {GPIOA, GPIO_Pin_8, RCC_APB2Periph_GPIOA, RCC_APB2PeriphClockCmd},
    
    /*       电磁阀电源使能   */
    {GPIOC, GPIO_Pin_8, RCC_APB2Periph_GPIOC, RCC_APB2PeriphClockCmd},
    
    /*       电磁阀开关控制使能   */
	{GPIOB, GPIO_Pin_9, RCC_APB2Periph_GPIOB, RCC_APB2PeriphClockCmd},  
	
    /*       充电芯片使能   */ 
	{GPIOB, GPIO_Pin_0, RCC_APB2Periph_GPIOB, RCC_APB2PeriphClockCmd},  
	
	/*       开机稳定       */
    {GPIOA, GPIO_Pin_0, RCC_APB2Periph_GPIOA, RCC_APB2PeriphClockCmd},     
};
/*
********************************************************************************
********************************************************************************
*                               功能函数
********************************************************************************
********************************************************************************
*/
/*
******************************************************************************
函数名称:   POWER_SwitchGpioConfig
函数功能:   配置电源开关GPIO
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-09-10
注    意:   none
*******************************************************************************
*/
static void POWER_SwitchGpioConfig(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    uint8_t i;
    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    for(i=0; i<DEF_POWER_SWITCH_COUNT; i++)
    {     
        /*      配置时钟    */
        m_PowerSwithSource[i].ClockCmd(m_PowerSwithSource[i].Clk, ENABLE);
        
        /*      配置端口        */
        GPIO_InitStructure.GPIO_Pin = m_PowerSwithSource[i].Pin;
        GPIO_Init(m_PowerSwithSource[i].Port, &GPIO_InitStructure);    
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
函数名称:   POWER_Config
函数功能:   配置POWER
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-09-10
注    意:   none
*******************************************************************************
*/
void POWER_Config(void)
{
    /*      配置电源开关GPIO       */
    POWER_SwitchGpioConfig();
}
/*
******************************************************************************
函数名称:   POWER_SwitchSet
函数功能:   打开或者关闭电源，高电平开，低电平关
输入参数:   power：电源编号
            status：设置的状态，1：打开，0：关闭
输出参数:   none
返 回 值:   none
创建日期:   2015-09-10
注    意:    
*******************************************************************************
*/
void POWER_SwitchSet(INT8U power, INT8U status)
{
    if(DEF_POWER_OPEN == status)
    {      
        /*      开电源      */
        GPIO_SetBits(m_PowerSwithSource[power].Port, m_PowerSwithSource[power].Pin);
        Debug("【Device】Power %d is open... \r\n", (power+1));
    }
    else
    {
        /*      关电源      */
        GPIO_ResetBits(m_PowerSwithSource[power].Port, m_PowerSwithSource[power].Pin);
        Debug("【Device】Power %d is close... \r\n", (power+1));
    }
}
