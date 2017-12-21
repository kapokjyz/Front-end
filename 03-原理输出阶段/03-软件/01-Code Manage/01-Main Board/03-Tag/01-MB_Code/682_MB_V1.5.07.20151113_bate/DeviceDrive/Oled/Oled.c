/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: Oled.c
* 摘    要: Oled驱动程序
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
#include "Oled.h"
#include "k096.h"
/*
*******************************************************************************
*                               内部变量
*******************************************************************************
*/
/*开机进行一次初始化的标志*/
static INT8U m_FirstInit = 0;
/*
********************************************************************************
********************************************************************************
*                               接口函数
********************************************************************************
********************************************************************************
*/
/*
********************************************************************************
* 函数名称: OLED_Config
* 函数功能: 配置Oled
* 入口参数: void
* 出口参数: void
* 返 回 值: void
* 日    期: 2015-08-28
* 修改问题:
* 修改内容:
********************************************************************************
*/
void OLED_Config(void)
{  
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* OUTPUT IO 初始化 oled接口*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    /* OUTPUT IO 初始化 oled接口*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);     
}
/*
********************************************************************************
* 函数名称: OLED_Display
* 函数功能: Oled显示函数
* 入口参数: void
* 出口参数: void
* 返 回 值: void
* 日    期: 2015-08-28
* 修改问题:
* 修改内容:
********************************************************************************
*/
void OLED_Display(int num)
{  
    INT32U a = 0, b = 0, c = 0; 
    
    /*就开机后初始化一次，后面就不执行了*/
    if(m_FirstInit == 0)
    {
        m_FirstInit = 1;
        EnVp_H;  //驱动板升压使能
        initi_oled();//初始化OLED
        LCD_Clear();
    }
        
    OSTimeDlyHMSM(0,0,0,50);
    
    a = num % 10;
    num = num / 10;
    b = num % 10;
    num = num /10;
    c = num;
    
    showzifu(7, c,4, 0);
    showzifu(0x27, b, 4, 0);
    showzifu(0x47, a, 4, 0);
}
/*
********************************************************************************
* 函数名称: OLED_Version
* 函数功能: Oled显示软件版本号函数
* 入口参数: void
* 出口参数: void
* 返 回 值: void
* 日    期: 2015-10-15
* 修改问题:
* 修改内容:
********************************************************************************
*/
void OLED_Version(int num)
{
    INT32U a = 0, b = 0, c = 0, d = 0; 
   
    LCD_Clear();
    
    OSTimeDlyHMSM(0,0,0,50);
    
    a = num % 10;
    num = num / 10;
    b = num % 10;
    num = num /10;
    c = num % 10;
    num = num / 10;
    d = num;
    
    showzifu(3, d,4, 0);
    showzifu(0x1B, c, 4, 0);
    showzifu(0x33, b, 4, 0);
    showzifu(0x4B, a, 4, 0);
}