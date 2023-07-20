
/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "myconfig.h"

/* ���� -----------------------------------------------------------------------------------------------------------------*/
uint8_t TargerNum;
uint8_t RoadLineCheck[2]={0,0};
uint8_t RoadLine;
short jishu;
int main(void)
{   OLED_Init();
    SysTick_Init();//�δ�ʱ����ʼ��
    NVIC_Config();//�ж����ȼ�����2
    Usart_Init();
//	Usart1_Init(115200);//115200-8-N-1
//	Usart2_Init(115200);//115200-8-N-1
//	Usart3_Init(115200);//115200-8-N-1
//	Tim3_Init();//��ʱ��3-10ms��ʱ
    TIM1_PWM_Init(0,7199);//10khz�������
    TIM2_Encoder_Init();//��ʱ��2����������
    TIM4_Encoder_Init();//��ʱ��4����������
    Motor_GPIO_Init();//�����Ӧ���ų�ʼ��
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