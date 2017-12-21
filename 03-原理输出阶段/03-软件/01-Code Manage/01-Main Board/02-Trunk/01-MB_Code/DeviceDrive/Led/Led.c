/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: Led.c
* 摘    要: LedSTM32F10XX驱动程序
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
#include "Share.h"
#include "Led.h"
/*
********************************************************************************
*                               宏定义
********************************************************************************
*/
/*
    Led count
*/
#define DEF_LED_COUNT   6
/*
********************************************************************************
*                               类型定义
********************************************************************************
*/
/*      Led Source      */
typedef struct
{
  PORT_TypeDef *Port;
  PIN_TypeDef  Pin;
  CLK_TypeDef  Clk;
  RCC_XXXPeriphClockCmd ClockCmd;
}LED_SOURCE;
/*
********************************************************************************
*                               内部变量
********************************************************************************
*/
/*      
    Led Source      
*/
static LED_SOURCE m_LedSource[DEF_LED_COUNT]=
{ 
    /*       电池灯_红      */
    {GPIOB, GPIO_Pin_5, RCC_APB2Periph_GPIOB, RCC_APB2PeriphClockCmd},
    /*       电池灯_绿      */
    {GPIOC, GPIO_Pin_12, RCC_APB2Periph_GPIOC, RCC_APB2PeriphClockCmd},
    /*       适配器灯       */
    {GPIOB, GPIO_Pin_1, RCC_APB2Periph_GPIOB, RCC_APB2PeriphClockCmd},
    /*       氧浓度过高报警灯       */
    {GPIOB, GPIO_Pin_3, RCC_APB2Periph_GPIOB, RCC_APB2PeriphClockCmd},
    /*       氧浓度过低报警灯       */
    {GPIOD, GPIO_Pin_2, RCC_APB2Periph_GPIOD, RCC_APB2PeriphClockCmd},
    /*       报警静音灯       */
    {GPIOB, GPIO_Pin_7, RCC_APB2Periph_GPIOB, RCC_APB2PeriphClockCmd},  
};

/*控制LED灯亮灭*/
static INT8U m_LedContrl[DEF_LED_COUNT][2]=
{
  {DEF_LED_CLOSE, DEF_LED_OPEN},
  {DEF_LED_CLOSE, DEF_LED_OPEN},
  {DEF_LED_CLOSE, DEF_LED_OPEN},

  {DEF_LED_OPEN, DEF_LED_CLOSE},
  {DEF_LED_OPEN, DEF_LED_CLOSE},
  {DEF_LED_OPEN, DEF_LED_CLOSE},
}; 
/*
********************************************************************************
********************************************************************************
*                               接口函数
********************************************************************************
********************************************************************************
*/
/*
******************************************************************************
函数名称:   LED_Config
函数功能:   配置Led
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-08-28
注    意:   none
*******************************************************************************
*/
void LED_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	INT8U i;
	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    	
    for(i=0; i<DEF_LED_COUNT; i++)
    {     
        /*      配置时钟    */
        m_LedSource[i].ClockCmd(RCC_APB2Periph_AFIO|m_LedSource[i].Clk, ENABLE);
        
        /*      配置端口        */
        GPIO_InitStructure.GPIO_Pin = m_LedSource[i].Pin;
        GPIO_Init(m_LedSource[i].Port, &GPIO_InitStructure);
        
        /*      关闭Led   */
        if(i < 3)
        {
            GPIO_ResetBits(m_LedSource[i].Port, m_LedSource[i].Pin);
        }
        else
        {
            GPIO_SetBits(m_LedSource[i].Port, m_LedSource[i].Pin);
        }
        Debug("【Device】Led %d is close... \r\n", (i+1));
    }	
}
/*
******************************************************************************
函数名称:   LED_Set
函数功能:   打开或者关闭Led，高电平亮，低电平灭
输入参数:   led：编号
            status：设置的状态，1：打开，0：关闭
输出参数:   none
返 回 值:   none
创建日期:   2015-08-28
注    意:    
*******************************************************************************
*/
void LED_Set(INT8U led, INT8U status)
{
    INT8U flag = DEF_LED_CLOSE;
    
    flag = m_LedContrl[led][status];
    
    if(DEF_LED_OPEN == flag)
    {      
        /*      开灯      */
        GPIO_SetBits(m_LedSource[led].Port, m_LedSource[led].Pin);
        Debug("【Device】Led %d is open... \r\n", (led+1));
    }
    else
    {
        /*      关灯      */
        GPIO_ResetBits(m_LedSource[led].Port, m_LedSource[led].Pin);
        Debug("【Device】Led %d is close... \r\n", (led+1));
    }
}