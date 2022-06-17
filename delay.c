#include "delay.h"
u32 my_us = 21;         	 //1us�ɼ�21����
u32 my_ms = 21000;         //1us�ɼ�21000����

void Delay_Init(void)
{
	//ʱ��Դѡ��168MHZ/8 = 21MHZ
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //Systickʱ��Դѡ��  misc.c�ļ���

}

//nusȡֵ��Χ��0~798915
void delay_us(int nus)
{
	u32 temp = 0; 
	
	//�趨����ֵ�Ĵ���
	SysTick->LOAD = my_us*nus - 1;
	//���ü���Ϊ0
	SysTick->VAL = 0x00;

	//������ʱ��
	SysTick->CTRL |=(0x01<<0);
	
	do
	{
		temp = SysTick->CTRL;  //��ȡֵ���жϵ�16λ
			//�ж��Ƿ�ʹ�ܶ�ʱ��	//�жϵ�16�Ƿ�Ϊ1	
	}while( (temp & (0x01<<0)) && !(temp & (0x01<<16)));

	//�رն�ʱ��
	SysTick->CTRL &= ~(0x01<<0);
}

//nusȡֵ��Χ��0~798915
void delay_ms(int nms)
{
	u32 temp = 0; 
	
	//�趨����ֵ�Ĵ���
	SysTick->LOAD = my_ms*nms - 1;
	//���ü���Ϊ0
	SysTick->VAL = 0x00;

	//������ʱ��
	SysTick->CTRL |=(0x01<<0);
	
	do
	{
		temp = SysTick->CTRL;  //��ȡֵ���жϵ�16λ
			//�ж��Ƿ�ʹ�ܶ�ʱ��	//�жϵ�16�Ƿ�Ϊ1	
	}while( (temp & (0x01<<0)) && !(temp & (0x01<<16)));

	//�رն�ʱ��
	SysTick->CTRL &= ~(0x01<<0);
}

void delay_s(int ns)
{
	int i;
	for(i=0; i<ns; i++)
	{
		delay_ms(500);
		delay_ms(500);
	}
}
