#include "encoder.h"
#include "struct.h"

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

	/*引脚初始化*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //PA0,PA1初始化为复用
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;  //PB4,PB5,PB6,PB7初始化
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	/*设置复用*/
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5);    
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_TIM1);
	
	/*TIM设置*/
	TIM_Struct.TIM_ClockDivision=0;							  
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_Struct.TIM_Period = 65535;
	TIM_Struct.TIM_Prescaler =0;							  //预分频系数为零？还是1

	/*TIM初始化*/
	TIM_TimeBaseInit(TIM3, &TIM_Struct);					  //TIM输出
	TIM_TimeBaseInit(TIM4, &TIM_Struct);
	TIM_TimeBaseInit(TIM5, &TIM_Struct);
	TIM_TimeBaseInit(TIM1, &TIM_Struct);

	/*设置编码器模式*/
	TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	
	/*定时器输入参数设置*/
	TIM_ICStructInit(&TIM_ICStruct);
    TIM_ICStruct.TIM_ICFilter = 0;		//ICx_FILTER;
    TIM_ICInit(TIM1, &TIM_ICStruct);
	TIM_ICInit(TIM3, &TIM_ICStruct);
	TIM_ICInit(TIM4, &TIM_ICStruct);
	TIM_ICInit(TIM5, &TIM_ICStruct);    
	
	/*使能所有的TIM*/
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

/*使用定时器2来定时读取编码器信息*/
void TIM2_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  ///使能TIM3时钟
	
	TIM_TimeBaseInitStructure.TIM_Period = 4000-1; 	//自动重装载值		//4ms一次中断
	TIM_TimeBaseInitStructure.TIM_Prescaler=84-1;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM2,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=6; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
	

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{
		Encoder_Update();
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
}


void Encoder_Update(void)
{

	static int Encode_Data_Last[4]={0};
	static int Encode_Data_Base[4]={0};
	static int Cnt_last[4]={0};

	int temp[4]={0};
	int Encode_Vel[4]={0};
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
		if(i == 4)															//这里面有一个光电编码器，有一个是霍尔的，到时候看看是哪一个
			Chassis.Real_wspeed[i] = ((float)Encode_Vel[i]/(float)4); 					//输出轴转一圈有4*500*30(脉冲升降沿都计数，一个脉冲计数为4,编码器为500线，减速比为30)=6000;1000*60(1000(ms/s)*60(s/min)=60000)
		else																//因此每ms的计数值在数值上等于rpm
			Chassis.Real_wspeed[i] = ((float)Encode_Vel[i]/(float)4)*500/13;
			
	}

	
}
