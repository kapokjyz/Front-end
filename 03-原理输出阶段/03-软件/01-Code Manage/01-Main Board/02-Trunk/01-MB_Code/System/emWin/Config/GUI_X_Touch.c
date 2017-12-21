/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              礐/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUI_TOUCH_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include "TouchPanel.h"

void GUI_TOUCH_X_ActivateX(void) {
}

void GUI_TOUCH_X_ActivateY(void) {
}


int  GUI_TOUCH_X_MeasureX(void) 
{
    unsigned char t,t1,count;
    unsigned short int databuffer[10]={5,7,9,3,2,6,4,0,3,1};
    unsigned short temp,x;	

    /*      循环读数10次,求平均    */
    count = 0;
    while( count < 10 )
    {	   	  
        databuffer[count] = TOUCHPANEL_ReadX();
        count++; 
    }  
 
    do//将数据X升序排列
    {	
        t1 = 0;		  
        for( t=0; t<count-1;t++ )
        {
            if( databuffer[t] > databuffer[t+1] )
            {
                temp = databuffer[t+1];
                databuffer[t+1] = databuffer[t];
                databuffer[t] = temp;
                t1 = 1; 
            }  
        }
        
    }while(t1); 	
    
    x = (databuffer[3] + databuffer[4] + databuffer[5]) / 3;	
    
    return(x);  
}


int  GUI_TOUCH_X_MeasureY(void) {
    unsigned char t,t1,count;
    unsigned short int databuffer[10]={5,7,9,3,2,6,4,0,3,1};
    unsigned short temp,y;	

    /*      循环读数10次,求平均    */
    count = 0;
    while( count < 10 )
    {	   	  
        databuffer[count] = TOUCHPANEL_ReadY();
        count++;  
    }
    do//将数据X升序排列
    {	
        t1 = 0;		  
        for( t=0; t<count-1; t++ )
        {
            if( databuffer[t] > databuffer[t+1] )
            {
                temp=databuffer[t+1];
                databuffer[t+1] = databuffer[t];
                databuffer[t] = temp;
                t1 = 1; 
            }  
        }
    }while(t1); 	
    
    y = (databuffer[3] + databuffer[4] + databuffer[5]) / 3;	   

    return(y); 
}