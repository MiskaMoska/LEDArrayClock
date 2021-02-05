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
					0x00020204};//row7 С��

u16 htens,hunit,mtens,munit; //ʱ�䣬Сʱʮλ����λ�ͷ���ʮλ����λ
u16 mb_mtens,mb_munit,mb_stens,mb_sunit; //���ķ���ʮλ����λ������ʮλ����λ
u16 htens_reg,hunit_reg,mtens_reg,munit_reg; //�޸�ģʽ�µļĴ�ֵ
u16 temptens,tempunit,tempfbrc; //�¶ȣ�ʮλ����λ��ʮ��λ

u32 ColData_time[8]; //ʱ�������
u32 ColData_tptr[8]; //�¶ȵ�����
u32 ColData_chng[8]; //�޸�ֵ������
u32 ColData_trans_021[8]; //ʱ��ת�¶ȹ��̵�����
u32 ColData_trans_120[8]; //�¶�תʱ����̵�����
u32 ColData_InitImg[8]; //��ʼ���������
u32 ColData_trans_520[8]; //���ƻ���תʱ����̵�����
u32 ColData_mb[8]; //���ģʽ�µ�����

/*---------------------
	��������Mode
ָʾϵͳ��ǰ����ģʽ

0: ������ʱ
1: ��ʾ�¶�
2: time2tptr���ɹ���
3: tptr2time���ɹ���
4: �޸�ʱ��
5: �������棨InitImg��
6: InitImg2time���ɹ���
7: ���ģʽ
---------------------*/
u8 Mode; 

/*---------------------
	��������DigBit
ָʾ��ǰ�����޸���һλ

0: ʱʮλ
1: ʱ��λ
2: ��ʮλ
3: �ָ�λ
---------------------*/
u8 DigBit; 

u8 mb_run; //������б�־
u16 mb_time; //��ʱֵ

int main(void)
{	
	u8 i;
	short temperature = 0;

	Mode  = 5; //��ʼʱ��ʾ��������
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ����� 2
	delay_init(); //��ʱ������ʼ��	  
	LED_Init();	//LED IO��ʼ��
	KEY_Init();	//���� IO��ʼ��
	TIM3_Int_Init(100-1,720-1); //��ʱ�жϳ�ʼ����ÿ��1ms�ж�
	RTC_Init(); //RTC��ʼ��
	DS18B20_Init(); //�¶ȴ�������ʼ��

	while(1)
	{	
		//�������
		switch(KEY_Scan(0))
		{
			case KEY0_PRES: //0������
			{
				if(!Mode) Mode = 2; //�������ģʽ
				else if(Mode == 1) Mode = 3; //�������ģʽ
				else if(Mode == 4) //��λ
				{
					DigBit++;
					DigBit &= 0x03;
				}
				else if(Mode == 7) //���ģʽ�£��ü�������ͣ�����
				{
					if(mb_run) mb_run = 0;
					else mb_run = 1;
				}
			}break;
			case KEY1_PRES: //1������
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
				else if(Mode == 7) //���ģʽ��,���㲢��ͣ
				{
					mb_time = 0;
					mb_ms   = 0;
					mb_run  = 0;
				}
			}break;
			case KEY2_PRES: //2������
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
					Mode = 7; //�������ģʽ
				}
				else if(Mode == 7)
				{	
					Mode = 0; //�ص���ʱģʽ
					mb_time = 0; //�������ֵ
					mb_run = 0; //ֹͣ����
				}
			}break;
			default: break;
		}

		//���ݲ�ͬģʽ�����²�ͬ������
		switch(Mode)
		{
			case 0: //ʱ����ʾģʽ
			{
				for(i=0;i<8;i++)
				{	
					if(((i == 2)||(i == 3)||(i == 5)||(i == 6)) && (t < 500)) //��ʾ��˸����ָʾ��
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
			case 1: //�¶���ʾģʽ
			{
				temperature = DS18B20_Get_Temp(); //��ȡ�¶�
				temptens = temperature / 100;
				tempunit = (temperature % 100) / 10;
				tempfbrc = (temperature % 100) % 10;
				for(i=0;i<8;i++)
				{
					if(i == 7) //��ʾС����
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
			case 2: //ʱ��ת�¶�ģʽ
			{	
				temperature = DS18B20_Get_Temp(); //��ȡ�¶�
				temptens = temperature / 100;
				tempunit = (temperature % 100) / 10;
				tempfbrc = (temperature % 100) % 10;
				for(i=0;i<8;i++) 
				{
					if(((i == 2)||(i == 3)||(i == 5)||(i == 6)) && (t < 500)) //ʱ������
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

					if(i == 7) //�¶�����
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

					ColData_trans_021[i] = (ColData_time[i] << block) | (ColData_tptr[i] >> (24 - block)); //����
				}
			}break;
			case 3: //�¶�תʱ��ģʽ
			{	
				temperature = DS18B20_Get_Temp(); //��ȡ�¶�
				temptens = temperature / 100;
				tempunit = (temperature % 100) / 10;
				tempfbrc = (temperature % 100) % 10;
				for(i=0;i<8;i++) 
				{
					if(((i == 2)||(i == 3)||(i == 5)||(i == 6)) && (t < 500)) //ʱ������
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

					if(i == 7) //�¶�����
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

					ColData_trans_120[i] = (ColData_tptr[i] << block) | (ColData_time[i] >> (24 - block)); //����
				}
			}break;
			case 4: //�޸�ʱ��ģʽ
			{
				for(i=0;i<8;i++) 
				{
					if((i == 2)||(i == 3)||(i == 5)||(i == 6)) //ʱ������
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

					if(((i == 2)||(i == 3)||(i == 5)||(i == 6)) && (t < 500)) //��ʾ��˸����ָʾ��
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

					ColData_trans_520[i] = (ColData_InitImg[i] << block) | (ColData_time[i] >> (24 - block)); //����
				}
			}break;
			case 7: //���ģʽ
			{
				for(i=0;i<8;i++) 
				{
					if(((i == 2)||(i == 3)||(i == 5)||(i == 6)) && ((!mb_run) || (mb_ms < 500))) //��ʾ��˸����ָʾ��
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

