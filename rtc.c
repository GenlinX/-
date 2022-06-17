#include "rtc.h"
#define BKP_DR 0x0004

u16 TmpSpeed;
u8 TmpGradientFlag;
RGBColor_TypeDef Tmpcolor, TmpBACKGD;
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
void Rtc_Tim_Init(void)
{

	RTC_InitTypeDef RTC_InitStruct;
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;

	// 1��ʹ��PWRʱ��,���Դ�Ĵ����й�
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	// 2��ʹ�ܺ󱸼Ĵ������ʣ���һ�ֵ��籣�����ݵļĴ���
	PWR_BackupAccessCmd(ENABLE);

	//�жϺ󱸼Ĵ�����ֵ
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != BKP_DR)
	{
		// 3������RTCʱ��Դ��ʹ��RTCʱ�ӣ�
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		//���ʹ��LSE��Ҫ��LSE��
		RCC_LSEConfig(RCC_LSE_ON);
		//��ʱһ��ʱ�䣬�ȴ�ʱ��Դ�ȶ�
		delay_ms(50);

		RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24; // 24Сʱ��
		RTC_InitStruct.RTC_AsynchPrediv = 0x7F;			   //�첽��Ƶ 128��Ƶ
		RTC_InitStruct.RTC_SynchPrediv = 0xFF;			   //ͬƵ��Ƶ 255��Ƶ
		// 4����ʼ��RTC(ͬ��/�첽��Ƶϵ����ʱ�Ӹ�ʽ)��
		RTC_Init(&RTC_InitStruct);

		RTC_TimeStruct.RTC_H12 = RTC_H12_PM; //���� 24Сʱ�ƣ���д�ɲ�д
		RTC_TimeStruct.RTC_Hours = 21;		 //Сʱ
		RTC_TimeStruct.RTC_Minutes = 1;		 //��
		RTC_TimeStruct.RTC_Seconds = 50;	 // ADC+
		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);

		RTC_DateStruct.RTC_Year = 22;					 //���
		RTC_DateStruct.RTC_Month = 5;					 //�·�
		RTC_DateStruct.RTC_Date = 16;					 //��
		RTC_DateStruct.RTC_WeekDay = RTC_Weekday_Monday; //����
		RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);

		//��ֵд��󱸼Ĵ���
		RTC_WriteBackupRegister(RTC_BKP_DR0, BKP_DR);
	}
}

//����A��ʼ��
void RTC_Alarm_AInit(u8 data, u8 hours, u8 minutes, u8 seconds)
{
	RTC_AlarmTypeDef RTC_AlarmStruct;
	RTC_TimeTypeDef RTC_AlarmTime;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	//�ر����ӣ�
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

	RTC_AlarmTime.RTC_H12 = RTC_H12_PM;	 //���� 24Сʱ�ƣ���д�ɲ�д
	RTC_AlarmTime.RTC_Hours = hours;	 //Сʱ
	RTC_AlarmTime.RTC_Minutes = minutes; //��
	RTC_AlarmTime.RTC_Seconds = seconds; // ADC+

	RTC_AlarmStruct.RTC_AlarmTime = RTC_AlarmTime;							//����ʱ��
	RTC_AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_None;						//�����룬��ʵ�����õ�ʱ����Ӧ����
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date; //��������Ӧ����
	RTC_AlarmStruct.RTC_AlarmDateWeekDay = data;							// 16��
	//�������Ӳ�����
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStruct);

	//�������ӣ�
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
	//�������������жϣ�
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	EXTI_InitStruct.EXTI_Line = EXTI_Line17;			//�ж���17
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;	//�ж�ģʽ
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;				//�ж���ʹ��
														//��ʼ�������жϣ����ô��������ȡ�
	EXTI_Init(&EXTI_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = RTC_Alarm_IRQn;	   //�ж�ͨ�����ж�ͨ����stm32f4xx.h�ļ����в���
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2; //��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;		   //��Ӧ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;		   //ʹ��ͨ��
														   //�����жϷ��飨NVIC������ʹ���жϡ�
	NVIC_Init(&NVIC_InitStruct);

	// TmpSpeed = speed;
	// TmpGradientFlag = GradientFlag;
	// Tmpcolor=color, TmpBACKGD=BACKGD;

	// speed=0;
	// GradientFlag=0;
	// color=RED;
	// BACKGD=BLACK;
}

//�����жϷ�����
void RTC_Alarm_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line17) == SET)
	{
		//�ж��Ƿ�Ϊ����A
		if (RTC_GetFlagStatus(RTC_FLAG_ALRAF) == SET)
		{
			AlarmLight();
			// speed=TmpSpeed;
			// GradientFlag=TmpGradientFlag;
			// color=Tmpcolor;
			// BACKGD=TmpBACKGD;
		}
		RTC_ClearFlag(RTC_FLAG_ALRAF);
	}
	//��ձ�־λ
	EXTI_ClearITPendingBit(EXTI_Line17);
}
