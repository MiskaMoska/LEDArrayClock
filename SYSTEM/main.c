#include "led.h"
#include "delay.h"
#include "sys.h"
#include "timer.h"
#include "rtc.h"
#include "ds18b20.h"
#include "key.h"

u8 Tab[] = {0x00,0x06,0x09,0x09,0x09,0x09,0x09,0x06, //0
			0x00,0x02,0x06,0x02,0x02,0x02,0x02,0x07, //1
			0x00,0x06,0x09,0x01,0x02,0x04,0x08,0x0f, //2
			0x00,0x06,0x09,0x01,0x02,0x01,0x09,0x06, //3
			0x00,0x01,0x03,0x05,0x09,0x0f,0x01,0x01, //4
			0x00,0x0f,0x08,0x0e,0x01,0x01,0x09,0x06, //5
			0x00,0x02,0x04,0x08,0x0e,0x09,0x09,0x06, //6
			0x00,0x0f,0x09,0x01,0x02,0x02,0x04,0x04, //7
			0x00,0x06,0x09,0x09,0x06,0x09,0x09,0x06, //8
			0x00,0x06,0x09,0x09,0x07,0x01,0x02,0x04};//9
		//  row0 row1 row2 row3 row4 row5 row6 row7	

u32 Tab_InitImg[]= {0x00020084, //row0
					0x00020084, //row1
					0x000203e4, //row2
					0x00020154, //row3
					0x00124154, //row4
					0x00222084, //row5
					0x0006014c, //row6
					0x00020204};//row7 小刘

u16 htens,hunit,mtens,munit; //时间，小时十位、个位和分钟十位、个位
u16 mb_mtens,mb_munit,mb_stens,mb_sunit; //秒表的分钟十位、个位和秒钟十位、个位
u16 htens_reg,hunit_reg,mtens_reg,munit_reg; //修改模式下的寄存值
u16 temptens,tempunit,tempfbrc; //温度，十位、个位和十分位

u32 ColData_time[8]; //时间的行码
u32 ColData_tptr[8]; //温度的行码
u32 ColData_chng[8]; //修改值的行码
u32 ColData_trans_021[8]; //时间转温度过程的行码
u32 ColData_trans_120[8]; //温度转时间过程的行码
u32 ColData_InitImg[8]; //初始画面的行码
u32 ColData_trans_520[8]; //开计画面转时间过程的行码
u32 ColData_mb[8]; //秒表模式下的行码

/*---------------------
	变量名：Mode
指示系统当前运行模式

0: 正常计时
1: 显示温度
2: time2tptr过渡过程
3: tptr2time过渡过程
4: 修改时间
5: 开机画面（InitImg）
6: InitImg2time过渡过程
7: 秒表模式
---------------------*/
u8 Mode; 

/*---------------------
	变量名：DigBit
指示当前正在修改哪一位

0: 时十位
1: 时个位
2: 分十位
3: 分个位
---------------------*/
u8 DigBit; 

u8 mb_run; //秒表运行标志
u16 mb_time; //计时值

int main(void)
{	
	u8 i;
	short temperature = 0;

	Mode  = 5; //开始时显示开机画面
	htens = 0;
	hunit = 0;
	mtens = 0;
	munit = 0;

	mb_mtens = 0;
	mb_munit = 0;
	mb_stens = 0;
	mb_sunit = 0;

	mb_run = 0;
	mb_time = 0;

	htens_reg = 0;
	hunit_reg = 0;
	mtens_reg = 0;
	munit_reg = 0;

	DigBit = 0;
	
	SystemInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组 2
	delay_init(); //延时函数初始化	  
	LED_Init();	//LED IO初始化
	KEY_Init();	//按键 IO初始化
	TIM3_Int_Init(100-1,720-1); //定时中断初始化，每隔1ms中断
	RTC_Init(); //RTC初始化
	DS18B20_Init(); //温度传感器初始化

	while(1)
	{	
		//按键检测
		switch(KEY_Scan(0))
		{
			case KEY0_PRES: //0键按下
			{
				if(!Mode) Mode = 2; //进入滚动模式
				else if(Mode == 1) Mode = 3; //进入滚动模式
				else if(Mode == 4) //换位
				{
					DigBit++;
					DigBit &= 0x03;
				}
				else if(Mode == 7) //秒表模式下，该键控制暂停或继续
				{
					if(mb_run) mb_run = 0;
					else mb_run = 1;
				}
			}break;
			case KEY1_PRES: //1键按下
			{
				if(!Mode) 
				{
					Mode = 4;
					htens_reg = htens;
					hunit_reg = hunit;
					mtens_reg = mtens;
					munit_reg = munit;
				}
				else if(Mode == 4) 
				{
					Mode = 0;
					DigBit = 0;
					RTC_Set(2020,0,0,(u8)(10*htens_reg + hunit_reg),(u8)(10*mtens_reg + munit_reg),0);
					htens = calendar.hour / 10;
					hunit = calendar.hour % 10;
					mtens = calendar.min / 10;
					munit = calendar.min % 10;
				}
				else if(Mode == 7) //秒表模式下,清零并暂停
				{
					mb_time = 0;
					mb_ms   = 0;
					mb_run  = 0;
				}
			}break;
			case KEY2_PRES: //2键按下
			{
				if(Mode == 4)
				{
					switch(DigBit)
					{
						case 0:
						{
							htens_reg ++; 
							if((hunit_reg > 3) && (htens_reg == 2)) htens_reg = 0;
							else if(htens_reg == 3) htens_reg = 0; 
						}break;
						case 1:
						{
							hunit_reg ++;
							if((htens_reg == 2) && (hunit_reg == 4)) hunit_reg = 0;
							else if(hunit_reg == 10) hunit_reg = 0;
						}break;
						case 2:
						{
							mtens_reg ++;
							if(mtens_reg == 6) mtens_reg = 0;
						}break;
						case 3:
						{
							munit_reg ++;
							if(munit_reg == 10) munit_reg = 0;
						}break;
						default: break;
					}
				}
				else if(!Mode)
				{
					Mode = 7; //进入秒表模式
				}
				else if(Mode == 7)
				{	
					Mode = 0; //回到计时模式
					mb_time = 0; //清零计数值
					mb_run = 0; //停止计数
				}
			}break;
			default: break;
		}

		//根据不同模式，更新不同的行码
		switch(Mode)
		{
			case 0: //时间显示模式
			{
				for(i=0;i<8;i++)
				{	
					if(((i == 2)||(i == 3)||(i == 5)||(i == 6)) && (t < 500)) //显示闪烁的秒指示灯
					{
						ColData_time[i]=(Tab[(htens << 3) + i] << 19) | 
										(Tab[(hunit << 3) + i] << 14) |
										(Tab[(mtens << 3) + i] <<  6) |
										(Tab[(munit << 3) + i] <<  1) | 3 << 11; 
					}
					else
					{
						ColData_time[i]=(Tab[(htens << 3) + i] << 19) | 
										(Tab[(hunit << 3) + i] << 14) |
										(Tab[(mtens << 3) + i] <<  6) |
										(Tab[(munit << 3) + i] <<  1); 
					}
				}
			}break;
			case 1: //温度显示模式
			{
				temperature = DS18B20_Get_Temp(); //获取温度
				temptens = temperature / 100;
				tempunit = (temperature % 100) / 10;
				tempfbrc = (temperature % 100) % 10;
				for(i=0;i<8;i++)
				{
					if(i == 7) //显示小数点
					{
						ColData_tptr[i]=(Tab[(temptens << 3) + i] << 16) | 
										(Tab[(tempunit << 3) + i] << 11) |
										(Tab[(tempfbrc << 3) + i] <<  4) | (1 << 9);
					}
					else
					{
						ColData_tptr[i]=(Tab[(temptens << 3) + i] << 16) | 
										(Tab[(tempunit << 3) + i] << 11) |
										(Tab[(tempfbrc << 3) + i] <<  4) ;
					}
					
				}
			}break;
			case 2: //时间转温度模式
			{	
				temperature = DS18B20_Get_Temp(); //获取温度
				temptens = temperature / 100;
				tempunit = (temperature % 100) / 10;
				tempfbrc = (temperature % 100) % 10;
				for(i=0;i<8;i++) 
				{
					if(((i == 2)||(i == 3)||(i == 5)||(i == 6)) && (t < 500)) //时间行码
					{
						ColData_time[i]=(Tab[(htens << 3) + i] << 19) | 
										(Tab[(hunit << 3) + i] << 14) |
										(Tab[(mtens << 3) + i] <<  6) |
										(Tab[(munit << 3) + i] <<  1) | 3 << 11; 
					}
					else
					{
						ColData_time[i]=(Tab[(htens << 3) + i] << 19) | 
										(Tab[(hunit << 3) + i] << 14) |
										(Tab[(mtens << 3) + i] <<  6) |
										(Tab[(munit << 3) + i] <<  1); 
					}

					if(i == 7) //温度行码
					{
						ColData_tptr[i]=(Tab[(temptens << 3) + i] << 16) | 
										(Tab[(tempunit << 3) + i] << 11) |
										(Tab[(tempfbrc << 3) + i] <<  4) | (1 << 9);
					}
					else
					{
						ColData_tptr[i]=(Tab[(temptens << 3) + i] << 16) | 
										(Tab[(tempunit << 3) + i] << 11) |
										(Tab[(tempfbrc << 3) + i] <<  4) ;
					}

					ColData_trans_021[i] = (ColData_time[i] << block) | (ColData_tptr[i] >> (24 - block)); //滚动
				}
			}break;
			case 3: //温度转时间模式
			{	
				temperature = DS18B20_Get_Temp(); //获取温度
				temptens = temperature / 100;
				tempunit = (temperature % 100) / 10;
				tempfbrc = (temperature % 100) % 10;
				for(i=0;i<8;i++) 
				{
					if(((i == 2)||(i == 3)||(i == 5)||(i == 6)) && (t < 500)) //时间行码
					{
						ColData_time[i]=(Tab[(htens << 3) + i] << 19) | 
										(Tab[(hunit << 3) + i] << 14) |
										(Tab[(mtens << 3) + i] <<  6) |
										(Tab[(munit << 3) + i] <<  1) | 3 << 11; 
					}
					else
					{
						ColData_time[i]=(Tab[(htens << 3) + i] << 19) | 
										(Tab[(hunit << 3) + i] << 14) |
										(Tab[(mtens << 3) + i] <<  6) |
										(Tab[(munit << 3) + i] <<  1); 
					}

					if(i == 7) //温度行码
					{
						ColData_tptr[i]=(Tab[(temptens << 3) + i] << 17) | 
										(Tab[(tempunit << 3) + i] << 12) |
										(Tab[(tempfbrc << 3) + i] <<  5) | (1 << 10);
					}
					else
					{
						ColData_tptr[i]=(Tab[(temptens << 3) + i] << 17) | 
										(Tab[(tempunit << 3) + i] << 12) |
										(Tab[(tempfbrc << 3) + i] <<  5) ;
					}

					ColData_trans_120[i] = (ColData_tptr[i] << block) | (ColData_time[i] >> (24 - block)); //滚动
				}
			}break;
			case 4: //修改时间模式
			{
				for(i=0;i<8;i++) 
				{
					if((i == 2)||(i == 3)||(i == 5)||(i == 6)) //时间行码
					{
						ColData_chng[i]=(((DigBit == 0) && flash ? 0 : Tab[(htens_reg << 3) + i]) << 19) | 
										(((DigBit == 1) && flash ? 0 : Tab[(hunit_reg << 3) + i]) << 14) |
										(((DigBit == 2) && flash ? 0 : Tab[(mtens_reg << 3) + i]) <<  6) |
										(((DigBit == 3) && flash ? 0 : Tab[(munit_reg << 3) + i]) <<  1) | 3 << 11; 
					}
					else
					{
						ColData_chng[i]=(((DigBit == 0) && flash ? 0 : Tab[(htens_reg << 3) + i]) << 19) | 
										(((DigBit == 1) && flash ? 0 : Tab[(hunit_reg << 3) + i]) << 14) |
										(((DigBit == 2) && flash ? 0 : Tab[(mtens_reg << 3) + i]) <<  6) |
										(((DigBit == 3) && flash ? 0 : Tab[(munit_reg << 3) + i]) <<  1);
					}
				}
			}break;
			case 5:
			{
				for(i=0;i<8;i++) 
				{
					ColData_InitImg[i] = Tab_InitImg[i];
				}
			}break;
			case 6:
			{
				for(i=0;i<8;i++) 
				{
					ColData_InitImg[i] = Tab_InitImg[i];

					if(((i == 2)||(i == 3)||(i == 5)||(i == 6)) && (t < 500)) //显示闪烁的秒指示灯
					{
						ColData_time[i]=(Tab[(htens << 3) + i] << 19) | 
										(Tab[(hunit << 3) + i] << 14) |
										(Tab[(mtens << 3) + i] <<  6) |
										(Tab[(munit << 3) + i] <<  1) | 3 << 11; 
					}
					else
					{
						ColData_time[i]=(Tab[(htens << 3) + i] << 19) | 
										(Tab[(hunit << 3) + i] << 14) |
										(Tab[(mtens << 3) + i] <<  6) |
										(Tab[(munit << 3) + i] <<  1); 
					}

					ColData_trans_520[i] = (ColData_InitImg[i] << block) | (ColData_time[i] >> (24 - block)); //滚动
				}
			}break;
			case 7: //秒表模式
			{
				for(i=0;i<8;i++) 
				{
					if(((i == 2)||(i == 3)||(i == 5)||(i == 6)) && ((!mb_run) || (mb_ms < 500))) //显示闪烁的秒指示灯
					{
						ColData_mb[i] = (Tab[(mb_mtens << 3) + i] << 19) | 
										(Tab[(mb_munit << 3) + i] << 14) |
										(Tab[(mb_stens << 3) + i] <<  6) |
										(Tab[(mb_sunit << 3) + i] <<  1) | 3 << 11; 
					}
					else
					{
						ColData_mb[i] = (Tab[(mb_mtens << 3) + i] << 19) | 
										(Tab[(mb_munit << 3) + i] << 14) |
										(Tab[(mb_stens << 3) + i] <<  6) |
										(Tab[(mb_sunit << 3) + i] <<  1); 
					}
				}
			}break;
			default: break;
		}
	}
}

