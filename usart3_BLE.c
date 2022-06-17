#include "usart3_BLE.h"

u8 buffer[datalen - 3] = {0};
u8 rx_buffer[21] = {0};
u8 ch_buffer[21]={0};
u8 count = 0;

/*****************************************
����˵��
PB10  ---- USART3_TX(���Ͷ�)
PB11  ---- USART3_RX(���ն�)

USART3����APB1��
******************************************/
void Usart3_Init(int MyBaudRate)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    //����ʱ��ʹ�ܣ�GPIO ʱ��ʹ�ܡ�
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    //ʹ��GPIOBʱ�ӣ�
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    // GPIOB10����ӳ�䵽USART3 ����ѡ�����õĹ���
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    // GPIOB11����ӳ�䵽USART3 ����ѡ�����õĹ���
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

    // GPIO ��ʼ�����ã�Ҫ����ģʽΪ���ù��ܡ�
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //����10 11
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;             //����ģʽ
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;           //����ʽ
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;             //����
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;        // 50MHZ
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    USART_InitStruct.USART_BaudRate = MyBaudRate;                                //������
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                 //ȫ˫��ģʽ
    USART_InitStruct.USART_Parity = USART_Parity_No;                             //����żУ��λ
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;                     //����λ
    USART_InitStruct.USART_StopBits = USART_StopBits_1;                          //ֹͣλ
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ��������
    //���ڲ�����ʼ�������ò����ʣ��ֳ�����żУ��Ȳ�����
    USART_Init(USART3, &USART_InitStruct);

    //�����жϲ��ҳ�ʼ�� NVIC��
    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;         //�ж�ͨ�����ж�ͨ����stm32f4xx.h�ļ����в���
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ�
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;        //��Ӧ���ȼ�
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;           //ʹ��ͨ��
                                                           // 3������ʱ���жϣ�����NVIC��
    NVIC_Init(&NVIC_InitStruct);

    //����Ϊ�����жϣ���ʾ�����ݹ�����CPUҪ�жϽ��н��գ�
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    //ʹ�ܴ��ڡ�
    USART_Cmd(USART3, ENABLE);
}

void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {

        //��ս��ܱ�־λ
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);

        rx_buffer[count++] = USART_ReceiveData(USART3);
        
        if (count >= 21) //��ֹԽ�磬count��ֵ���Ϊ20
        {
            count = 0;
            memset(rx_buffer,0,sizeof(rx_buffer));
        }
        //printf("%d\r\n",sizeof(rx_buffer));

        if ((rx_buffer[count - 1] == 0x5A) ) //�ж��Ƿ������ݰ�β�������յ������ݰ�
        {
            printf("recieve package\r\n");
            u8 bytesum=0;
            for(u8 i=0;i<(datalen-3);i++)
            {
                bytesum+=rx_buffer[i+1];
            }
            if(bytesum==rx_buffer[count-2])
            {
                for (u8 i = 0; i < datalen - 3; i++)
                {
                    buffer[i] = rx_buffer[i + 1];
                }
                memset(rx_buffer, 0, sizeof(rx_buffer));
                count = 0;

                rx_flag = 1;
                u8 i = 0;
                cur_switch = buffer[i++]; // 0:����
                if (buffer[i++] <= (NOM - 1))
                {
                    cur_mtrid = buffer[i - 1];
                    // printf("%d\r\n",cur_mtrid);
                }
                else
                {
                    cur_mtrid = 0; //�ز�ID
                                   // printf("%d\r\n",cur_mtrid);
                }
                if (buffer[i++] == 0x81)
                {
                    color = BLACK;
                }
                else
                {
                    color = Colourful_Wheel(buffer[i - 1]); //ǰ��ɫ
                }
                if (buffer[i++] == 0x81)
                {
                    BACKGD = BLACK;
                }
                else
                {
                    BACKGD = Colourful_Wheel(buffer[i - 1]); //����ɫ
                }
                speed = buffer[i++];        //�ٶȣ�δ����
                cur_position = buffer[i++]; //λ�ã�δ����
                GradientFlag = buffer[i++]; //�Ƿ񽥱�
                ch_switch = buffer[i++];    //�Ƿ���ʾ�û��ַ�
                if (ch_switch == 1)
                {
                    tim_switch = 0;
                }
                else
                {
                    tim_switch = buffer[i]; //�Ƿ���ʾʱ��
                }

                if ((cur_switch != 0) && (cur_switch != 1))
                {
                    cur_switch = 1;
                    if (cur_mtrid > 10)
                    {
                        cur_mtrid = 0;
                    }
                }
                printf("switch:%X MtrID:%X color:%d %d %d BACKGD:%d %d %d SPEED:%X Position:%X CH��%X TIM:%X\r\n", cur_switch, cur_mtrid, color.R, color.G, color.B, BACKGD.R, BACKGD.G, BACKGD.B, speed, cur_position, ch_switch, tim_switch);
                rx_flag = 0;
            }
            else
            {
                count=0;
                memset(rx_buffer,0,sizeof(rx_buffer));
            }
            
        }
        else if ((rx_buffer[count - 2] == 0x5C) && (rx_buffer[count-1] ==';')) 
        {

            ch_flag=0;
            //printf("count=%d\r\n",count);
            MtrTotalColumn[NOM+1] = (count -2) * 8; //�ַ��ܸ���Ϊcount-2,��¼�û������زĵ�������
            printf("CH:");
            for(u8 i=0;i<(count-2);i++)
            {
                ch_buffer[i]=rx_buffer[i];
                printf("%c",rx_buffer[i]);
            }
            ch_buffer[20]=count-2;
           
            printf("\r\n");
            count=0;
            memset(rx_buffer,0,sizeof(rx_buffer));
            ch_flag=1;
        }
        
    }
}
