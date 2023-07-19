
/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "myconfig.h"

/* 定义 -----------------------------------------------------------------------------------------------------------------*/
uint8_t TargerNum;
uint8_t RoadLineCheck[2]={0,0};
uint8_t RoadLine;

/**
 * 函数名:main  
 * 描述:主函数
 * 输入:无
 * 输出:无
 */
int main(void)
{   OLED_Init();
	SysTick_Init();//滴答定时器初始化	
	NVIC_Config();//中断优先级分组2
	Usart1_Init(115200);//115200-8-N-1
//	Usart2_Init(115200);//115200-8-N-1
//	Usart3_Init(115200);//115200-8-N-1
	Tim3_Init();//定时器3-10ms定时
	TIM1_PWM_Init(0,7199);//10khz驱动电机
	TIM2_Encoder_Init();//定时器2编码器配置
	TIM4_Encoder_Init();//定时器4编码器配置
	Motor_GPIO_Init();//电机对应引脚初始化
OLED_ShowString(1, 2, "sipeed");
while(1)
{
    printf("\r\njishu:%d\n",Read_Pluse(2));
}

}