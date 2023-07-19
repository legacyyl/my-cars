#include "usart1.h"
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;

};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    while((USART1->SR&0X40)==0);
    USART1->DR=(u8)ch;
    return ch;
}
#endif

uint8_t Usart_RxData;
uint8_t Usart_RxFlag;

uint8_t Usart_RxPacket[100];//接受数据包
uint8_t Usart_RxPacket_Len=100;//接受数据包长度

void Usart_Init(void)
{
    RCC_APB2PeriphClockCmd(USART_RCC,ENABLE);
    RCC_APB2PeriphClockCmd(USART_GPIO_RCC,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//推挽输出，也就是发送口
    GPIO_InitStructure.GPIO_Pin=USART_TX;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(USART_GPIO,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输入，接收端
    GPIO_InitStructure.GPIO_Pin=USART_RX;
    GPIO_Init(USART_GPIO,&GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate=9600;//波特率
    USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//流控
    USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
    USART_InitStructure.USART_Parity=USART_Parity_No;//校验位
    USART_InitStructure.USART_StopBits=USART_StopBits_1;
    USART_InitStructure.USART_WordLength=USART_WordLength_8b;
    USART_Init(USART_X,&USART_InitStructure);

    //开启串口中断
    USART_ITConfig(USART_X,USART_IT_RXNE,ENABLE);//打开接受中断
    //配置中断组
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    //配置中断
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel=USART_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART_X,ENABLE);
    USART_ClearFlag(USART_X,USART_FLAG_TC);//清楚串口发送标志位

}

void Send_Byte(uint8_t Byte)//发送一个字节
{
    USART_SendData(USART_X,Byte);
    while(USART_GetFlagStatus(USART_X,USART_FLAG_TXE)==RESET);
}

void Send_Array(uint8_t *Array,uint16_t Length)//发送一个数组
{
    uint16_t i;
    for(i=0;i<Length;i++)
    {
        Send_Byte(Array[i]);
    }
}

void Send_String(char*String)//发送一个字符串
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

void Send_Number(uint32_t Num,uint8_t Length)//发送数字
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

uint8_t id_Flag;//1为Kp 2为Ki 3为Kd
uint8_t Data_BitNum=0;//数据的位数，即12.123 有6位 -12.123有7为

//串口中断，用于接受vofa的参数的   #P1=12.123！   #P为帧头，1为是改变谁的标志位， =是数据收集标志位
void USART1_IRQHandler(void)    //  12.123是数据本身  ！是帧尾
{
    static uint8_t RxState=0;
    static uint8_t pRxPacket=0;
    if(USART_GetFlagStatus(USART_X,USART_IT_RXNE)==SET)
    {
        Usart_RxData=USART_ReceiveData(USART_X);
        if(RxState==0&&Usart_RxData==0x23) //第一个帧头  "#"==0x23
        {
            RxState=1;
        }
        else if(RxState==1&&Usart_RxData==0x50) //第二个帧头  "P"==0x50
        {
            RxState=2;
        }
        else if(RxState==2)//确认传参的对象 即修改id_Flag
        {
            id_Flag=Usart_RxData-48;
            RxState=3;
        }
        else if(RxState==3&&Usart_RxData==0x3D) //判断等号，也可以类比为数据开始的帧头
        {
            RxState=4;
        }
        else if(RxState==4)//开始接收传输的数据
        {
            if(Usart_RxData==0x21)//结束的帧尾   如果没有接收到！即还有数据来，就一直接收
            {
                Data_BitNum=pRxPacket;//获取位数
                pRxPacket=0;//清除索引方便下次进行接收数据
                RxState=0;
                Usart_RxFlag=1;
            }
            else
            {
                Usart_RxPacket[pRxPacket++]=Usart_RxData;//把数据放在数据包内
            }
        }
        USART_ClearITPendingBit(USART_X,USART_IT_RXNE);
    }
}

uint8_t Get_id_Flag(void)//将获取id_Flag封装成函数
{
    uint8_t id_temp;
    id_temp=id_Flag;
    id_Flag=0;
    return id_temp;
}

float Pow_invert(uint8_t X,uint8_t n)//x除以n次10
{
    float result=X;
    while(n--)
    {
        result/=10;
    }
    return result;
}

//uint8_t Usart_RxPacket[5]={0x31,0x32,0x2E,0x31,0x33};//可以给数据包直接赋值直接调用一下换算程序，看是否输出为12.13
//Data_BitNum = 5//别忘记数据的长度也要设置
//然后直接在主程序就放  Usart_Printf("%f\n",RxPacket_Data_Handle());  Delay_ms(1000);就ok了
float RxPacket_Data_Handle(void)//数据包换算处理
{
    float Data=0.0;
    uint8_t dot_Flag=0;//小数点标志位，能区分小数点后或小数点前 0为小数点前，1为小数点后
    uint8_t dot_after_num=1;//小数点后的第几位
    int8_t minus_Flag=1;// 负号标志位 -1为是负号 1为正号
    for(uint8_t i=0;i<Data_BitNum;i++)
    {
        if(Usart_RxPacket[i]==0x2D)//如果第一位为负号
        {
            minus_Flag=-1;
            continue;//跳过本次循环
        }
        if(dot_Flag==0)
        {
            if(Usart_RxPacket[i]==0x2E)//如果识别到小数点，则将dot_Flag置1
            {
                dot_Flag=1;
            }
            else//还没遇到小数点前的运算
            {
                Data = Data*10 + Usart_RxPacket[i]-48;
            }
        }
        else//遇到小数点后的运算
        {
            Data = Data + Pow_invert(Usart_RxPacket[i]-48,dot_after_num);
            dot_after_num++;
        }
    }
    return Data*minus_Flag;//将换算后的数据返回出来 这里乘上负号标志位
}

