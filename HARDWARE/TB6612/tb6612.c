#include "tb6612.h"
#include "encoder.h"

void Motor_Init(void)
{
	Motor_IOInit();
	Motor_PWMInit();
	Motor_Enable();
	Encoder_Init();
}


void Motor_IOInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);   //IO口时钟使能	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_8|GPIO_Pin_12;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			//IO口设置输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);		
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_8|GPIO_Pin_10|GPIO_Pin_12;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);                  //IO口设置输出

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	
	GPIO_Init(GPIOF, &GPIO_InitStructure);                  //IO口设置输出
}

void Motor_Enable(void)
{
	Head_STBY = 1;
	Tail_STBY = 1;
}

void Motor_Disable(void)
{
	Head_STBY = 0;
	Tail_STBY = 0;
}

void Motor_Stop(void)
{
	Head_AIN1 = 0;
	Head_AIN2 = 0;
	Head_BIN1 = 0;
	Head_BIN2 = 0;
	
	Tail_AIN1 = 0;
	Tail_AIN2 = 0;
	Tail_BIN1 = 0;
	Tail_BIN2 = 0;
}


void Motor_PWMInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);       //使能总线中的TIM8
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);      //            TIM10
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11,ENABLE); 	  //            TIM11
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 	  //使能IO口
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); 
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM8);     //四路PWM口
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8); 
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource6,GPIO_AF_TIM10); 
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource7,GPIO_AF_TIM11); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;           
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	      //GPIO口设置
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        
	GPIO_Init(GPIOC,&GPIO_InitStructure);  
	GPIO_Init(GPIOF,&GPIO_InitStructure); 
	  
	TIM_TimeBaseStructure.TIM_Prescaler=84-1; 
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //定时器的设置
	TIM_TimeBaseStructure.TIM_Period=TIM_PERIOD;   
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM10,&TIM_TimeBaseStructure);           //TIM8,10,11
	TIM_TimeBaseInit(TIM11,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;         //定时器通道设置
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);  				  //四路通道输出
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);  		  //TIM8_CH1
	
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);  				  //TIM8_CH2
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);  
	
	TIM_OC1Init(TIM10, &TIM_OCInitStructure);  				  //TIM10_CH1
	TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);  
	
	TIM_OC1Init(TIM11, &TIM_OCInitStructure);  				  //TIM11_CH1
	TIM_OC1PreloadConfig(TIM11, TIM_OCPreload_Enable);  
 
	TIM_ARRPreloadConfig(TIM8,ENABLE);						  //在溢出后重新载入
	TIM_CtrlPWMOutputs  (TIM8,ENABLE);						  //高级TIM想输出PWM要设置这个
	TIM_ARRPreloadConfig(TIM10,ENABLE);
	TIM_ARRPreloadConfig(TIM11,ENABLE);
	
	TIM_Cmd(TIM8,  ENABLE);  
	TIM_Cmd(TIM10, ENABLE);  
	TIM_Cmd(TIM11, ENABLE);  
 	
	TIM_SetCompare1(TIM8, 0);								  //PWM输出值
	TIM_SetCompare2(TIM8, 0);
	TIM_SetCompare1(TIM11,0);
	TIM_SetCompare1(TIM10,0);	
}  


//ID  :  0:广播模式，1~4:四个电机;
//speed :0~TIM_PERIOD
void Motor_SetPWM(u8 ID,int pluse)
{
	int pluse_temp = 0;
	pluse_temp = AMP_LIMIT(pluse,TIM_PERIOD,-TIM_PERIOD);
	if(pluse_temp<0)
		Motor_Setdir(ID,0);
	else
		Motor_Setdir(ID,1);	
	pluse_temp = my_abs(pluse_temp);
	
	switch(ID)
	{
		//case 0:	TIM_SetCompare1(TIM8,pluse_temp);
		//		TIM_SetCompare2(TIM8,pluse_temp);
		//		TIM_SetCompare1(TIM11,pluse_temp);
		//		TIM_SetCompare1(TIM10,pluse_temp);break;
		case 1:TIM_SetCompare2(TIM8,pluse_temp);break;
		case 2:TIM_SetCompare1(TIM8,pluse_temp);break;
		case 3:TIM_SetCompare1(TIM10,pluse_temp);break;
		case 4:TIM_SetCompare1(TIM11,pluse_temp);break;
		default:break;
	}
}


//设置电机旋转方向，内部Motor_Setspeed使用，不对外调用
//ID  :  0:广播模式，1~4:四个电机;
//dir :	 1:正向；非1：反向	面向电机，顺时针为正方向
static void Motor_Setdir(u8 ID,u8 dir)
{
	if(dir == 1)
	{
		switch(ID)
		{
			//case 0:Head_AIN1 = 1;Head_AIN2 = 0;Head_BIN1 = 0;Head_BIN2 = 1;Tail_AIN1 = 1;Tail_AIN2 = 0;Tail_BIN1 = 0;Tail_BIN2 = 1;	break;
			case 1:Head_BIN1 = 0;Head_BIN2 = 1;break;
			case 2:Head_AIN1 = 1;Head_AIN2 = 0;break;
			case 3:Tail_BIN1 = 1;Tail_BIN2 = 0;break;
			case 4:Tail_AIN1 = 0;Tail_AIN2 = 1;break;
			default:break;
		}
	}
	else
	{
		
		switch(ID)
		{
			//case 0:Head_AIN1 = 0;Head_AIN2 = 1;Head_BIN1 = 1;Head_BIN2 = 0;Tail_AIN1 = 0;Tail_AIN2 = 1;Tail_BIN1 = 1;Tail_BIN2 = 0;	break;
			case 1:Head_BIN1 = 1;Head_BIN2 = 0;break;
			case 2:Head_AIN1 = 0;Head_AIN2 = 1;break;
			case 3:Tail_BIN1 = 0;Tail_BIN2 = 1;break;
			case 4:Tail_AIN1 = 1;Tail_AIN2 = 0;break;
			default:break;
		}
	}
}

