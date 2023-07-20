
/* Includes -------------------------------------------------------------------------------------------------------------*/
#include "myconfig.h"

/* ���� -----------------------------------------------------------------------------------------------------------------*/

int main(void)
{
    OLED_Init();
    SysTick_Init();//�δ�ʱ����ʼ��
    NVIC_Config();//�ж����ȼ�����2

//	Usart1_Init(115200);//115200-8-N-1
//	Usart2_Init(115200);//115200-8-N-1
//	Usart3_Init(115200);//115200-8-N-1
    Usart_Init();
	Tim3_Init();//��ʱ��3-10ms��ʱ

    TIM1_PWM_Init(0,7199);//10khz�������
    TIM2_Encoder_Init();//��ʱ��2����������
    TIM4_Encoder_Init();//��ʱ��4����������
    Motor_GPIO_Init();//�����Ӧ���ų�ʼ��


    PID_Param_Init();
    Param.UnitTime_Motor1Pluse=0;
    Param.UnitTime_Motor2Pluse=0;
    Param.Sigma_Motor1Pluse=0;
    Param.Sigma_Motor2Pluse=0;
    Motor_Left_DIR(BACK);

    float PID_K[4]={0.0,0.0,0.0,0.0};//Kp,Ki,Kd
    uint8_t PID_index=0;
    while(1)
    {
        if(Usart_GetRxFlag())
        {
            PID_index=Get_id_Flag();
            PID_K[PID_index-1]=RxPacket_Data_Handle();
            PID.Velocity_Kp=PID_K[0];
            PID.Velocity_Ki=PID_K[1];
            PID.Velocity_Kd=PID_K[2];
            PID.Velocity_Target_Val=PID_K[3];
//            OLED_ShowNum(4,2, PID.Velocity_Kp,3);
        }
        Usart_Printf("%.3f,%.3f,%.3f,%.3f\n",PID_K[0],PID_K[1],PID_K[2],PID_K[3]);
        OLED_ShowNum(1,1,Param.Motor1_PWM,4);
        OLED_ShowNum(2,2,PID_K[0],3);
        OLED_ShowNum(3,2,PID_K[3],3);

        printf("%.3f\n", PID.Velocity_Target_Val);
    }

}