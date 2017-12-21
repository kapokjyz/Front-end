/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: Key.c
* 摘    要: STM32F10XX按键驱动程序
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
#include  "Share.h"
#include  "Key.h"
/*
********************************************************************************
*                               宏定义
********************************************************************************
*/
/*      Key Count   */
#define DEF_KEY_COUNT      3
/*      没按按键        */
#define DEF_KEY_NONE        0
/*
********************************************************************************
*                               结构定义
********************************************************************************
*/
/*      Key Source      */
typedef struct
{
  PORT_TypeDef *Port;
  PIN_TypeDef  Pin;
  CLK_TypeDef  Clk;
  RCC_XXXPeriphClockCmd ClockCmd;
}KEY_SOURCE;
/*
********************************************************************************
*                               内部变量
********************************************************************************
*/
/*      
    Key Source      
*/
static KEY_SOURCE m_KeySource[DEF_KEY_COUNT]=
{
  /*        k1 : 开关机键      */
  {GPIOC, GPIO_Pin_3, RCC_APB2Periph_GPIOC, RCC_APB2PeriphClockCmd},
    /*      k2 : 校准键      */
  {GPIOA, GPIO_Pin_1, RCC_APB2Periph_GPIOA, RCC_APB2PeriphClockCmd},
    /*      k3 : 报警键      */
  {GPIOA, GPIO_Pin_2, RCC_APB2Periph_GPIOA, RCC_APB2PeriphClockCmd},
};
/*      
    Key Value      
*/
static INT8U m_KeyValue[DEF_KEY_COUNT]=
{
    DEF_KEY_CODE_PPOWER_SWITCH,
    DEF_KEY_CODE_ADJUST,
    DEF_KEY_CODE_ALARM_SOUND
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
函数名称:   KEY_Config
函数功能:   按鍵配置函数
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-08-28
注    意:    
*******************************************************************************
*/
void KEY_Config(void)
{
    INT8U i;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    for (i=0; i<DEF_KEY_COUNT; i++)
    {
        /*      配置时钟    */
        m_KeySource[i].ClockCmd(m_KeySource[i].Clk, ENABLE);
        
        /*      配置端口        */
        GPIO_InitStructure.GPIO_Pin = m_KeySource[i].Pin;
        GPIO_Init(m_KeySource[i].Port, &GPIO_InitStructure);
    }
}
/*
******************************************************************************
函数名称:   KEY_Scan
函数功能:   按键扫描
输入参数:   none
输出参数:   none
返 回 值:   按键键值
创建日期:   2015-08-28
注    意:    
*******************************************************************************
*/
INT8U KEY_Scan(void)
{
    INT8U i;
    INT8U key_value;
    
    key_value = DEF_KEY_NONE;
    
    for (i=0; i<DEF_KEY_COUNT; i++)
    {
        if(!GPIO_ReadInputDataBit(m_KeySource[i].Port,m_KeySource[i].Pin))
        {
            key_value = m_KeyValue[i];
            break;
        }
    }
    
    return key_value;
}