
/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "myconfig.h"

/* 定义 -----------------------------------------------------------------------------------------------------------------*/
uint8_t TargerNum;
uint8_t RoadLineCheck[2]={0,0};
uint8_t RoadLine;
short jishu;
int main(void)
{   OLED_Init();
    SysTick_Init();//滴答定时器初始化
    NVIC_Config();//中断优先级分组2
    Usart_Init();
//	Usart1_Init(115200);//115200-8-N-1
//	Usart2_Init(115200);//115200-8-N-1
//	Usart3_Init(115200);//115200-8-N-1
//	Tim3_Init();//定时器3-10ms定时
    TIM1_PWM_Init(0,7199);//10khz驱动电机
    TIM2_Encoder_Init();//定时器2编码器配置
    TIM4_Encoder_Init();//定时器4编码器配置
    Motor_GPIO_Init();//电机对应引脚初始化
    OLED_ShowString(1, 2, "xxxx");
    PID_Param_Init();
    Param.UnitTime_Motor1Pluse=0;
    Param.UnitTime_Motor2Pluse=0;
    Param.Sigma_Motor1Pluse=0;
    Param.Sigma_Motor2Pluse=0;
    Motor_Left_DIR(FORWARD);
    PID.Velocity_Target_Val=220;
    float PID_K[3]={5.0,6.0,7.0};//Kp,Ki,Kd
    uint8_t PID_index=0;
    while(1)
    {
        if(Usart_GetRxFlag())
        {
            PID_index=Get_id_Flag();
            PID_K[PID_index-1]=RxPacket_Data_Handle();
        }
        printf("%.3f,%.3f,%.3f\n",PID_K[0],PID_K[1],PID_K[2]);
        OLED_ShowNum(1,1,PID_index,1);

    }

}