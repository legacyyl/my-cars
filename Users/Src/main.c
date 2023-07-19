#include "myconfig.h"

uint8_t TargerNum;
uint8_t RoadLineCheck[2]={0,0};
uint8_t RoadLine;

int main(void)
{   OLED_Init();
	SysTick_Init();
	NVIC_Config();
    Usart_Init();
	Tim3_Init();
	TIM1_PWM_Init(0,7199);
	TIM2_Encoder_Init();
	TIM4_Encoder_Init();
	Motor_GPIO_Init();
OLED_ShowString(1, 2, "xxx");
    float PID_K[3]={1.0,1.0,1.0};//Kp,Ki,Kd
    uint8_t PID_index=0;
while(1)
{
     if(Usart_GetRxFlag())
    {
      PID_index=Get_id_Flag();
      PID_K[PID_index-1]=RxPacket_Data_Handle();
    }
    Usart_Printf("%.3f,%.3f,%.3f\n",PID_K[0],PID_K[1],PID_K[2]);
    OLED_ShowNum(1,1,PID_index,1);
}

}