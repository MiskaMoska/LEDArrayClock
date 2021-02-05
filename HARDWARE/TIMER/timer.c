#include "timer.h"
#include "led.h"

//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM3, //TIM2
		TIM_IT_Update ,
		ENABLE  //使能
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
}

u16 t = 0;
u8 block = 0; //滚动模式下的滚动格数
u8 flash = 0; //修改时间时的闪烁指示位
u16 mb_ms = 0;

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{	
	static u8 n = 0;
	static u16 ms = 0;
	static u16 flash_ms = 0;
	static u16 init_ms = 0;

	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{	
		//消隐
		Col0 = 0;Col1 = 0;Col2 = 0;Col3 = 0;Col4 = 0;Col5 = 0;Col6 = 0;Col7 = 0;
		Col8 = 0;Col9 = 0;Col10 = 0;Col11 = 0;Col12 = 0;Col13 = 0;Col14 = 0;Col15 = 0;
		Col16 = 0;Col17 = 0;Col18 = 0;Col19 = 0;Col20 = 0;Col21 = 0;Col22 = 0;Col23 = 0;

		//依次选择所有行
		switch(n)
		{
			case 0:
			{RowSel2 = 0; RowSel1 = 0; RowSel0 = 0; break;}
			case 1:
			{RowSel2 = 0; RowSel1 = 0; RowSel0 = 1; break;}
			case 2:
			{RowSel2 = 0; RowSel1 = 1; RowSel0 = 0; break;}
			case 3:
			{RowSel2 = 0; RowSel1 = 1; RowSel0 = 1; break;}
			case 4:
			{RowSel2 = 1; RowSel1 = 0; RowSel0 = 0; break;}
			case 5:
			{RowSel2 = 1; RowSel1 = 0; RowSel0 = 1; break;}
			case 6:
			{RowSel2 = 1; RowSel1 = 1; RowSel0 = 0; break;}
			case 7:
			{RowSel2 = 1; RowSel1 = 1; RowSel0 = 1; break;}
			default:
			{RowSel2 = 0; RowSel1 = 0; RowSel0 = 0; break;}
		}

		//根据不同模式，显示不同的行码
		switch(Mode)
		{
			case 0: //时间显示模式，显示时间行码
			{
				Col23 = ColData_time[n];
				Col22 = ColData_time[n] >> 1; 
				Col21 = ColData_time[n] >> 2; 
				Col20 = ColData_time[n] >> 3; 
				Col19 = ColData_time[n] >> 4; 
				Col18 = ColData_time[n] >> 5; 
				Col17 = ColData_time[n] >> 6; 
				Col16 = ColData_time[n] >> 7; 
				Col15 = ColData_time[n] >> 8; 
				Col14 = ColData_time[n] >> 9; 
				Col13 = ColData_time[n] >> 10; 
				Col12 = ColData_time[n] >> 11; 
				Col11 = ColData_time[n] >> 12; 
				Col10 = ColData_time[n] >> 13; 
				Col9  = ColData_time[n] >> 14; 
				Col8  = ColData_time[n] >> 15; 
				Col7  = ColData_time[n] >> 16; 
				Col6  = ColData_time[n] >> 17; 
				Col5  = ColData_time[n] >> 18; 
				Col4  = ColData_time[n] >> 19; 
				Col3  = ColData_time[n] >> 20; 
				Col2  = ColData_time[n] >> 21; 
				Col1  = ColData_time[n] >> 22; 
				Col0  = ColData_time[n] >> 23;
			}break;
			case 1: //温度显示模式，显示温度行码
			{
				Col23 = ColData_tptr[n];
				Col22 = ColData_tptr[n] >> 1; 
				Col21 = ColData_tptr[n] >> 2; 
				Col20 = ColData_tptr[n] >> 3; 
				Col19 = ColData_tptr[n] >> 4; 
				Col18 = ColData_tptr[n] >> 5; 
				Col17 = ColData_tptr[n] >> 6; 
				Col16 = ColData_tptr[n] >> 7; 
				Col15 = ColData_tptr[n] >> 8; 
				Col14 = ColData_tptr[n] >> 9; 
				Col13 = ColData_tptr[n] >> 10; 
				Col12 = ColData_tptr[n] >> 11; 
				Col11 = ColData_tptr[n] >> 12; 
				Col10 = ColData_tptr[n] >> 13; 
				Col9  = ColData_tptr[n] >> 14; 
				Col8  = ColData_tptr[n] >> 15; 
				Col7  = ColData_tptr[n] >> 16; 
				Col6  = ColData_tptr[n] >> 17; 
				Col5  = ColData_tptr[n] >> 18; 
				Col4  = ColData_tptr[n] >> 19; 
				Col3  = ColData_tptr[n] >> 20; 
				Col2  = ColData_tptr[n] >> 21; 
				Col1  = ColData_tptr[n] >> 22; 
				Col0  = ColData_tptr[n] >> 23;
			}break;
			case 2: //时间转温度模式，显示滚动码
			{
				Col23 = ColData_trans_021[n];
				Col22 = ColData_trans_021[n] >> 1; 
				Col21 = ColData_trans_021[n] >> 2; 
				Col20 = ColData_trans_021[n] >> 3; 
				Col19 = ColData_trans_021[n] >> 4; 
				Col18 = ColData_trans_021[n] >> 5; 
				Col17 = ColData_trans_021[n] >> 6; 
				Col16 = ColData_trans_021[n] >> 7; 
				Col15 = ColData_trans_021[n] >> 8; 
				Col14 = ColData_trans_021[n] >> 9; 
				Col13 = ColData_trans_021[n] >> 10; 
				Col12 = ColData_trans_021[n] >> 11; 
				Col11 = ColData_trans_021[n] >> 12; 
				Col10 = ColData_trans_021[n] >> 13; 
				Col9  = ColData_trans_021[n] >> 14; 
				Col8  = ColData_trans_021[n] >> 15; 
				Col7  = ColData_trans_021[n] >> 16; 
				Col6  = ColData_trans_021[n] >> 17; 
				Col5  = ColData_trans_021[n] >> 18; 
				Col4  = ColData_trans_021[n] >> 19; 
				Col3  = ColData_trans_021[n] >> 20; 
				Col2  = ColData_trans_021[n] >> 21; 
				Col1  = ColData_trans_021[n] >> 22; 
				Col0  = ColData_trans_021[n] >> 23;
			}break;
			case 3: //温度转时间模式，显示滚动码
			{
				Col23 = ColData_trans_120[n];
				Col22 = ColData_trans_120[n] >> 1; 
				Col21 = ColData_trans_120[n] >> 2; 
				Col20 = ColData_trans_120[n] >> 3; 
				Col19 = ColData_trans_120[n] >> 4; 
				Col18 = ColData_trans_120[n] >> 5; 
				Col17 = ColData_trans_120[n] >> 6; 
				Col16 = ColData_trans_120[n] >> 7; 
				Col15 = ColData_trans_120[n] >> 8; 
				Col14 = ColData_trans_120[n] >> 9; 
				Col13 = ColData_trans_120[n] >> 10; 
				Col12 = ColData_trans_120[n] >> 11; 
				Col11 = ColData_trans_120[n] >> 12; 
				Col10 = ColData_trans_120[n] >> 13; 
				Col9  = ColData_trans_120[n] >> 14; 
				Col8  = ColData_trans_120[n] >> 15; 
				Col7  = ColData_trans_120[n] >> 16; 
				Col6  = ColData_trans_120[n] >> 17; 
				Col5  = ColData_trans_120[n] >> 18; 
				Col4  = ColData_trans_120[n] >> 19; 
				Col3  = ColData_trans_120[n] >> 20; 
				Col2  = ColData_trans_120[n] >> 21; 
				Col1  = ColData_trans_120[n] >> 22; 
				Col0  = ColData_trans_120[n] >> 23;
			}break;
			case 4: //修改时间模式，显示寄存值码
			{
				Col23 = ColData_chng[n];
				Col22 = ColData_chng[n] >> 1; 
				Col21 = ColData_chng[n] >> 2; 
				Col20 = ColData_chng[n] >> 3; 
				Col19 = ColData_chng[n] >> 4; 
				Col18 = ColData_chng[n] >> 5; 
				Col17 = ColData_chng[n] >> 6; 
				Col16 = ColData_chng[n] >> 7; 
				Col15 = ColData_chng[n] >> 8; 
				Col14 = ColData_chng[n] >> 9; 
				Col13 = ColData_chng[n] >> 10; 
				Col12 = ColData_chng[n] >> 11; 
				Col11 = ColData_chng[n] >> 12; 
				Col10 = ColData_chng[n] >> 13; 
				Col9  = ColData_chng[n] >> 14; 
				Col8  = ColData_chng[n] >> 15; 
				Col7  = ColData_chng[n] >> 16; 
				Col6  = ColData_chng[n] >> 17; 
				Col5  = ColData_chng[n] >> 18; 
				Col4  = ColData_chng[n] >> 19; 
				Col3  = ColData_chng[n] >> 20; 
				Col2  = ColData_chng[n] >> 21; 
				Col1  = ColData_chng[n] >> 22; 
				Col0  = ColData_chng[n] >> 23;
			}break;
			case 5: //初始化面模式，显示画面码
			{
				Col23 = ColData_InitImg[n];
				Col22 = ColData_InitImg[n] >> 1; 
				Col21 = ColData_InitImg[n] >> 2; 
				Col20 = ColData_InitImg[n] >> 3; 
				Col19 = ColData_InitImg[n] >> 4; 
				Col18 = ColData_InitImg[n] >> 5; 
				Col17 = ColData_InitImg[n] >> 6; 
				Col16 = ColData_InitImg[n] >> 7; 
				Col15 = ColData_InitImg[n] >> 8; 
				Col14 = ColData_InitImg[n] >> 9; 
				Col13 = ColData_InitImg[n] >> 10; 
				Col12 = ColData_InitImg[n] >> 11; 
				Col11 = ColData_InitImg[n] >> 12; 
				Col10 = ColData_InitImg[n] >> 13; 
				Col9  = ColData_InitImg[n] >> 14; 
				Col8  = ColData_InitImg[n] >> 15; 
				Col7  = ColData_InitImg[n] >> 16; 
				Col6  = ColData_InitImg[n] >> 17; 
				Col5  = ColData_InitImg[n] >> 18; 
				Col4  = ColData_InitImg[n] >> 19; 
				Col3  = ColData_InitImg[n] >> 20; 
				Col2  = ColData_InitImg[n] >> 21; 
				Col1  = ColData_InitImg[n] >> 22; 
				Col0  = ColData_InitImg[n] >> 23;
			}break;
			case 6: 
			{
				Col23 = ColData_trans_520[n];
				Col22 = ColData_trans_520[n] >> 1; 
				Col21 = ColData_trans_520[n] >> 2; 
				Col20 = ColData_trans_520[n] >> 3; 
				Col19 = ColData_trans_520[n] >> 4; 
				Col18 = ColData_trans_520[n] >> 5; 
				Col17 = ColData_trans_520[n] >> 6; 
				Col16 = ColData_trans_520[n] >> 7; 
				Col15 = ColData_trans_520[n] >> 8; 
				Col14 = ColData_trans_520[n] >> 9; 
				Col13 = ColData_trans_520[n] >> 10; 
				Col12 = ColData_trans_520[n] >> 11; 
				Col11 = ColData_trans_520[n] >> 12; 
				Col10 = ColData_trans_520[n] >> 13; 
				Col9  = ColData_trans_520[n] >> 14; 
				Col8  = ColData_trans_520[n] >> 15; 
				Col7  = ColData_trans_520[n] >> 16; 
				Col6  = ColData_trans_520[n] >> 17; 
				Col5  = ColData_trans_520[n] >> 18; 
				Col4  = ColData_trans_520[n] >> 19; 
				Col3  = ColData_trans_520[n] >> 20; 
				Col2  = ColData_trans_520[n] >> 21; 
				Col1  = ColData_trans_520[n] >> 22; 
				Col0  = ColData_trans_520[n] >> 23;
			}break;
			case 7: 
			{
				Col23 = ColData_mb[n];
				Col22 = ColData_mb[n] >> 1; 
				Col21 = ColData_mb[n] >> 2; 
				Col20 = ColData_mb[n] >> 3; 
				Col19 = ColData_mb[n] >> 4; 
				Col18 = ColData_mb[n] >> 5; 
				Col17 = ColData_mb[n] >> 6; 
				Col16 = ColData_mb[n] >> 7; 
				Col15 = ColData_mb[n] >> 8; 
				Col14 = ColData_mb[n] >> 9; 
				Col13 = ColData_mb[n] >> 10; 
				Col12 = ColData_mb[n] >> 11; 
				Col11 = ColData_mb[n] >> 12; 
				Col10 = ColData_mb[n] >> 13; 
				Col9  = ColData_mb[n] >> 14; 
				Col8  = ColData_mb[n] >> 15; 
				Col7  = ColData_mb[n] >> 16; 
				Col6  = ColData_mb[n] >> 17; 
				Col5  = ColData_mb[n] >> 18; 
				Col4  = ColData_mb[n] >> 19; 
				Col3  = ColData_mb[n] >> 20; 
				Col2  = ColData_mb[n] >> 21; 
				Col1  = ColData_mb[n] >> 22; 
				Col0  = ColData_mb[n] >> 23;
			}break;
			default: break;
		}
	
		n++;
		n &= 0x07;

		t++; //秒指示灯闪烁计时

		//滚动计时
		if((Mode == 2)||(Mode == 3)||(Mode == 6)) //滚动模式计时
		{
			if(ms == 50) //每70ms滚动一格
			{
				ms = 0;
				if(block == 23) //一共滚动24格
				{
					block = 0;

					//滚完了回归静态显示模式
					if(Mode == 2) Mode = 1; 
					else if((Mode == 3)||(Mode == 6)) Mode = 0;
				}
				else block++;
			}
		}
		else //非滚动模式下两个变量常0
		{
			ms = 0;
			block = 0;
		}
		ms++;

		//修改位闪烁计时
		flash_ms++;
		if(flash_ms == 400)
		{
			flash_ms = 0;
			if(!flash) flash = 1;
			else flash = 0;
		}

		//初始化计时
		if(Mode == 5)
		{
			init_ms++;
			if(init_ms == 2000)
			{
				Mode = 6; //开始滚动
			}
		}

		//秒表计时
		if(Mode == 7) //秒表模式下
		{	
			if(mb_run) //秒表在运行
			{
				mb_ms++;
				if(mb_ms >= 1000)
				{
					mb_ms = 0;
					mb_time ++;
				}
			}
			mb_mtens = mb_time / 600;
			mb_munit = (mb_time % 600) / 60;
			mb_stens = (mb_time % 60) / 10;
			mb_sunit = mb_time % 10;
		}

	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}
