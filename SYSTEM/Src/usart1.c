#include "usart1.h"
#if 1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
    int handle;

};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
    x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
    while((USART1->SR&0X40)==0);
    USART1->DR=(u8)ch;
    return ch;
}
#endif

uint8_t Usart_RxData;
uint8_t Usart_RxFlag;

uint8_t Usart_RxPacket[100];//�������ݰ�
uint8_t Usart_RxPacket_Len=100;//�������ݰ�����

void Usart_Init(void)
{
    RCC_APB2PeriphClockCmd(USART_RCC,ENABLE);
    RCC_APB2PeriphClockCmd(USART_GPIO_RCC,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//���������Ҳ���Ƿ��Ϳ�
    GPIO_InitStructure.GPIO_Pin=USART_TX;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(USART_GPIO,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//�������룬���ն�
    GPIO_InitStructure.GPIO_Pin=USART_RX;
    GPIO_Init(USART_GPIO,&GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate=9600;//������
    USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//����
    USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
    USART_InitStructure.USART_Parity=USART_Parity_No;//У��λ
    USART_InitStructure.USART_StopBits=USART_StopBits_1;
    USART_InitStructure.USART_WordLength=USART_WordLength_8b;
    USART_Init(USART_X,&USART_InitStructure);

    //���������ж�
    USART_ITConfig(USART_X,USART_IT_RXNE,ENABLE);//�򿪽����ж�
    //�����ж���
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    //�����ж�
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel=USART_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART_X,ENABLE);
    USART_ClearFlag(USART_X,USART_FLAG_TC);//������ڷ��ͱ�־λ

}

void Send_Byte(uint8_t Byte)//����һ���ֽ�
{
    USART_SendData(USART_X,Byte);
    while(USART_GetFlagStatus(USART_X,USART_FLAG_TXE)==RESET);
}

void Send_Array(uint8_t *Array,uint16_t Length)//����һ������
{
    uint16_t i;
    for(i=0;i<Length;i++)
    {
        Send_Byte(Array[i]);
    }
}

void Send_String(char*String)//����һ���ַ���
{
    while(*String)
    {
        Send_Byte(*String++);
    }
}

uint32_t Pow(uint32_t X,uint32_t Y)
{
    uint32_t result=1;
    while(Y--)
    {
        result*=X;
    }
    return result;
}

void Send_Number(uint32_t Num,uint8_t Length)//��������
{
    uint8_t i;
    for(i=0;i<Length;i++)
    {
        Send_Byte(Num/Pow(10,Length-i-1)%10+'0');
    }
}

void Usart_Printf(char *format,...)
{
    char String[100];
    va_list arg;
    va_start(arg,format);
    vsprintf(String,format,arg);
    va_end(arg);
    Send_String(String);
}

uint8_t Usart_GetRxFlag(void)
{
    if(Usart_RxFlag==1)
    {
        Usart_RxFlag=0;
        return 1;
    }
    return 0;
}

uint8_t Usart_GetRxData(void)
{
    return Usart_RxData;
}

uint8_t id_Flag;//1ΪKp 2ΪKi 3ΪKd
uint8_t Data_BitNum=0;//���ݵ�λ������12.123 ��6λ -12.123��7Ϊ

//�����жϣ����ڽ���vofa�Ĳ�����   #P1=12.123��   #PΪ֡ͷ��1Ϊ�Ǹı�˭�ı�־λ�� =�������ռ���־λ
void USART1_IRQHandler(void)    //  12.123�����ݱ���  ����֡β
{
    static uint8_t RxState=0;
    static uint8_t pRxPacket=0;
    if(USART_GetFlagStatus(USART_X,USART_IT_RXNE)==SET)
    {
        Usart_RxData=USART_ReceiveData(USART_X);
        if(RxState==0&&Usart_RxData==0x23) //��һ��֡ͷ  "#"==0x23
        {
            RxState=1;
        }
        else if(RxState==1&&Usart_RxData==0x50) //�ڶ���֡ͷ  "P"==0x50
        {
            RxState=2;
        }
        else if(RxState==2)//ȷ�ϴ��εĶ��� ���޸�id_Flag
        {
            id_Flag=Usart_RxData-48;
            RxState=3;
        }
        else if(RxState==3&&Usart_RxData==0x3D) //�жϵȺţ�Ҳ�������Ϊ���ݿ�ʼ��֡ͷ
        {
            RxState=4;
        }
        else if(RxState==4)//��ʼ���մ��������
        {
            if(Usart_RxData==0x21)//������֡β   ���û�н��յ�������������������һֱ����
            {
                Data_BitNum=pRxPacket;//��ȡλ��
                pRxPacket=0;//������������´ν��н�������
                RxState=0;
                Usart_RxFlag=1;
            }
            else
            {
                Usart_RxPacket[pRxPacket++]=Usart_RxData;//�����ݷ������ݰ���
            }
        }
        USART_ClearITPendingBit(USART_X,USART_IT_RXNE);
    }
}

uint8_t Get_id_Flag(void)//����ȡid_Flag��װ�ɺ���
{
    uint8_t id_temp;
    id_temp=id_Flag;
    id_Flag=0;
    return id_temp;
}

float Pow_invert(uint8_t X,uint8_t n)//x����n��10
{
    float result=X;
    while(n--)
    {
        result/=10;
    }
    return result;
}

//uint8_t Usart_RxPacket[5]={0x31,0x32,0x2E,0x31,0x33};//���Ը����ݰ�ֱ�Ӹ�ֱֵ�ӵ���һ�»�����򣬿��Ƿ����Ϊ12.13
//Data_BitNum = 5//���������ݵĳ���ҲҪ����
//Ȼ��ֱ����������ͷ�  Usart_Printf("%f\n",RxPacket_Data_Handle());  Delay_ms(1000);��ok��
float RxPacket_Data_Handle(void)//���ݰ����㴦��
{
    float Data=0.0;
    uint8_t dot_Flag=0;//С�����־λ��������С������С����ǰ 0ΪС����ǰ��1ΪС�����
    uint8_t dot_after_num=1;//С�����ĵڼ�λ
    int8_t minus_Flag=1;// ���ű�־λ -1Ϊ�Ǹ��� 1Ϊ����
    for(uint8_t i=0;i<Data_BitNum;i++)
    {
        if(Usart_RxPacket[i]==0x2D)//�����һλΪ����
        {
            minus_Flag=-1;
            continue;//��������ѭ��
        }
        if(dot_Flag==0)
        {
            if(Usart_RxPacket[i]==0x2E)//���ʶ��С���㣬��dot_Flag��1
            {
                dot_Flag=1;
            }
            else//��û����С����ǰ������
            {
                Data = Data*10 + Usart_RxPacket[i]-48;
            }
        }
        else//����С����������
        {
            Data = Data + Pow_invert(Usart_RxPacket[i]-48,dot_after_num);
            dot_after_num++;
        }
    }
    return Data*minus_Flag;//�����������ݷ��س��� ������ϸ��ű�־λ
}

