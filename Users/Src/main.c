#include "myconfig.h"

uint8_t TargerNum;
uint8_t RoadLineCheck[2]={0,0};
uint8_t RoadLine;

int main(void)
{   OLED_Init();
	SysTick_Init();//�δ�ʱ����ʼ��	
	NVIC_Config();//�ж����ȼ�����2
	Usart1_Init(115200);//115200-8-N-1
	Tim3_Init();//��ʱ��3-10ms��ʱ
	TIM1_PWM_Init(0,7199);//10khz�������
	TIM2_Encoder_Init();//��ʱ��2����������
	TIM4_Encoder_Init();//��ʱ��4����������
	Motor_GPIO_Init();//�����Ӧ���ų�ʼ��
OLED_ShowString(1, 2, "sipeed");
while(1)
{
//    printf("\r\njishu:%d\n",Read_Pluse(2));
}

}