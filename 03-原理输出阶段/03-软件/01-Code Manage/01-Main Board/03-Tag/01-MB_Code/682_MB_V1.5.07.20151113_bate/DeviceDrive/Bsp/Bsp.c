/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: Bsp.c
* 摘    要: STM32F10X BSP驱动程序模块
*
* 当前版本: 1.0
* 作    者: 曾健
* 完成日期: 2019-09-03
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
********************************************************************************
*                            包含头文件
********************************************************************************
*/
#include "Bsp.h"
/*
********************************************************************************
*                            宏定义
********************************************************************************
*/                                                       
#define  BSP_RCC_TO_VAL                 0x00000FFF 

/*
********************************************************************************
*                            内部变量
********************************************************************************
*/ 

/*
********************************************************************************
********************************************************************************
*                            功能函数
********************************************************************************
********************************************************************************
*/ 
/*
******************************************************************************
函数名称:	BSP_CPU_ClkFreq
函数功能:   计算CPU时钟
输入参数:	none
输出参数:	none
返 回 值:   CPU时钟
创建日期:   2019-09-03
注    意:    
*******************************************************************************
*/
static INT32U BSP_CPU_ClkFreq (void)
{
    static RCC_ClocksTypeDef rcc_clocks;
    
    RCC_GetClocksFreq(&rcc_clocks);
    
    return ((INT32U)rcc_clocks.HCLK_Frequency);
}
/*
******************************************************************************
函数名称:	BSP_RCC_Init
函数功能:   初始化RCC模块，设置FLASH时序和系统时钟分频
输入参数:	none
输出参数:	none
返 回 值:   none
创建日期:   2019-09-03
注    意:    
*******************************************************************************
*/
static void BSP_RCCInit(void) 
{    
    /* RCC registers timeout*/
    INT32U  rcc_to;                                          

    /*  Reset the RCC clock config to the default reset state */
    RCC_DeInit();                                                

    /*  HSE Oscillator ON*/   
    RCC_HSEConfig(RCC_HSE_ON);                                        
    
    rcc_to = BSP_RCC_TO_VAL;
    
    /* Wait until the oscilator is stable*/
    while ((rcc_to > 0) && 
           (RCC_WaitForHSEStartUp() != SUCCESS)) {               
        rcc_to--;
    }
      
    FLASH_SetLatency(FLASH_Latency_2);
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    
    /* Fcpu = (PLL_src * PLL_MUL) = (12 Mhz / 1) * (6) = 72Mhz */ 
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);         
    RCC_PLLCmd(ENABLE);

    rcc_to = BSP_RCC_TO_VAL;

    while ((rcc_to > 0) &&
           (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)) {
        rcc_to--;
    }
    
    /* Set system clock dividers*/   
    RCC_HCLKConfig(RCC_SYSCLK_Div1);                                                      
    RCC_PCLK2Config(RCC_HCLK_Div1);    
    RCC_PCLK1Config(RCC_HCLK_Div1);    
    RCC_ADCCLKConfig(RCC_PCLK2_Div4);

    /* Embedded Flash Configuration*/
    FLASH_SetLatency(FLASH_Latency_2);                           
    FLASH_HalfCycleAccessCmd(FLASH_HalfCycleAccess_Disable);
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
  
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}
/*
********************************************************************************
********************************************************************************
*                            接口函数
********************************************************************************
********************************************************************************
*/ 
/*
******************************************************************************
函数名称:	FeedWatchDog
函数功能:   喂内部看门狗
输入参数:	none
输出参数:	none
返 回 值:   none
创建日期:   2019-09-03
注    意:    
*******************************************************************************
*/
void FeedWatchDog(void)
{
    IWDG_ReloadCounter();
}

/*
******************************************************************************
函数名称:	OS_CPU_SysTickClkFreq
函数功能:   查询CPU频率
输入参数:	none
输出参数:	none
返 回 值:   CPU运行频率
创建日期:   2019-09-03
注    意:    
*******************************************************************************
*/
INT32U  OS_CPU_SysTickClkFreq (void)
{
    INT32U  freq;
    
    freq = BSP_CPU_ClkFreq();
    
    return (freq);
}
/*
******************************************************************************
函数名称:	BSP_Init
函数功能:   初始化驱动程序
输入参数:	none
输出参数:	none
返 回 值:   none
创建日期:   2019-09-03
注    意:    
*******************************************************************************
*/
void BSP_Init(void)
{   
	BSP_IntInit();  
	BSP_IntDisAll();
    BSP_RCCInit(); 
	
	OS_CPU_SysTickInit(); 
    
    I2C_EE_Init();
    
    LED_Config();
    KEY_Config();
    POWER_Config();
    OLED_Config();
    PAW_LedConfig();
    BUZZER_Config();
    BUZZER_TIM3Init();
}