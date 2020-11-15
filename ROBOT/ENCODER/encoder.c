#include "encoder.h"


void Encoder_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_Struct;
	TIM_ICInitTypeDef TIM_ICStruct;

	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);	   //总线连接时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  //IO口连接时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;     //PA0,PA1复用
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5);    //设置复用
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_TIM1);
	
	TIM_Struct.TIM_ClockDivision=0;							  //TIM设置
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_Struct.TIM_Period = 65535;
	TIM_Struct.TIM_Prescaler =0;							  //预分频系数为零？还是1
	TIM_TimeBaseInit(TIM3, &TIM_Struct);					  //TIM输出
	TIM_TimeBaseInit(TIM4, &TIM_Struct);
	TIM_TimeBaseInit(TIM5, &TIM_Struct);
	TIM_TimeBaseInit(TIM1, &TIM_Struct);

	TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	
	TIM_ICStructInit(&TIM_ICStruct);
    TIM_ICStruct.TIM_ICFilter = 0;		//ICx_FILTER;
    TIM_ICInit(TIM1, &TIM_ICStruct);
	TIM_ICInit(TIM3, &TIM_ICStruct);
	TIM_ICInit(TIM4, &TIM_ICStruct);
	TIM_ICInit(TIM5, &TIM_ICStruct);    //定时器输入参数设置，没有通道之说
	
	TIM_ClearFlag(TIM1,TIM_FLAG_Update);
	TIM_ITConfig(TIM1, TIM_FLAG_Update, ENABLE);
	TIM1->CNT  = 0;
	TIM_Cmd(TIM1, ENABLE);
	
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_FLAG_Update, ENABLE);
	TIM3->CNT  = 0;
	TIM_Cmd(TIM3, ENABLE);
	
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);
	TIM_ITConfig(TIM4, TIM_FLAG_Update, ENABLE);
	TIM4->CNT  = 0;
	TIM_Cmd(TIM4, ENABLE);
	
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);
	TIM_ITConfig(TIM5, TIM_FLAG_Update, ENABLE);
	TIM5->CNT  = 0;
	TIM_Cmd(TIM5, ENABLE);
	
}






float Motor_Speed1 = 0;
float Motor_Speed2 = 0;
float Motor_Speed3 = 0;
float Motor_Speed4 = 0;

int Encode_Data[4]={0};
int Encode_Vel[4]={0};
float Rpm_Vel[4]={0};
void Encoder_Update(void)
{
	int temp[4]={0};
	static int Encode_Data_Last[4]={0};
	static int Encode_Data_Base[4]={0};
	static int Cnt_last[4]={0};
	//static float last_time=0;
	temp[0] = TIM1->CNT;
	temp[1] = TIM3->CNT;
	temp[2] = TIM4->CNT;
	temp[3] = TIM5->CNT;
	
	for(u8 i=0;i<4;i++)
	{
		if((temp[i]-Encode_Data_Last[i])>32767)
			Encode_Data_Base[i] -= 65535;
		else if((temp[i]-Encode_Data_Last[i])<-32767)       //超载？
			Encode_Data_Base[i] += 65535;
		Encode_Data_Last[i] = temp[i];
		
		Encode_Data[i] = Encode_Data_Base[i]+temp[i];
		Encode_Vel[i] = Encode_Data[i]-Cnt_last[i];
		Cnt_last[i] = Encode_Data[i];
		if(i == 4)
			Rpm_Vel[i] = ((float)Encode_Vel[i]/(float)4);//输出轴转一圈有4*500*30(脉冲升降沿都计数，一个脉冲计数为4,编码器为500线，减速比为30)=6000;1000*60(1000(ms/s)*60(s/min)=60000)
		else																//因此每ms的计数值在数值上等于rpm
			Rpm_Vel[i] = ((float)Encode_Vel[i]/(float)4)*500/13;
			
	}
	Motor_Speed1 = Rpm_Vel[0];
	Motor_Speed2 = Rpm_Vel[1];
	Motor_Speed3 = Rpm_Vel[2];
	Motor_Speed4 = Rpm_Vel[3];
	//last_time = (float)millis();
	
}
