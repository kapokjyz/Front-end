/*
********************************************************************************
*                  �����а����Ƽ����޹�˾������                     
*                      Ambulanc Tech Co., Ltd                       
*            Copyright (C) 2009, All Rights Reserved                
*                                                                   
* �ļ�����: k096.h
* �ļ���ʶ:
* ժ    Ҫ: KD096064A2F01��������
*
* ��ǰ�汾: 1.0
* ��    ��: 
* �������: 2013-09-10
*
* ע    ��:                                                                 
********************************************************************************
* ȡ���汾: 
* ��    ��: 
* �������: 
* �޸�����: 
* �޸��ļ�: 
* �������: 
* ���������
* �������ݣ�
********************************************************************************
*/
#ifndef K096_H
#define K096_H
/*
********************************************************************************
*                            ����ͷ�ļ�
********************************************************************************
*/
#include <includes.h>
/*
********************************************************************************
*                            �궨��
********************************************************************************
*/
#define SYSCLK       24500000          // Internal oscillator frequency in Hz
typedef  unsigned char byte;
typedef  unsigned int word;

/*
********************************************************************************
*                            I/O�ڶ���
********************************************************************************
*/
#define    EnVp_L            GPIO_ResetBits(GPIOC,GPIO_Pin_9)
#define    EnVp_H            GPIO_SetBits(GPIOC,GPIO_Pin_9)

#define    CS_L              GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define    CS_H              GPIO_SetBits(GPIOB,GPIO_Pin_12)

#define    SCL_L             GPIO_ResetBits(GPIOB,GPIO_Pin_13)
#define    SCL_H             GPIO_SetBits(GPIOB,GPIO_Pin_13)

#define    MOSI_L            GPIO_ResetBits(GPIOB,GPIO_Pin_15)
#define    MOSI_H            GPIO_SetBits(GPIOB,GPIO_Pin_15)

#define    RST_L             GPIO_ResetBits(GPIOC,GPIO_Pin_7)
#define    RST_H             GPIO_SetBits(GPIOC,GPIO_Pin_7)

#define    DC_L             GPIO_ResetBits(GPIOC,GPIO_Pin_6)
#define    DC_H             GPIO_SetBits(GPIOC,GPIO_Pin_6)

// color definitions
#define	  BLACK	   0x0000
#define	  BLUE	   0x001F
#define	  RED 	   0xF800
#define	  GREEN    0x07E0
#define   CYAN	   0x07FF
#define   MAGENTA  0xF81F
#define   YELLOW   0xFFE0
#define   WHITE	   0xFFFF

/*
********************************************************************************
*                            ��������
********************************************************************************
*/
void delay(unsigned int t);
void write_command   (unsigned char idat);
void write_data (unsigned char ddat);
void send_color(unsigned char Color);
void LCD_Clear(void);
void addset(unsigned char x) ;
void initi_oled(void);
void display_w (void);


void showzifu(unsigned char x,  unsigned char value, unsigned char dcolor,  unsigned char bgcolor);


#endif