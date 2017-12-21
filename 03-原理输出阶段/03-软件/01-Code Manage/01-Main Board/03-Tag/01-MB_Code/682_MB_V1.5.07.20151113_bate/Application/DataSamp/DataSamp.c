/*
*******************************************************************************
*                  深圳市安保科技有限公司开发部                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* 模块名称: DataSamp.h
* 摘    要: 数据采样模块程序头文件
*
* 当前版本: 1.0
* 作    者: 曾健
* 完成日期: 2015-09-02
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
#include "DataSamp.h"
#include "Calibration.h"
#include "StartingUp.h"
/*
********************************************************************************
*                               宏定义
********************************************************************************
*/
/*       氧气  AD                          */
#define          DEF_O2_AD_PORT                 (GPIOC)
#define          DEF_O2_AD_PIN                  (GPIO_Pin_2)
#define          DEF_O2_AD_CHANNEL              (ADC_Channel_12)

/*       气道压 AD                          */
#define          DEF_PAW_AD_PORT                (GPIOC)
#define          DEF_PAW_AD_PIN                 (GPIO_Pin_1)
#define          DEF_PAW_AD_CHANNEL             (ADC_Channel_11)

/*       电池 AD                          */
#define          DEF_BAT_AD_PORT                (GPIOC)
#define          DEF_BAT_AD_PIN                 (GPIO_Pin_5)
#define          DEF_BAT_AD_CHANNEL             (ADC_Channel_15)

/*       适配器  AD                          */
#define          DEF_VAC_AD_PORT                (GPIOC)
#define          DEF_VAC_AD_PIN                 (GPIO_Pin_4)
#define          DEF_VAC_AD_CHANNEL             (ADC_Channel_14)

#define DEF_ADC1DR_Address                 ((u32)0x4001244C)
#define DEF_O2_FITER_SIZE               (7)
#define DEF_BAT_FITER_SIZE              (7)
#define DEF_ADAPTOR_FITER_SIZE          (7)
#define DEF_PAW_FITER_SIZE              (7)

#define DEF_DATASAMP_BUFFER_COUNT		7
#define DEF_DATASAMP_VOLTAGE_COUNT		4
/*
********************************************************************************
*                               类型定义
********************************************************************************
*/
/*      数据采样结构    */
typedef struct 
{
    INT16U Buf[DEF_DATASAMP_BUFFER_COUNT];
    INT16U AdValue;
	FP32 Voltage;
      
}DATASAMP_FILTER_BUF;
/*
********************************************************************************
*                               内部变量
********************************************************************************
*/
static DATASAMP_FILTER_BUF m_DaFilterBuf[DEF_DATASAMP_VOLTAGE_COUNT];
/*      ADC Value Buffer      */
static INT16U m_DaAdcValueBuf[DEF_DATASAMP_VOLTAGE_COUNT];

/*      保存采集的氧浓度和气道压的AD值*/
static INT16U m_DaADFiO2[2] = {0};
static INT16U m_DaADPaw[13] = {0};

/*      校准获取到的参数值和氧浓度与气道压的计算用到的参数值*/
static FP32 m_DaKO2 = 0.0;
static FP32 m_DaBO2 = 0.0;
static FP32 m_DaKPAW = 0.0;
static FP32 m_DaBPAW = 0.0;

/*      保存氧浓度值      */
static FP32 m_DaO2Value = 0.0;
/*      保存气道压值      */
static FP32 m_DaPawValue = 0.0;
/*      保存从EEPROM中读取出的校准数据      */
static INT8U m_DaBufferCalibration[30] = {0};
/*      作为只运行一次的标志      */
static INT8U m_DaOneFlag = 0;
/*
********************************************************************************
* 函数名称: MValue_Filter
* 函数功能: 中值滤波
* 入口参数: data 滤波数据源
*           Bufptr 滤波窗口
*           Windows_Size 滤波窗口大小
* 出口参数: void
* 返 回 值: void
* 日    期: 2015-09-02
* 修改问题:
* 修改内容:
********************************************************************************
*/
static INT16U DATASAMP_MidValueFilter(INT16U data,INT16U *Bufptr, INT16U  Windows_Size)
{
    INT16U *ptr;
    INT32U ir_Sum = 0x00000000;
    INT16U Cnt;
    INT16U Maxdata;
    INT16U Mindata;
    
    ptr = Bufptr;

    for (Cnt=0;Cnt<Windows_Size - 1;Cnt++) {
        
        ptr[Cnt]=ptr[Cnt+1];
    }
    ptr[Windows_Size-1]=data;

    Maxdata =   ptr[Cnt];
    Mindata =   ptr[Cnt];
    for (Cnt = 0; Cnt < Windows_Size; Cnt++) {
        
        ir_Sum += ptr[Cnt];
        if (Maxdata < ptr[Cnt + 1]) {
            Maxdata = ptr[Cnt + 1];
        }
        if (Mindata > ptr[Cnt + 1]) {
            Mindata = ptr[Cnt + 1];
        }
    }
    
    return ir_Sum/Windows_Size;	
}
/*
********************************************************************************
* 函数名称: VoltageCal
* 函数功能: 计算电压值
* 入口参数: ad采样值
* 出口参数: void
* 返 回 值: 电压值
* 日    期: 2015-09-02
* 修改问题:
* 修改内容:
********************************************************************************
*/
static  FP32 DATASAMP_CalVoltage(INT16U Vadc)
{
    FP32 voltage;
    
    voltage = (FP32)Vadc/4096.0;
    voltage = voltage * 3.3;
    
    return voltage;
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
函数名称:   DATASAMP_Init
函数功能:   初始化数据采样模块
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-09-02
注    意:   none
*******************************************************************************
*/
void DATASAMP_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
    ADC_InitTypeDef   ADC_InitStructure;
    DMA_InitTypeDef   DMA_InitStructure;
   
    /* Enable DMA1 clock                                                      */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
     
    /* Enable ADC1 clock                                                      */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    
    /*          O2       */
    GPIO_InitStructure.GPIO_Pin  = DEF_O2_AD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(DEF_O2_AD_PORT, &GPIO_InitStructure);
    
    /*          气道压    */
    GPIO_InitStructure.GPIO_Pin  = DEF_PAW_AD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(DEF_PAW_AD_PORT, &GPIO_InitStructure);
    
    /*          电池      */
    GPIO_InitStructure.GPIO_Pin  = DEF_BAT_AD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(DEF_BAT_AD_PORT, &GPIO_InitStructure);
    
    /*          适配器  */
    GPIO_InitStructure.GPIO_Pin  = DEF_VAC_AD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(DEF_VAC_AD_PORT, &GPIO_InitStructure);
    
    
    /* DMA1 channel1 configuration ----------------------------------------------*/
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = DEF_ADC1DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)m_DaAdcValueBuf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = DEF_ADC1_CHANNALS;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    
    /* Enable DMA1 channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);
    
    /* ADC1 configuration ------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = DEF_ADC1_CHANNALS;
    ADC_Init(ADC1, &ADC_InitStructure);
    	
    /* ADC1 regular channel13 configuration */ 
    ADC_RegularChannelConfig(ADC1, DEF_VAC_AD_CHANNEL, 1, ADC_SampleTime_13Cycles5);      /*适配器*/
    ADC_RegularChannelConfig(ADC1, DEF_BAT_AD_CHANNEL, 2, ADC_SampleTime_13Cycles5);    /* 电池*/
    ADC_RegularChannelConfig(ADC1, DEF_PAW_AD_CHANNEL, 3, ADC_SampleTime_13Cycles5);    /* 气道压*/
    ADC_RegularChannelConfig(ADC1, DEF_O2_AD_CHANNEL, 4, ADC_SampleTime_13Cycles5);    /* O2 */
    
    ADC_TempSensorVrefintCmd(ENABLE);
    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);

    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);
    
    /* Enable ADC1 reset calibaration register */   
    ADC_ResetCalibration(ADC1);
    /* Check the end of ADC1 reset calibration register */
    while(ADC_GetResetCalibrationStatus(ADC1));
    
    /* Enable ADC1 start calibaration register */   
    ADC_StartCalibration(ADC1);
    /* Check the end of ADC1 start calibration register */
    while(ADC_GetCalibrationStatus(ADC1));
    
    /* Start ADC1 Software Conversion     */ 
    ADC_SoftwareStartConvCmd(ADC1,ENABLE);
    
    /* Test on DMA1 channel1 transfer complete flag */
    while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
    /* Clear DMA1 channel1 transfer complete flag */
    DMA_ClearFlag(DMA1_FLAG_TC1);
}
/*
******************************************************************************
函数名称:   DATASAMP_Calib
函数功能:   数据校准
输入参数:   none
输出参数:   none
返 回 值:   none
创建日期:   2015-09-02
注    意:   none
*******************************************************************************
*/
void DATASAMP_Calib(void)
{
    INT32U i;
    
    /*开机第一次获取校准值*/
    if((1 == STARTINGUP_GetFlag()) && (m_DaOneFlag == 0))
    {  
        m_DaOneFlag = 1;
        for(i = 0; i < 30; i++)
        {
            m_DaBufferCalibration[i] =STARTINGUP_GetCalibValue(i);
        }  
        
        /*      把校准数据读取出来，以便使用     */
        for(i=0;i<2;i++)
        {
            m_DaADFiO2[i] = m_DaBufferCalibration[i*2] * 256 + m_DaBufferCalibration[i*2+1];
        }
        for(i=0;i<13;i++)
        {
            m_DaADPaw[i] = m_DaBufferCalibration[i*2 + 4] * 256 + m_DaBufferCalibration[i*2 + 5];
        }
    }
    
    /*校准成功时，读取校准值*/
    if(CALIBRATION_GetCalSucflag() == 1)
    {
        I2C_EE_BufferRead(EEPROM_DEVICE_ADDRESS,m_DaBufferCalibration, 0, 30);
        
        /*      把校准数据读取出来，以便使用     */
        for(i=0;i<2;i++)
        {
            m_DaADFiO2[i] = m_DaBufferCalibration[i*2] * 256 + m_DaBufferCalibration[i*2+1];
        }
        for(i=0;i<13;i++)
        {
            m_DaADPaw[i] = m_DaBufferCalibration[i*2 + 4] * 256 + m_DaBufferCalibration[i*2 + 5];
        }
    }
    
    /*氧浓度校准*/
    DATASAMP_AppAdjustO2(&m_DaKO2, &m_DaBO2, 21, 100, m_DaADFiO2[0], m_DaADFiO2[1]);
    
    /*气道压校准*/
    for(i = 0; i < 12; i++)
    {
        if(m_DaFilterBuf[ENUM_PAW].AdValue <= m_DaADPaw[i + 1])
        {
            DATASAMP_AppAdjustO2(&m_DaKPAW, &m_DaBPAW, i, i+1, m_DaADPaw[i], m_DaADPaw[i+1]);
            break;
        }
    }
}
/*
******************************************************************************
函数名称:   DATASAMP_Exec
函数功能:   数据采集任务执行函数
输入参数:   ENUM_ADAPTOR,ENUM_BAT,ENUM_PAW,ENUM_O2.
输出参数:   none
返 回 值:   返回采集到的适配器，电池，氧浓度，气道压的值
创建日期:   2013-12-02
注    意:   none
*******************************************************************************
*/
void DATASAMP_Exec(void)
{
    OSTimeDlyHMSM(0,0,0,10);
    
    /*      获取AD值       */
    m_DaFilterBuf[ENUM_O2].AdValue = DATASAMP_MidValueFilter(m_DaAdcValueBuf[ENUM_O2],
                                                            m_DaFilterBuf[ENUM_O2].Buf,
                                                            DEF_O2_FITER_SIZE);

    m_DaFilterBuf[ENUM_BAT].AdValue = DATASAMP_MidValueFilter(m_DaAdcValueBuf[ENUM_BAT],
                                                            m_DaFilterBuf[ENUM_BAT].Buf,
                                                            DEF_BAT_FITER_SIZE);
        
    m_DaFilterBuf[ENUM_ADAPTOR].AdValue = DATASAMP_MidValueFilter(m_DaAdcValueBuf[ENUM_ADAPTOR],
                                                            m_DaFilterBuf[ENUM_ADAPTOR].Buf,
                                                            DEF_ADAPTOR_FITER_SIZE);        

    m_DaFilterBuf[ENUM_PAW].AdValue = DATASAMP_MidValueFilter(m_DaAdcValueBuf[ENUM_PAW],
                                                            m_DaFilterBuf[ENUM_PAW].Buf,
                                                            DEF_PAW_FITER_SIZE);
    /*      获取电压值       */    
    m_DaFilterBuf[ENUM_O2].Voltage = DATASAMP_CalVoltage(m_DaFilterBuf[ENUM_O2].AdValue)* 2.0;
    
    m_DaFilterBuf[ENUM_BAT].Voltage = DATASAMP_CalVoltage(m_DaFilterBuf[ENUM_BAT].AdValue)* 2.0;
    
    m_DaFilterBuf[ENUM_ADAPTOR].Voltage = DATASAMP_CalVoltage(m_DaFilterBuf[ENUM_ADAPTOR].AdValue)* 2.8;
    
    m_DaFilterBuf[ENUM_PAW].Voltage = DATASAMP_CalVoltage(m_DaFilterBuf[ENUM_PAW].AdValue)* 2.0;
    
    DATASAMP_Calib();
}
/*
********************************************************************************
* 函数名称: DATASAMP_AppAdjustO2
* 函数功能: 计算校准参数相关函数Y=K*X+B中的K、B值
* 入口参数: x1,x2,y1,y2   x1,x2代表某浓度对应的AD值
* 出口参数: 曲线参数
* 返 回 值: void
* 日    期: 2013-09-26
* 修改问题:
* 修改内容:
* 注    意: 
********************************************************************************
*/
void  DATASAMP_AppAdjustO2(FP32 *k, FP32 *b, INT16U x1,INT16U x2,INT16U y1,INT16U y2)
{ 
	*k=( (FP32)(y2-y1) / (FP32)(x2-x1) );  //添加(FP32)by zj on 2015.11.12
	*b=( (FP32)(x2*y1-x1*y2) / (FP32)(x2-x1));
}
/*
********************************************************************************
* 函数名称: DATASAMP_AppCalO2
* 函数功能: 计算氧浓度值
* 入口参数: 
* 出口参数: 曲线参数
* 返 回 值: void
* 日    期: 2013-09-26
* 修改问题:
* 修改内容:
* 注    意: 
********************************************************************************
*/
INT8U DATASAMP_AppCalO2(INT16U AD_CAL_O2)
{
    INT16U O2_show=0;
    
    m_DaO2Value=((FP32)AD_CAL_O2 - m_DaBO2)/m_DaKO2;
    /*      对计算出的氧浓度值进行四舍五入     */
    O2_show=(INT16U)(m_DaO2Value +0.5);
    
    if(O2_show >= 100) 
    {      
      if(O2_show == 65535)
      {
        O2_show = 0;
      }
      else
      {
        O2_show =100;
      }
    }
    else 
    {
      if(O2_show <= 0) 
      {
        O2_show = 0;
      }
    }
    
    return O2_show;
}
/*
********************************************************************************
* 函数名称: DATASAMP_AppCalPaw
* 函数功能: 计算气道压压力值
* 入口参数: 
* 出口参数: 曲线参数
* 返 回 值: void
* 日    期: 2013-09-26
* 修改问题:
* 修改内容:
* 注    意: 
********************************************************************************
*/
INT8U DATASAMP_AppCalPaw(INT16U AD_CAL_Paw)
{
    s8 paw_show=0;
    
    m_DaPawValue=((FP32)AD_CAL_Paw - m_DaBPAW)/m_DaKPAW;
    paw_show=(s8)(m_DaPawValue +0.5);
    
    if(paw_show >= 12)
    {
      paw_show = 12;
    }
    
    if(paw_show < 0)
    {
      paw_show = 0;
    }
    
    return paw_show;
}
/*
******************************************************************************
函数名称:   DATASAMP_GetVoltage
函数功能:   获取电压值
输入参数:   type_id:获取电压类型的id
输出参数:   none
返 回 值:   电源电压值
创建日期:   2015-09-08
注    意:   none
*******************************************************************************
*/
FP32 DATASAMP_GetVoltage(INT8U type_id)
{
	return m_DaFilterBuf[type_id].Voltage;
}
/*
******************************************************************************
函数名称:   DATASAMP_AdValue
函数功能:   获取AD值
输入参数:   type_id:获取电压类型的id
输出参数:   none
返 回 值:   电源电压值
创建日期:   2015-09-08
注    意:   none
*******************************************************************************
*/
INT16U DATASAMP_AdValue(INT8U type_id)
{
	return m_DaFilterBuf[type_id].AdValue;
}
