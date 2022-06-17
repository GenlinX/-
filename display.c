#include "display.h"
RGBColor_TypeDef color;
RGBColor_TypeDef BACKGD;

u8 cur_switch = 0;
u8 pre_switch = 0;
u8 ch_switch=0;
u8 tim_switch=0;
u8 cur_mtrid;
u8 pre_mtrid = 255;
u8 pre_clock=1;
u8 cur_clock=0;
u8 speed;
u8 column;    //���ڼ�¼�زģ���
u8 CH_column; //���ڼ�¼�û��ַ�����
u8 totalcoloum;
u8 Gradient = 0;
u8 GradientFlag = 0;
u8 pre_position = 0;
u8 cur_position;
u8 rx_flag = 0;
u8 ch_flag = 0;
u8 time_flag = 0;
static u8 user_ch[288];

//��LED���ĵ�column�п�ʼ��ʾ�زģ�column��СֵΪ0
void DisplayFrom_LedColumn(u8 mtrx[], RGBColor_TypeDef color, RGBColor_TypeDef BACKGD, u8 column, u8 GradientFlag)
{
    //�������ܴ�����Ļ������
    if (column > (Pixel_S1_NUM / 16 - 1))
    {
        printf("Out of LedDColumn Range!\r\n");
        return;
    }

    u16 LedId;
    u16 BACKGDcount = column * 16; //����ʾ�زĵ�LED����
    //��ǰ�治��ʾ�زĵ�LED����Ϊ����ɫ
    for (LedId = 0; LedId < BACKGDcount; LedId++)
    {
        rgb_SetColor(LedId, BACKGD);
    }

    //�����ܹ��ж��ٸ�Ԫ��
    u16 size = MtrTotalColumn[cur_mtrid] * 2;

    //���㽫Ҫ��ʾ���ز���
    u16 EndMtrId;
    EndMtrId = (size < (Pixel_S1_NUM - BACKGDcount) / 8) ? size : ((Pixel_S1_NUM - BACKGDcount) / 8);

    // flag��0��������1������
    u8 flag = (column % 2 == 0) ? 0 : 1;
    //������װ��
    LedId = ParseAndLoad(mtrx, 0, EndMtrId, LedId, color, BACKGD, flag, GradientFlag);

    //��ʣ���LED���Ϊ����ɫ
    for (u16 i = LedId; i < Pixel_S1_NUM; i++)
    {
        rgb_SetColor(i, BACKGD);
    }

    //��ʼ���ݴ���
    rgb_SendArray();
}

//��LED���ĵ�0�п�ʼ�����زĵĵ�column�п�ʼ��ʾ��column����СֵΪ0
void DisplayFrom_MtrColumn(u8 mtrx[], RGBColor_TypeDef color, RGBColor_TypeDef BACKGD, u8 column, u8 GradientFlag)
{
    //�����ز��ܹ��ж��ٸ�Ԫ��
    u16 size = MtrTotalColumn[cur_mtrid] * 2;

    //����ӵڼ���Ԫ�ؿ�ʼ����
    u16 StartMtrId = column * 2; //ÿһ�а���2���ز�Ԫ��
    if (StartMtrId > size)
    {
        printf("MtrColumn Out of Range!\r\n");
        return;
    }

    //����ʣ��Ԫ�ظ���
    u16 RemainMtrCount = size - StartMtrId;

    //�������Ԫ��
    u16 EndMtrId = (RemainMtrCount * 8 >= Pixel_S1_NUM) ? (StartMtrId + Pixel_S1_NUM / 8) : (StartMtrId + RemainMtrCount);

    //װ�ز�����
    u16 LedId = 0;
    LedId = ParseAndLoad(mtrx, StartMtrId, EndMtrId, LedId, color, BACKGD, 0, GradientFlag);

    //��ʣ���LED���Ϊ����ɫ
    for (u16 i = LedId; i < Pixel_S1_NUM; i++)
    {
        rgb_SetColor(i, BACKGD);
    }
    rgb_SendArray();
}

//������װ��
u16 ParseAndLoad(u8 mtrx[], u16 StartMtrId, u16 EndMtrId, u16 LedId, RGBColor_TypeDef color, RGBColor_TypeDef BACKGD, u8 flag, u8 GradientFlag)
{
    for (u16 i = StartMtrId; i < EndMtrId; i = i + 2)
    {
        //��    ((i * 256 / Pixel_LEN) + j)&255)
        if (flag == 0)
        {
            for (u8 j = 0; j < 8; j++)
            {
                if (mtrx[i] & (0x01 << j))
                {
                    if (GradientFlag == 1)
                    {

                        rgb_SetColor(LedId++, Colourful_Wheel(((LedId * 256 / Pixel_S1_NUM)) & 255));
                    }
                    else
                    {
                        rgb_SetColor(LedId++, color);
                    }
                }
                else
                {
                    rgb_SetColor(LedId++, BACKGD);
                }
            }

            for (u8 j = 0; j < 8; j++)
            {
                if (mtrx[i + 1] & (0x01 << j))
                {
                    if (GradientFlag == 1)
                    {

                        rgb_SetColor(LedId++, Colourful_Wheel(((LedId * 256 / Pixel_S1_NUM)) & 255));
                    }
                    else
                    {
                        rgb_SetColor(LedId++, color);
                    }
                }
                else
                {
                    rgb_SetColor(LedId++, BACKGD);
                }
            }
        }
        //��
        else
        {

            for (u8 j = 0; j < 8; j++)
            {
                if (mtrx[i + 1] & (0x01 << (7 - j)))
                {
                    if (GradientFlag == 1)
                    {

                        rgb_SetColor(LedId++, Colourful_Wheel(((LedId * 256 / Pixel_S1_NUM)) & 255));
                    }
                    else
                    {
                        rgb_SetColor(LedId++, color);
                    }
                }
                else
                {
                    rgb_SetColor(LedId++, BACKGD);
                }
            }

            for (u8 j = 0; j < 8; j++)
            {
                if (mtrx[i] & (0x01 << (7 - j)))
                {
                    if (GradientFlag == 1)
                    {

                        rgb_SetColor(LedId++, Colourful_Wheel(((LedId * 256 / Pixel_S1_NUM)) & 255));
                    }
                    else
                    {
                        rgb_SetColor(LedId++, color);
                    }
                }
                else
                {
                    rgb_SetColor(LedId++, BACKGD);
                }
            }
        }
        flag = !flag;
    }
    return LedId;
}

void slide(u8 mtrid, RGBColor_TypeDef color, RGBColor_TypeDef BACKGD, u8 speed, u8 GradientFlag)
{
    cur_mtrid = mtrid;
    for (u8 i = Pixel_S1_NUM / 16 - 1; i > 0; i--)
    {
        DisplayFrom_LedColumn(MtrLib[cur_mtrid], color, BACKGD, i, GradientFlag);
        delay_ms(speed);
    }
    for (u8 i = 0; i < MtrTotalColumn[cur_mtrid]; i++)
    {
        DisplayFrom_MtrColumn(MtrLib[cur_mtrid], color, BACKGD, i, GradientFlag);
        delay_ms(speed);
    }
}

//������Ч��
void AlarmLight(void)
{
    u8 flag = 0, time = 35;
    RGBColor_TypeDef color;
    u16 i, j, k, n, value1, value2;

    //����
    for (n = 0; n < TIME; n++)
    {
        RGB_BLACK(Pixel_S1_NUM);
        rgb_SendArray();
    }
    //��˸
    for (n = 0; n < 4; n++)
    {
        value1 = (flag == 0) ? 0 : Pixel_S1_NUM / 2;
        value2 = (flag == 0) ? Pixel_S1_NUM / 2 : Pixel_S1_NUM;
        for (i = 0; i < 10; i++)
        {
            color = (i % 2 == 1) ? BLACK : ((flag == 0) ? RED : BLUE);
            for (k = 0; k < TIME; k++)
            {
                for (j = value1; j < value2; j++)
                {
                    rgb_SetColor(j, color);
                }
                rgb_SendArray();
            }
            delay_ms(time);
        }
        flag = !flag;
        delay_ms(1500);
    }
}

void display(void) //������ʾ��ͨ�ز�
{
    if (column < (Pixel_S1_NUM / 16))
    {
        // printf("����ģʽ\r\n");
        //����ģʽ
        column = Pixel_S1_NUM / 16 - column - 1; //�����һ�п�ʼ����
        DisplayFrom_LedColumn(MtrLib[cur_mtrid], color, BACKGD, column, GradientFlag);
    }
    else
    {
        //����ģʽ
        column -= Pixel_S1_NUM / 16; //��0�п�ʼ����
        DisplayFrom_MtrColumn(MtrLib[cur_mtrid], color, BACKGD, column, GradientFlag);
    }
}

void display2(void)
{

    if (CH_column < (Pixel_S1_NUM / 16))
    {
        CH_column = Pixel_S1_NUM / 16 - CH_column - 1; //�����һ�п�ʼ����
        DisplayFrom_LedColumn(user_ch, color, BACKGD, CH_column, GradientFlag);
    }
    else
    {
        CH_column -= Pixel_S1_NUM / 16; //��0�п�ʼ����
        DisplayFrom_MtrColumn(user_ch, color, BACKGD, CH_column, GradientFlag);
    }
}

void MCON(void) //�ز��Ƿ����ı�
{
    if (cur_mtrid != pre_mtrid) //�زķ����˸ı�
    {
        pre_mtrid = cur_mtrid;
        cur_position = (1 - (float)Pixel_S1_NUM / 16 / (MtrTotalColumn[cur_mtrid] + Pixel_S1_NUM / 8)) * 127; //λ���ƶ����ز��׸�
    }
    column = (1 - (float)cur_position / 127) * (MtrTotalColumn[cur_mtrid] + Pixel_S1_NUM / 8);
}

/*****************************************************************************
 * 0      1      2          3         4        5        6       7
 * ����   �ز�    ǰ��ɫ     ����ɫ    �ٶ�      λ��     ����     �ַ�
 *
 *
 ****************************************************************************/

void CH_display(void) //��ʾ�û������ַ�
{
    
    if (speed == 0)
    {
        if(cur_position != pre_position)
        {
            CH_column = (1 - (float)cur_position / 127) * (MtrTotalColumn[cur_mtrid] + Pixel_S1_NUM / 8);
        }
        
        display2();
    }
    else
    {
        if (cur_position != pre_position) //λ�÷����˸ı�
        {
            pre_position = cur_position;
            CH_column = (1 - (float)cur_position / 127) * (MtrTotalColumn[cur_mtrid] + Pixel_S1_NUM / 8);

            display2();
        }
        else
        {
            CH_column++;
            if (CH_column >= (MtrTotalColumn[cur_mtrid] + Pixel_S1_NUM / 16))
            {
                CH_column = 0;
            }
            u8 tmp_column = CH_column;
            display2();
            CH_column = tmp_column;
        }
        delay_ms(120 - speed);
    }
    
}

void MTR_display(void) //��ʾ�ز�
{
    if (speed == 0) //��ֹ
    {
        MCON();
        display();
    }
    else //����
    {
        if (cur_position != pre_position) //λ�÷����˸ı�
        {
            pre_position = cur_position;
            column = (1 - (float)cur_position / 127) * (MtrTotalColumn[cur_mtrid] + Pixel_S1_NUM / 8);
            MCON();
            display();
        }
        else
        {

            column++;
            if (column >= (MtrTotalColumn[cur_mtrid] + Pixel_S1_NUM / 16))
            {
                column = 0;
            }
            u8 tmp_column = column;
            display();
            column = tmp_column;
        }
        delay_ms(120 - speed);
    }
}


void TIM_display(void)
{
    u8 time_buffer[8];
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);

    time_buffer[0] = RTC_TimeStruct.RTC_Hours / 10 + '0'; //ʱ
    time_buffer[1] = RTC_TimeStruct.RTC_Hours % 10 + '0';
    
    if(RTC_TimeStruct.RTC_Seconds % 2==0)
    {
        time_buffer[2] = ':';
        time_buffer[5] = ':';
    }
    else
    {
        time_buffer[2]=' ';
        time_buffer[5] = ' ';
    }
    
    time_buffer[3] = RTC_TimeStruct.RTC_Minutes / 10 + '0'; //��
    time_buffer[4] = RTC_TimeStruct.RTC_Minutes % 10 + '0';
    
    time_buffer[6] = RTC_TimeStruct.RTC_Seconds / 10 + '0'; //��
    time_buffer[7] = RTC_TimeStruct.RTC_Seconds % 10 + '0';

    for(u8 i=0;i<128;i++)
    {
        user_ch[i] = character[(time_buffer[i / 16] - ' ') * 16 + (i % 16)];
    }
    
    //printf("show the time\r\n");
    cur_mtrid=NOM;
    CH_display();
    cur_mtrid = pre_mtrid;
    memset(user_ch,0,sizeof(user_ch));

}


void Function(void)
{

    if (rx_flag == 0) //��������ݰ���ת��
    {

        if (cur_switch == 0) //���ر��ر�
        {
            if (pre_switch == 1) //��ĻδϨ��
            {
                for (u8 i = 0; i < TIME; i++)
                {
                    RGB_BLACK(Pixel_S1_NUM); //Ϩ����Ļ
                }
                pre_switch = 0;
            }
        }
        else //���ر���
        {
            pre_switch = 1;
            cur_switch = 1;
            if (ch_switch == 1)
            {
                cur_mtrid = NOM+1;
                CH_display();
                cur_mtrid = pre_mtrid;
            }
            else if(tim_switch==1)
            {
                TIM_display();
            }
            else 
            {
                MTR_display();
            }


            //printf("mission complete\r\n");
        }
        

        
    }
    if(ch_flag==1)
    {
         for (u16 i = 0; i < ch_buffer[20] * 16; i++)
            {
                user_ch[i] = character[(ch_buffer[i / 16] - ' ') * 16 + (i % 16)];
            }
            ch_flag=0;
            printf("װ�����:%d\r\n",ch_buffer[20]);
    }
    
}

