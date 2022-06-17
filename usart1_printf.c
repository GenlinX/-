#include "usart1_printf.h"

#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
int _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc����   printf ��һ����
int fputc(int ch, FILE *f)
{ 	
	USART_SendData(USART1,ch);  //ͨ�����ڷ�������
	//�ȴ����ݷ������
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);      
	return ch;
}


/*****************************************
����˵��
PA9  ---- USART1_TX(���Ͷ�)
PA10 ---- USART1_RX(���ն�)
******************************************/
void Usart1_Init(int MyBaudRate)
{
	
	GPIO_InitTypeDef  	GPIO_InitStruct;
	USART_InitTypeDef	USART_InitStruct;
	NVIC_InitTypeDef  NVIC_InitStruct;
	
	//����ʱ��ʹ�ܣ�GPIO ʱ��ʹ�ܡ�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    //ʹ��GPIOAʱ�ӣ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	
	
	//GPIOA9����ӳ�䵽USART1 ����ѡ�����õĹ���
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 	
	//GPIOA10����ӳ�䵽USART1 ����ѡ�����õĹ���
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 	
	
	//GPIO ��ʼ�����ã�Ҫ����ģʽΪ���ù��ܡ�
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_9|GPIO_Pin_10;		//����9 10
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_AF;		//����ģʽ
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;	//����ʽ
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_UP;		//����
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;	//50MHZ
	GPIO_Init(GPIOA,&GPIO_InitStruct);	


	USART_InitStruct.USART_BaudRate				= MyBaudRate;					//������
	USART_InitStruct.USART_Mode					= USART_Mode_Tx|USART_Mode_Rx;	//ȫ˫��ģʽ
	USART_InitStruct.USART_Parity				= USART_Parity_No;				//����żУ��λ
	USART_InitStruct.USART_WordLength			= USART_WordLength_8b;			//����λ
	USART_InitStruct.USART_StopBits				= USART_StopBits_1;				//ֹͣλ
	USART_InitStruct.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;//��Ӳ��������
	//���ڲ�����ʼ�������ò����ʣ��ֳ�����żУ��Ȳ�����
	USART_Init(USART1, &USART_InitStruct);
	
	//�����жϲ��ҳ�ʼ�� NVIC��
	NVIC_InitStruct.NVIC_IRQChannel						= USART1_IRQn; //�ж�ͨ�����ж�ͨ����stm32f4xx.h�ļ����в���
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority	= 0;		 //��ռ���ȼ�			
	NVIC_InitStruct.NVIC_IRQChannelSubPriority			= 0;		 //��Ӧ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelCmd					= ENABLE;	 //ʹ��ͨ��
	//3������ʱ���жϣ�����NVIC��
    NVIC_Init(&NVIC_InitStruct);		


	//����Ϊ�����жϣ���ʾ�����ݹ�����CPUҪ�жϽ��н��գ�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	
	
	
	//ʹ�ܴ��ڡ�
	USART_Cmd(USART1, ENABLE);
	printf("usart1 activated!\r\n");

}
