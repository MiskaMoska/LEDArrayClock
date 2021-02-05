#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"

#define Col0  PCout(11) 
#define Col1  PCout(7) 
#define Col2  PCout(6) 
#define Col3  PDout(2) 
#define Col4  PBout(15) 
#define Col5  PCout(12) 
#define Col6  PCout(10) 
#define Col7  PAout(15) 

#define Col8  PBout(5) 
#define Col9  PCout(5) 
#define Col10 PCout(4) 
#define Col11 PBout(7) 
#define Col12 PAout(7) 
#define Col13 PBout(6) 
#define Col14 PBout(4) 
#define Col15 PBout(3)

#define Col16 PCout(2) 
#define Col17 PAout(6) 
#define Col18 PAout(5) 
#define Col19 PCout(0) 
#define Col20 PAout(4) 
#define Col21 PCout(1) 
#define Col22 PCout(3) 
#define Col23 PAout(0) 

extern u32 ColData_time[8];
extern u32 ColData_tptr[8];
extern u32 ColData_chng[8];
extern u32 ColData_trans_021[8];
extern u32 ColData_trans_120[8];
extern u32 ColData_InitImg[8];
extern u32 ColData_trans_520[8]; 
extern u32 ColData_mb[8]; //秒表模式下的行码

extern u16 mb_mtens,mb_munit,mb_stens,mb_sunit; //秒表的分钟十位、个位和秒钟十位、个位
extern u8  mb_run; //秒表运行标志
extern u16 mb_time; //计时值

extern u16 t;
extern u8  Mode;
extern u8  block;
extern u8  flash;
extern u16 mb_ms;

void TIM3_Int_Init(u16 arr,u16 psc);
#endif
