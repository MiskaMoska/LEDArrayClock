#include "timer.h"
#include "led.h"

//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM3, //TIM2
		TIM_IT_Update ,
		ENABLE  //ʹ��
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
}

u16 t = 0;
u8 block = 0; //����ģʽ�µĹ�������
u8 flash = 0; //�޸�ʱ��ʱ����˸ָʾλ
u16 mb_ms = 0;

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{	
	static u8 n = 0;
	static u16 ms = 0;
	static u16 flash_ms = 0;
	static u16 init_ms = 0;

	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{	
		//����
		Col0 = 0;Col1 = 0;Col2 = 0;Col3 = 0;Col4 = 0;Col5 = 0;Col6 = 0;Col7 = 0;
		Col8 = 0;Col9 = 0;Col10 = 0;Col11 = 0;Col12 = 0;Col13 = 0;Col14 = 0;Col15 = 0;
		Col16 = 0;Col17 = 0;Col18 = 0;Col19 = 0;Col20 = 0;Col21 = 0;Col22 = 0;Col23 = 0;

		//����ѡ��������
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

		//���ݲ�ͬģʽ����ʾ��ͬ������
		switch(Mode)
		{
			case 0: //ʱ����ʾģʽ����ʾʱ������
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
			case 1: //�¶���ʾģʽ����ʾ�¶�����
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
			case 2: //ʱ��ת�¶�ģʽ����ʾ������
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
			case 3: //�¶�תʱ��ģʽ����ʾ������
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
			case 4: //�޸�ʱ��ģʽ����ʾ�Ĵ�ֵ��
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
			case 5: //��ʼ����ģʽ����ʾ������
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

		t++; //��ָʾ����˸��ʱ

		//������ʱ
		if((Mode == 2)||(Mode == 3)||(Mode == 6)) //����ģʽ��ʱ
		{
			if(ms == 50) //ÿ70ms����һ��
			{
				ms = 0;
				if(block == 23) //һ������24��
				{
					block = 0;

					//�����˻ع龲̬��ʾģʽ
					if(Mode == 2) Mode = 1; 
					else if((Mode == 3)||(Mode == 6)) Mode = 0;
				}
				else block++;
			}
		}
		else //�ǹ���ģʽ������������0
		{
			ms = 0;
			block = 0;
		}
		ms++;

		//�޸�λ��˸��ʱ
		flash_ms++;
		if(flash_ms == 400)
		{
			flash_ms = 0;
			if(!flash) flash = 1;
			else flash = 0;
		}

		//��ʼ����ʱ
		if(Mode == 5)
		{
			init_ms++;
			if(init_ms == 2000)
			{
				Mode = 6; //��ʼ����
			}
		}

		//����ʱ
		if(Mode == 7) //���ģʽ��
		{	
			if(mb_run) //���������
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
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}
