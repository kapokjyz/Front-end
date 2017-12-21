/*
*******************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* ģ������: Bsp.c
* ժ    Ҫ: STM32F10X BSP��������ģ��
*
* ��ǰ�汾: 1.0
* ��    ��: ����
* �������: 2019-09-03
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
********************************************************************************
*                            ����ͷ�ļ�
********************************************************************************
*/
#include "Bsp.h"
/*
********************************************************************************
*                            �궨��
********************************************************************************
*/                                                       
#define  BSP_RCC_TO_VAL                 0x00000FFF 

/*
********************************************************************************
*                            �ڲ�����
********************************************************************************
*/ 

/*
********************************************************************************
********************************************************************************
*                            ���ܺ���
********************************************************************************
********************************************************************************
*/ 
/*
******************************************************************************
��������:	BSP_CPU_ClkFreq
��������:   ����CPUʱ��
�������:	none
�������:	none
�� �� ֵ:   CPUʱ��
��������:   2019-09-03
ע    ��:    
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
��������:	BSP_RCC_Init
��������:   ��ʼ��RCCģ�飬����FLASHʱ���ϵͳʱ�ӷ�Ƶ
�������:	none
�������:	none
�� �� ֵ:   none
��������:   2019-09-03
ע    ��:    
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
*                            �ӿں���
********************************************************************************
********************************************************************************
*/ 
/*
******************************************************************************
��������:	FeedWatchDog
��������:   ι�ڲ����Ź�
�������:	none
�������:	none
�� �� ֵ:   none
��������:   2019-09-03
ע    ��:    
*******************************************************************************
*/
void FeedWatchDog(void)
{
    IWDG_ReloadCounter();
}

/*
******************************************************************************
��������:	OS_CPU_SysTickClkFreq
��������:   ��ѯCPUƵ��
�������:	none
�������:	none
�� �� ֵ:   CPU����Ƶ��
��������:   2019-09-03
ע    ��:    
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
��������:	BSP_Init
��������:   ��ʼ����������
�������:	none
�������:	none
�� �� ֵ:   none
��������:   2019-09-03
ע    ��:    
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