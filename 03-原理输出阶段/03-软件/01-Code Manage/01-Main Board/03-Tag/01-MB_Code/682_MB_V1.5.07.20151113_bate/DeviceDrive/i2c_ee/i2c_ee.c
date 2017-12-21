/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : i2c_ee.c
* Author             : MCD Application Team
* Version            : V2.0.3
* Date               : 09/22/2008
* Description        : This file provides a set of functions needed to manage the
*                      communication between I2C peripheral and I2C M24C08 EEPROM.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "i2c_ee.h"
#include "Share.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define I2C_Speed              400000

#define I2C_PageSize           16

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void GPIO_Configuration(void);
static void I2C_Configuration(void);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configure the used I/O ports pin
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  /* Configure I2C2 pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : I2C_Configuration
* Description    : I2C Configuration
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void I2C_Configuration(void)
{
  I2C_InitTypeDef  I2C_InitStructure; 

  /* I2C2 Periph clock enable */
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE); 
  /* I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = EEPROM_DEVICE_ADDRESS;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;//modified by ivan 07/02/2010 
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
  
  /* I2C Peripheral Enable */
  I2C_Cmd(DEF_I2C_MODULE_USE, ENABLE);
  /* Apply I2C configuration after enabling it */
  I2C_Init(DEF_I2C_MODULE_USE, &I2C_InitStructure);
  
  //I2C_AcknowledgeConfig(I2C2, DISABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_Init
* Description    : Initializes peripherals used by the I2C EEPROM driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_Init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE); 
	   
  /* GPIO configuration */
  GPIO_Configuration();

  /* I2C configuration */
  I2C_Configuration();

}

/*******************************************************************************
* Function Name  : I2C_EE_BufferWrite
* Description    : Writes buffer of data to the I2C EEPROM.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_BufferWrite(INT16U DEVICE_ADDRESS,INT8U* pBuffer, INT16U WriteAddr, INT16U NumByteToWrite)
{

  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
 
  Addr = WriteAddr % I2C_PageSize;
  count = I2C_PageSize - Addr;
  NumOfPage =  NumByteToWrite / I2C_PageSize;
  NumOfSingle = NumByteToWrite % I2C_PageSize;

  I2C_EE_WaitEepromStandbyState();  // by zj on 2015.11.09
 
  /* If WriteAddr is I2C_PageSize aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      I2C_EE_PageWrite(DEVICE_ADDRESS, pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(DEVICE_ADDRESS, pBuffer, WriteAddr, I2C_PageSize); 
    	I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;
      }

      if(NumOfSingle!=0)
      {
        I2C_EE_PageWrite(DEVICE_ADDRESS, pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
      I2C_EE_PageWrite(DEVICE_ADDRESS, pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / I2C_PageSize;
      NumOfSingle = NumByteToWrite % I2C_PageSize;	
      
      if(count != 0)
      {  
        I2C_EE_PageWrite(DEVICE_ADDRESS, pBuffer, WriteAddr, count);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(DEVICE_ADDRESS, pBuffer, WriteAddr, I2C_PageSize);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;  
      }
      if(NumOfSingle != 0)
      {
        I2C_EE_PageWrite(DEVICE_ADDRESS, pBuffer, WriteAddr, NumOfSingle); 
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }
}

/*******************************************************************************
* Function Name  : I2C_EE_ByteWrite
* Description    : Writes one byte to the I2C EEPROM.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_ByteWrite(INT16U DEVICE_ADDRESS, INT8U* pBuffer, INT16U WriteAddr)
{  
  // add by ck 30/07/2009 /* While the bus is busy */
  while(I2C_GetFlagStatus(DEF_I2C_MODULE_USE, I2C_FLAG_BUSY));
    
  /* Send STRAT condition */
  I2C_GenerateSTART(DEF_I2C_MODULE_USE, ENABLE);

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(DEF_I2C_MODULE_USE, I2C_EVENT_MASTER_MODE_SELECT));  
  
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(DEF_I2C_MODULE_USE, DEVICE_ADDRESS, I2C_Direction_Transmitter);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(DEF_I2C_MODULE_USE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
     
  /* Send the EEPROM's internal address to write to */
  I2C_SendData(DEF_I2C_MODULE_USE, WriteAddr);
  
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(DEF_I2C_MODULE_USE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
   
  /* Send the byte to be written */
  I2C_SendData(DEF_I2C_MODULE_USE, *pBuffer); 
   
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(DEF_I2C_MODULE_USE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
     
  /* Send STOP condition */
  I2C_GenerateSTOP(DEF_I2C_MODULE_USE, ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_PageWrite
* Description    : Writes more than one byte to the EEPROM with a single WRITE
*                  cycle. The number of byte can't exceed the EEPROM page size.
* Input          : - pBuffer : pointer to the buffer containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_PageWrite(INT16U DEVICE_ADDRESS,INT8U* pBuffer, INT16U WriteAddr, INT8U NumByteToWrite)
{  
  /* While the bus is busy */
  while(I2C_GetFlagStatus(DEF_I2C_MODULE_USE, I2C_FLAG_BUSY));
  
  /* Send START condition */
  I2C_GenerateSTART(DEF_I2C_MODULE_USE, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(DEF_I2C_MODULE_USE, I2C_EVENT_MASTER_MODE_SELECT)); 
  
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(DEF_I2C_MODULE_USE, DEVICE_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(DEF_I2C_MODULE_USE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  

  /* Clear EV6 by setting again the PE bit */ // by zj on 2015.11.09
  //I2C_Cmd(DEF_I2C_MODULE_USE, ENABLE);
 
  /* Send the EEPROM's internal address to write to */
  I2C_SendData(DEF_I2C_MODULE_USE, WriteAddr);  
  
  /* Test on EV8 and clear it */
  while(! I2C_CheckEvent(DEF_I2C_MODULE_USE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* While there is data to be written */
  while(NumByteToWrite--)  
  {
    /* Send the current byte */
    I2C_SendData(DEF_I2C_MODULE_USE, *pBuffer); 

    /* Point to the next byte to be written */
    pBuffer++; 
  
    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(DEF_I2C_MODULE_USE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  }

  /* Send STOP condition */
  I2C_GenerateSTOP(DEF_I2C_MODULE_USE, ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_BufferRead
* Description    : Reads a block of data from the EEPROM.
* Input          : - pBuffer : pointer to the buffer that receives the data read 
*                    from the EEPROM.
*                  - ReadAddr : EEPROM's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_BufferRead(INT16U DEVICE_ADDRESS, INT8U* pBuffer, INT16U ReadAddr, INT16U NumByteToRead)
{  
  I2C_EE_WaitEepromStandbyState();  // by zj on 2015.11.09

    /* While the bus is busy */
  while(I2C_GetFlagStatus(DEF_I2C_MODULE_USE, I2C_FLAG_BUSY));
  
  /* Send START condition */
  I2C_GenerateSTART(DEF_I2C_MODULE_USE, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(DEF_I2C_MODULE_USE, I2C_EVENT_MASTER_MODE_SELECT));
   
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(DEF_I2C_MODULE_USE, DEVICE_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(DEF_I2C_MODULE_USE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));// ACK
  
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(DEF_I2C_MODULE_USE, ENABLE);

  /* Send the EEPROM's internal address to write to */
  I2C_SendData(DEF_I2C_MODULE_USE, ReadAddr);  

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(DEF_I2C_MODULE_USE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(DEF_I2C_MODULE_USE, ENABLE);
  
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(DEF_I2C_MODULE_USE, I2C_EVENT_MASTER_MODE_SELECT));
  
  /* Send EEPROM address for read */
  I2C_Send7bitAddress(DEF_I2C_MODULE_USE, DEVICE_ADDRESS, I2C_Direction_Receiver);
  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(DEF_I2C_MODULE_USE, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  
  /* While there is data to be read */
  I2C_AcknowledgeConfig(DEF_I2C_MODULE_USE, ENABLE);//add by ivan 07/02/2010 
   while(NumByteToRead)  
  {
    if(NumByteToRead == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(DEF_I2C_MODULE_USE, DISABLE);
      
      /* Send STOP Condition */
      I2C_GenerateSTOP(DEF_I2C_MODULE_USE, ENABLE);
    }

    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(DEF_I2C_MODULE_USE, I2C_EVENT_MASTER_BYTE_RECEIVED))  
    {      
      /* Read a byte from the EEPROM */
      *pBuffer = I2C_ReceiveData(DEF_I2C_MODULE_USE);

      /* Point to the next location where the byte read will be saved */
      pBuffer++; 
      
      /* Decrement the read bytes counter */
      NumByteToRead--;        
    }   
  }
  /* Enable Acknowledgement to be ready for another reception */
 // I2C_AcknowledgeConfig(I2C2, ENABLE);
}

/*******************************************************************************
* Function Name  : I2C_EE_WaitEepromStandbyState
* Description    : Wait for EEPROM Standby state
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_WaitEepromStandbyState(void)      
{
  vu16 SR1_Tmp = 0;

  do
  {
    /* Send START condition */
    I2C_GenerateSTART(DEF_I2C_MODULE_USE, ENABLE);
    /* Read I2C2 SR1 register */
    SR1_Tmp = I2C_ReadRegister(DEF_I2C_MODULE_USE, I2C_Register_SR1);
    /* Send EEPROM address for write */
    I2C_Send7bitAddress(DEF_I2C_MODULE_USE, EEPROM_Block0_ADDRESS, I2C_Direction_Transmitter);
  }while(!(I2C_ReadRegister(DEF_I2C_MODULE_USE, I2C_Register_SR1) & 0x0002));
  
  /* Clear AF flag */
  I2C_ClearFlag(DEF_I2C_MODULE_USE, I2C_FLAG_AF);
  
  /* STOP condition */    
  I2C_GenerateSTOP(DEF_I2C_MODULE_USE, ENABLE);  
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
