#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

//#define KEY0 PAin(8)   	
//#define KEY1 PCin(9)	 
//#define KEY2 PCin(8)	 
 

#define KEY0  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)//��ȡ����0
#define KEY1  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)//��ȡ����1
#define KEY2  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)//��ȡ����2 
 

#define KEY0_PRES	1		//KEY0  
#define KEY1_PRES	2		//KEY1 
#define KEY2_PRES	3		//KEY2 

void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8 mode);  	//����ɨ�躯��					    
#endif
