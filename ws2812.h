/* Includes ------------------------------------------------------------------*/
#ifndef __WS2812_H
#define __WS2812_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "delay.h"
#include "usart1_printf.h"

#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOSʹ��		  
#include "task.h"
#endif

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  u8 R;
  u8 G;
  u8 B;
} RGBColor_TypeDef;

extern u8 pixelBuffer[][24];
extern const RGBColor_TypeDef RED ;
extern const RGBColor_TypeDef GREEN;
extern const RGBColor_TypeDef BLUE;
extern const RGBColor_TypeDef SKY;
extern const RGBColor_TypeDef MAGENTA ;
extern const RGBColor_TypeDef YELLOW ;
extern const RGBColor_TypeDef ORANGE;
extern const RGBColor_TypeDef BLACK;
extern const RGBColor_TypeDef WHITE;
extern const RGBColor_TypeDef PURPLE;
/* Exported constants --------------------------------------------------------*/
#define Pixel_S1_NUM 512//���� RGB����
#define TIME 8
/**************************************
 ˵����

 WS2812B����Э��(��λ��ns)��
       min     typ     max
 T     1200
 T0H:  200     300     400
 T0L:  800     900      -
 T1H:  650     900     1000
 T1L:  200     300      -
 Trst: 200      -       -

 ʱ�����ڵ���СֵΪ��1200/8=150ns
 ��ˣ�8��ƵAPB1��42MHz/8 = 5.25MHz
 ʱ������Ϊ��1/5.25/1e6 = 1.90e-7=190ns
**************************************/

#define CODE0 0xC0 // 0��,192 ���͵�ʱ�� 1100 0000  ���ݲ�ͬ��SCK�ʵ�����
#define CODE1 0xFC // 1��,252 ���͵�ʱ�� 1111 1100

void detect(void);
void detect2(void);
void LED_Detect(void);
void LED_Detect2(void);
void LED_Detect3(void);
void WS2812b_Configuration(void);

/* Basic Color Effects -------------------------------------------------------*/
void rgb_SetColor(u16 LedId, RGBColor_TypeDef Color);
void RGB_RED(u16 Pixel_LEN);
void RGB_GREEN(u16 Pixel_LEN);
void RGB_BLUE(u16 Pixel_LEN);
void RGB_YELLOW(u16 Pixel_LEN);
void RGB_MAGENTA(u16 Pixel_LEN);
void RGB_BLACK(u16 Pixel_LEN);
void RGB_WHITE(u16 Pixel_LEN);
void RGB_SKY(u16 Pixel_LEN);
void RGB_ORANGE(u16 Pixel_LEN);
void RGB_PURPLE(u16 Pixel_LEN);
void rgb_SendArray(void);
RGBColor_TypeDef Colourful_Wheel(u8 WheelPos);
/* Complicated Color Effects -------------------------------------------------*/
void rainbowCycle(u16 Pixel_LEN);


#endif /* __WS2812_H */

/*********************************************   END OF FILE   ***************************************************/
