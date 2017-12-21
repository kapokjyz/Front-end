/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: Bsp.c
* 摘    要: STM32F407XX BSP驱动程序模块
*
* 当前版本: 1.0
* 作    者: 黎明
* 完成日期: 2013-11-08
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

#include "GUIDRV_FlexColor.h"
#include "Share.h"


#define Bank1_LCD_C    (*((volatile unsigned short *) 0x6C000000/*0x6F000000*/)) /* CS = NE4 */
#define Bank1_LCD_D    (*((volatile unsigned short *) (0x6C000000|(1<<11))/*0x6F001000*/)) /* RS = FSMC_A11 */
                                                         
#define LCD_Addr      (*(vu16 *)Bank1_LCD_C)
#define LCD_Data      (*(vu16 *)Bank1_LCD_D)


/*
*********************************************************************
*
*       Layer configuration
*
**********************************************************************
*/
//
// 实际显示尺寸
//
#define XSIZE_PHYS 800
#define YSIZE_PHYS 480


#define DISPLAY_ORIENTATION  GUI_MIRROR_X


#define  LCD_RST_PIN            GPIO_Pin_9
#define  LCD_RST_PORT           GPIOB

#define  LCD_RS_PIN             GPIO_Pin_1
#define  LCD_RS_PORT            GPIOG

#define  LCD_CS_PIN             GPIO_Pin_12
#define  LCD_CS_PORT            GPIOG


/*    背光    */
#define  LCD_LP_PIN             GPIO_Pin_9
#define  LCD_LP_PORT            GPIOB


#define H_Sync_Pluse_Wide      30	 
#define H_Sync_to_DE	        144         // DE horizontal start position
#define H_Sync_total           800	 

#define V_Sync_Pluse_Wide      3	 
#define V_Sync_to_DE	        35         // DE horizontal start position
#define V_Sync_total           640	 


#define REFRESH_RATE	        58	      //Hz
#define PIXEL_CLOCK		(u32)(H_Sync_total  * V_Sync_total  * REFRESH_RATE)

#define OSC_FREQ		    10000000  //Hz
#define MULTIPLIER_N	35
#define DIVIDER_M		2
#define DCLK_Latch              0	    // 0: Rising       1: Falling

#define H_Sync_polarity         0 			// 0: Active low   1:Active High
#define V_Sync_polarity         0			// 0: Active low   1:Active High

#define VCO_FREQ					  (u32)(OSC_FREQ * (MULTIPLIER_N + 1))		//Hz, VCO Frequency is required to be larger than 250MHz
#define PLL_FREQ					  (u32)(VCO_FREQ / (DIVIDER_M + 1))		//Hz

#define CAL						  (u32)(((float)((float)PIXEL_CLOCK * 128) / (float)PLL_FREQ) * 8192)

#define LCDC_FPR					  (u32)(CAL - 1)
#define Polarity      				          (1<<3)|(DCLK_Latch<<2)|(H_Sync_polarity<<2)|(V_Sync_polarity<<2)

#define LCD_WriteADDR(RGB_ADDR)      ( (LCD_Addr) =  (RGB_ADDR) )
#define LCD_WriteRAM(RGB_Code)       ( (LCD_Data) =  (RGB_Code) )

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   NUM_VSCREENS
  #define NUM_VSCREENS 1
#else
  #if (NUM_VSCREENS <= 0)
    #error At least one screeen needs to be defined!
  #endif
#endif
#if (NUM_VSCREENS > 1) && (NUM_BUFFERS > 1)
  #error Virtual screens and multiple buffers are not allowed!
#endif
#ifndef   DISPLAY_ORIENTATION
  #define DISPLAY_ORIENTATION  0
#endif

/****************************************************************************
* 名    称：LCD_WR_REG(unsigned int index)
* 功    能：FSMC写显示器寄存器地址函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void LCD_WR_REG(unsigned short index)
{
	*(__IO uint16_t *) (Bank1_LCD_C)= index;
}			 

/****************************************************************************
* 名    称：void LCD_WR_CMD(unsigned int index,unsigned int val)
* 功    能：FSMC写显示器寄存器数据函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void LCD_WR_CMD(unsigned short index,unsigned short val)
{	
	*(__IO uint16_t *) (Bank1_LCD_C)= index;	
	*(__IO uint16_t *) (Bank1_LCD_D)= val;
}


/****************************************************************************
* 名    称：unsigned short LCD_RD_data(void)
* 功    能：FSMC读显示区16位数据函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
unsigned short LCD_RD_data(void)
{
	unsigned int a=0;
	a=*(__IO uint16_t *) (Bank1_LCD_D);   //空操作
	a=*(__IO uint16_t *) (Bank1_LCD_D);   //读出的实际16位像素数据	  
	return(a);	
}

/****************************************************************************
* 名    称：LCD_WR_Data(unsigned int val)
* 功    能：FSMC写16位数据函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void LCD_WR_Data(unsigned short val)
{   
	*(__IO uint16_t *) (Bank1_LCD_D)= val; 	
}

/****************************************************************************
* 名    称：void LCD_WR_M_Data(U16 * pData, int NumWords)
* 功    能：FSMC写多字节的16位数据函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void LCD_WR_M_Data(U16 * pData, int NumWords)
{
  for (; NumWords; NumWords--) 
  {
    *(__IO uint16_t *) (Bank1_LCD_D)= *pData++;
  }
}

/****************************************************************************
* 名    称：void LCD_RD_M_Data(U16 * pData, int NumWords)
* 功    能：FSMC读多字节的16位数据函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void LCD_RD_M_Data(U16 * pData, int NumWords)
{	 
  for (; NumWords; NumWords--){
  	//*pData++=ili9320_BGR2RGB(*(__IO uint16_t *) (Bank1_LCD_D));	
        *pData++= *(__IO uint16_t *) (Bank1_LCD_D);  
  }
}

/****************************************************************************
* 名    称：void Delay(__IO uint32_t nCount)
* 功    能：延时函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

/*******************************************************************************
* Function Name  : LCD_CtrlLinesConfig
* Description    : Configures LCD Control lines (FSMC Pins) in alternate function
                   Push-Pull mode.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void LCD_CtrlLinesConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	
	/* Enable GPIOs clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG, ENABLE);
	
	/* Enable FSMC clock */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE); 

	/*
	+-------------------+--------------------+------------------+------------------+
	+                       SRAM pins assignment                                   +
    +-------------------+--------------------+------------------+------------------+
	| PE0  <-> FSMC_D0  | PD4  <-> FSMC_NOE | PG1 <-> LCD_RS = FSMC_A11 | 
	| PE1  <-> FSMC_D1  | PD5  <-> FSMC_NWE | PG12<-> LCD_CS = FSMC_NE4 |
	| PE2  <-> FSMC_D2  |                   | PG13<-> FSMC_A24 |
	| PE3  <-> FSMC_D3  |----------------------------------------------+ 
	| PE4  <-> FSMC_D4  | 
	| PE5  <-> FSMC_D5  |  
	| PE6  <-> FSMC_D6  | 
	| PE7  <-> FSMC_D7  | 
	| PE8  <-> FSMC_D8  |                   
    | PE9  <-> FSMC_D9  |                  
    | PE10 <-> FSMC_D10 |
    | PE11 <-> FSMC_D11 |
    | PE12 <-> FSMC_D12 |
    | PE13 <-> FSMC_D13 |
    | PE14 <-> FSMC_D14 |
    | PE15 <-> FSMC_D15 |
    +-------------------+--------------------+------------------+------------------+
	*/
    
    /* 
        GPIOE configuration 
        FSMC_D0 -> FSMC_D15
    */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource0 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource6 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3  | GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_6 |
	                              GPIO_Pin_7  | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13|
                                  GPIO_Pin_14 | GPIO_Pin_15;
                                  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
    
	/* 
        GPIOD configuration
        FSMC_NOE : PD4
        FSMC_NWE : PD5
    */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;	                          
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* 
        GPIOD configuration
        FSMC_RS : FSMC_A11 = PG1
        FSMC_CS : FSMC_NE4 = PG12
        FSMC_A24 : =PG13
    */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource12 , GPIO_AF_FSMC);
	//GPIO_PinAFConfig(GPIOG, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_12;      
	GPIO_Init(GPIOG, &GPIO_InitStructure);
      
    /* 
        LCD RST configuration 
    */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void TestGpio(void)
{
    uint8_t value;
    GPIO_InitTypeDef GPIO_InitStructure; 
    
    value = 10;
      
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOB, ENABLE);
    
    /*
        Reset
    */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_ResetBits(GPIOB, GPIO_Pin_9);
    value = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9);
	GPIO_SetBits(GPIOB, GPIO_Pin_9);
    value = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9);
      
    /*
    ************
    
    | PD4  <-> FSMC_NOE | PG1 <-> LCD_RS = FSMC_A11 | 
	| PD5  <-> FSMC_NWE | PG12<-> LCD_CS = FSMC_NE4 |
	                    | PG13<-> FSMC_A24 |
    ************
    */
    
    /*
        RS
        CS
    */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_1; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    
    GPIO_ResetBits(GPIOG, GPIO_Pin_1);
    value = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_1);
	GPIO_SetBits(GPIOG, GPIO_Pin_1);
    value = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_1);
    
    GPIO_ResetBits(GPIOG, GPIO_Pin_12);
    value = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_12);
	GPIO_SetBits(GPIOG, GPIO_Pin_12);
    value = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_12);
    /*
        OE
        WE
    */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_ResetBits(GPIOD, GPIO_Pin_4);
    value = GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4);
	GPIO_SetBits(GPIOD, GPIO_Pin_4);
    value = GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4);
    
    GPIO_ResetBits(GPIOD, GPIO_Pin_5);
    value = GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_5);
	GPIO_SetBits(GPIOD, GPIO_Pin_5);
    value = GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_5);
   
    value = value;
    /*****************************/
}
/**
  * @brief  Configures the Parallel interface (FSMC) for LCD(Parallel mode)
  * @param  None
  * @retval None
  */
void LCD_FSMCConfig(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMTimingInitStructure;
	
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
	//FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
	
    /* FSMC Write Timing */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 1;   /* Address Setup Time */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 1;	   
	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 4;	     /* Data Setup Time */
	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	/* FSMC Access Mode */
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;	  
	
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
	
	/* Enable FSMC Bank4_SRAM Bank */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  
}

static void Clear_Display(void)
{ 
   	u16 k,l;
	LCD_WriteADDR( 0x2a);
	LCD_WriteRAM(0);
	LCD_WriteRAM(0);
	LCD_WriteRAM(((XSIZE_PHYS-1)&0xff00)>>8);
	LCD_WriteRAM((XSIZE_PHYS-1)&0x00ff); 

	//Set Y Address
	LCD_WriteADDR( 0x2b);
	LCD_WriteRAM(0);
	LCD_WriteRAM(0);
	LCD_WriteRAM(((YSIZE_PHYS-1)&0xff00)>>8);
	LCD_WriteRAM((YSIZE_PHYS-1)&0x00ff); 
	
	LCD_WriteADDR( 0x2C);
	LCD_WriteADDR( 0x3C);	
	for(k=0;k<480;k++)
	{ 
		for(l=0;l<640;l++)
		{   
			LCD_WriteRAM(0);
		}
	}	
}

/*******************************************************************************
* Function Name  : STM3210E_LCD_Init
* Description    : Initializes the LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void STM3210E_LCD_Init(void)
{ 
	CPU_INT16U tmp = 0;

    TestGpio();
      
	LCD_CtrlLinesConfig();
    LCD_FSMCConfig();

	GPIO_ResetBits(LCD_RST_PORT, LCD_RST_PIN);
	Delay(0xFFFF);
	GPIO_SetBits(LCD_RST_PORT, LCD_RST_PIN);
	Delay(0xFFFF);
	
	// Set PLL frequency
        /*    设置锁相环    */
	LCD_WriteADDR(0xe2);							     	//Set PLL: VCO (> 250MHz) = OSC x (N + 1)
	LCD_WriteRAM(MULTIPLIER_N); 						    //Multiplier N, VCO = 360MHz 
	LCD_WriteRAM(DIVIDER_M); 		       				    //Divider M, PLL = 120MHz (Standard)
	LCD_WriteRAM(0x04); 								    //dummy write, no meaning. PLL = VCO / (M + 1)
	/*    启动锁相环    */
	LCD_WriteADDR(0xe0);									//Command: Set SSD1961 PLL
	LCD_WriteRAM(0x01);									    //Turn on PLL
	Delay(2000);                           					// Delay for PLL stable
	/*    */
    LCD_WriteADDR(0xe0);									//Command: Set SSD1961 PLL
	LCD_WriteRAM(0x03);									    //Switch the clock source to PLL

	while((tmp &0x0004) != 0x0004)
	{	
		Delay(50);
		LCD_WriteADDR(0xe4);	
		tmp = LCD_Data;										// wait for 100ms for the PLL to lock.
	}
											
	LCD_WriteADDR(0x01); 							   	    //Command: Soft reset SSD1963
	Delay(100);												// Delay for RESET
	LCD_WriteADDR( 0x36); 
	LCD_WriteRAM( 0x02);
        
	LCD_WriteADDR( 0xE6); 							 	    // pclk = pll freq * (setting + 1) / 0x100000		//Set LSHIFT frequency
	LCD_WriteRAM( (LCDC_FPR&0x000F0000)>>16); 			    //Remark: 0x100000 must be divided by (setting+1), otherwise, pclk may oscilate
	LCD_WriteRAM( (LCDC_FPR&0x0000FF00)>>8); 			    //pclk = 5MHz
	LCD_WriteRAM( (LCDC_FPR&0x000000FF)); 				    //refresh rate = 5MHz / (336 * 244) = 60.98Hz
	
	LCD_WriteADDR( 0xb0); 								    // display period		//Set LCD mode / pad strength
	LCD_WriteRAM( Polarity); 							    // [B5:0] - 18Bit [B2:1] LSHIFT data latch falling edge [B1:0] LLINE active low [B0:0] - LFRAME active low
	LCD_WriteRAM( 0x00);
	LCD_WriteRAM( ((XSIZE_PHYS-1)&0xFF00)>>8); 			//0x13F = 320 - 1 (Horizontal)
	LCD_WriteRAM( (XSIZE_PHYS-1)&0x00FF);
	LCD_WriteRAM( ((YSIZE_PHYS-1)&0xFF00)>>8); 			//0x0ef = 240 - 1 (Vertical)
	LCD_WriteRAM( (YSIZE_PHYS-1)&0x00FF);
	LCD_WriteRAM( 0x00);									// It's RGB mode

	LCD_WriteADDR( 0xb4); 					        // hsync
	LCD_WriteRAM( ((H_Sync_total-1)&0xFF00)>>8); 		        // ht LCD_TOTAL_WIDTH - 1
	LCD_WriteRAM( (H_Sync_total-1)&0x00FF); 
	LCD_WriteRAM( (H_Sync_to_DE&0x0700)>>8);
	LCD_WriteRAM( (H_Sync_to_DE)&0x00FF); 				 // Horizontal Start 8 pclks
	LCD_WriteRAM( H_Sync_Pluse_Wide - 1); 			        // pulse width = setting + 1 clock = 2 clock
	LCD_WriteRAM( 0x00);
	LCD_WriteRAM( 0x00);

	LCD_WriteADDR( 0xb6); 					    // vsync
	LCD_WriteRAM( ((V_Sync_total-1)&0xFF00)>>8); 		    // vt 244-1
	LCD_WriteRAM( (V_Sync_total-1)&0x00FF); 
	LCD_WriteRAM( (V_Sync_to_DE&0x0700)>>8);
	LCD_WriteRAM( (V_Sync_to_DE)&0x00FF); 				    // Vertical Start Position: 2 line
	LCD_WriteRAM( V_Sync_Pluse_Wide - 1); 			        // pulse width = setting + 1 line = 2 line
	LCD_WriteRAM( 0x00);
	LCD_WriteRAM( 0x00);

	LCD_WriteADDR( 0x3A); 
	LCD_WriteRAM( 0x50);
	
	LCD_WriteADDR( 0x29); // display on
	
	LCD_WriteADDR( 0xb8); 								// config gpio
	LCD_WriteRAM( 0x0f);
	LCD_WriteRAM( 0x01);

        /*    设置LCD显示方向   */
	LCD_WriteADDR( 0xba); 								// Set GPIO 
	LCD_WriteRAM( 0x0f);  
	Delay(50);

	//Set X Address
	LCD_WriteADDR( 0x2a);
	LCD_WriteRAM( 0x00);
	LCD_WriteRAM( 0x00);
    LCD_WriteRAM( ((XSIZE_PHYS-1) & 0xFF00)>>8);
	LCD_WriteRAM( ((XSIZE_PHYS-1) & 0x00FF));

	//Set Y Address
	LCD_WriteADDR( 0x2b);
	LCD_WriteRAM( 0x00);
	LCD_WriteRAM( 0x00);
	LCD_WriteRAM( ((YSIZE_PHYS-1) & 0xFF00)>>8);
	LCD_WriteRAM( ((YSIZE_PHYS-1) & 0x00FF)); 

	//Set the read order from frame buffer to the display panel
	//LCD_WriteADDR(0x36); 
	//LCD_WriteRAM(0x20);
    //LCD_WriteRAM(0b0100110);
	
	LCD_WriteADDR( 0xbe);									// pwm config
	LCD_WriteRAM( 0x06);									// PWM Frequency
	LCD_WriteRAM( 240);									// duty of the PWM	
	LCD_WriteRAM( 0x09);									// pwm controled by the host
	LCD_WriteRAM( 0xFF);									//D[7:0] : DBC manual brightness (00 Dimmest FF brightest)
	LCD_WriteRAM( 0xFF);									//D[7:0] : DBC minimum brightnesss (00 Dimmest FF brightest)
	LCD_WriteRAM( 0x0F);									// F[3:0] : Brightness prescaler (0 Dimmest F brightest)
		
	LCD_WriteADDR( 0xd4);//set threshold;
	LCD_WriteRAM( 0x00);
	LCD_WriteRAM( 0x16);
	LCD_WriteRAM( 0x80);
	LCD_WriteRAM( 0x00);
	LCD_WriteRAM( 0x38);
	LCD_WriteRAM( 0x40);
	LCD_WriteRAM( 0x00);
	LCD_WriteRAM( 0x87);
	LCD_WriteRAM( 0x00);	
		
	LCD_WriteADDR( 0xd0);//enable dbc
	LCD_WriteRAM( 0<<6 | 1<<5 | 1<<2 | 1<<0);  
	                      // A[6] : ABC bypass 0: ABC is not bypassed 1 ABC is bypassed
                          // A[5] : Transition effect
						         // 0 Transition effect disable  1 Transition effect enable
						  // A[3:2] : Energy saving selection for DBC (POR = 00)
						  //        00 DBC is disable  01 Conservative mode 10 Normal mode 11 Aggressive mode
						  // A[0] : Master enable of ABC and DBC 
						  // 0 DBC/ABC is not enabled 1 DBC/ABC is enabled	
   // Set
	LCD_WriteADDR( 0xf0); 
	LCD_WriteRAM( 0x03);
      
    Clear_Display();
}


/****************************************************************************
* 名    称：LCD_X_Config(void)
* 功    能：显示器的驱动配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void LCD_X_Config(void) 
{
  GUI_PORT_API PortAPI = {0}; 
  CONFIG_FLEXCOLOR Config = {0};
  GUI_DEVICE * pDevice;
  
  #if (NUM_BUFFERS > 1)
    GUI_MULTIBUF_Config(NUM_BUFFERS);
  #endif
  
  pDevice = GUI_DEVICE_CreateAndLink(GUIDRV_FLEXCOLOR, GUICC_M565, 0, 0);
  
  /* 公共显示驱动配置 */
    LCD_SetSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);               // 实际显示像素
    
  /* 公共显示驱动配置 */
  if (LCD_GetSwapXY()) {
        LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
        LCD_SetVSizeEx(0, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
    } 
    else {
        LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
        LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
  }			
  
  /* 设置命令和数据模式 */ 	   
  PortAPI.pfWrite16_A0  = LCD_WR_REG;
  PortAPI.pfWrite16_A1  = LCD_WR_Data;
  PortAPI.pfWriteM16_A1 = LCD_WR_M_Data;
  PortAPI.pfReadM16_A1  = LCD_RD_M_Data;
  GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66720,GUIDRV_FLEXCOLOR_M16C0B16);

  /* 显示方向和偏移*/                    
  Config.Orientation   = DISPLAY_ORIENTATION;
  Config.RegEntryMode  = 0;
  GUIDRV_FlexColor_Config(pDevice, &Config);
}

/****************************************************************************
* 名    称：LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData)
* 功    能：显示器的驱动初始化
* 入口参数：LayerIndex - 配置层的序号
*   		Cmd        - 请参考下面的switch语句的细节
*   		pData      - LCD_X_DATA指针结构
* 出口参数：< -1 - 错误
*     		  -1 - 命令不处理
*      		   0 - 正确
* 说    明：
* 调用方法：无 
****************************************************************************/ 
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;

  GUI_USE_PARA(LayerIndex);
  GUI_USE_PARA(pData);

  switch (Cmd){  
  	case LCD_X_INITCONTROLLER: 
  	{	   
       STM3210E_LCD_Init();      //LCD初始化
       return 0;
    }
    default:  r = -1;
  }
  return r;
}

/*************************** End of file ****************************/
