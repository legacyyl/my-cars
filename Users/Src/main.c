#include "myconfig.h"

uint8_t TargerNum;
uint8_t RoadLineCheck[2]={0,0};
uint8_t RoadLine;

int main(void)
{   OLED_Init();
	SysTick_Init();
	NVIC_Config();
	Usart1_Init(115200);
	Tim3_Init();
	TIM1_PWM_Init(0,7199);
	TIM2_Encoder_Init();
	TIM4_Encoder_Init();
	Motor_GPIO_Init();
OLED_ShowString(1, 2, "sipeed");
while(1)
{
//    printf("\r\njishu:%d\n",Read_Pluse(2));
}

}