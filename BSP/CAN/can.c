#include "can.h"
#include "delay.h"
#include "usart.h"
#include "os.h"
#include "string.h"
#include "frictionwheel.h"
#include "elmo.h"
//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024; tq=(brp)*tpclk1
//波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//Fpclk1的时钟在初始化的时候设置为42M,如果设置CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//则波特率为:42M/((6+7+1)*6)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败; 

float realAng;
float realOmiga;

int32_t cnt_x, cnt_y = 0;
float current_x,current_y=0;

u8 Set_Gyro_Cmd[2]= {0x55,0Xee};
u8 Data_Test[8] = {0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00};


u8 CAN1_Init(void)
{
	GPIO_InitTypeDef 	   GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	NVIC_InitTypeDef       NVIC_InitStructure;

	//使能相关时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能PORTA时钟	                   											 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	

	//初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化PA11,PA12

	//引脚复用映射配置
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1); //GPIOA11复用为CAN1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1); //GPIOA12复用为CAN1

	//CAN单元设置
	CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式   
	CAN_InitStructure.CAN_ABOM=DISABLE;	//软件自动离线管理	  
	CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN_InitStructure.CAN_NART=ENABLE;	//禁止报文自动传送 
	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的  
	CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定 
	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;	 //模式设置 
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=CAN_BS1_9tq; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=CAN_BS2_4tq;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=3;  //分频系数(Fdiv)为brp+1	
	CAN_Init(CAN1, &CAN_InitStructure);   // 初始化CAN1 

	//配置过滤器
	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化

	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.		    

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	return 0;
}   
 


//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:1,成功;
//		 0,失败;
u8 CAN1_Send_Msg(u8* msg,u8 len,int id)
{	
	u8 mbox; 
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=id;	 // 标准标识符为0
	TxMessage.IDE=0;		  // 使用扩展标识符
	TxMessage.RTR=0;		  // 消息类型为数据帧，一帧8位
	TxMessage.DLC=len;	 // 发送len帧信息
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];				 // 第一帧信息          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	if(i>=0XFFF)
		return 0;
	else
		return 1;
	
}




void CAN2_Init(void)
{
	GPIO_InitTypeDef		GPIO_InitStructure; 
	CAN_InitTypeDef			CAN_InitStructure;
	CAN_FilterInitTypeDef	CAN_FilterInitStructure;
	NVIC_InitTypeDef		NVIC_InitStructure;

	//使能相关时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);					//使能PORTB时钟	                   											 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);					//使能CAN2时钟	

	//初始化GPIO
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_12 | GPIO_Pin_13; 
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;							//复用功能
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;						//推挽输出
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;				//100MHz
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_UP;							//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);										//初始化PB12,PB13

	//引脚复用映射配置
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_CAN2);					//GPIOB12复用为CAN2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_CAN2);					//GPIOB13复用为CAN2

	//CAN单元设置
	CAN_InitStructure.CAN_TTCM		= DISABLE;								//非时间触发通信模式   
	CAN_InitStructure.CAN_ABOM		= DISABLE;								//软件自动离线管理	  
	CAN_InitStructure.CAN_AWUM		= DISABLE;								//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN_InitStructure.CAN_NART		= DISABLE;								//禁止报文自动传送 
	CAN_InitStructure.CAN_RFLM		= DISABLE;								//报文不锁定,新的覆盖旧的  
	CAN_InitStructure.CAN_TXFP		= ENABLE;									//优先级由报文标识符决定 
	CAN_InitStructure.CAN_Mode		= CAN_Mode_Normal;				//正常模式 
	CAN_InitStructure.CAN_SJW		= CAN_SJW_1tq;							//重新同步跳跃宽度(Tsjw)CAN_SJW_1tq+1个时间单位
	CAN_InitStructure.CAN_BS1		= CAN_BS1_9tq;							//Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2		= CAN_BS2_4tq;							//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler	= 3;										//分频系数(Fdiv)3+1	
	CAN_Init(CAN2, &CAN_InitStructure);											// 初始化CAN1 

	//配置过滤器
	CAN_FilterInitStructure.CAN_FilterNumber			=14;											//(CAN2只能用过滤器14~28？小于14的进不了中断)						//过滤器14
	CAN_FilterInitStructure.CAN_FilterMode				=CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale				=CAN_FilterScale_32bit;		//32位 
	CAN_FilterInitStructure.CAN_FilterIdHigh			=0x0000;									//32位ID
	CAN_FilterInitStructure.CAN_FilterIdLow				=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh		=0x0000;								//32位MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow			=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment	=CAN_Filter_FIFO0;		//过滤器14关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation		=ENABLE;								//激活过滤器0
	CAN_FilterInit(&CAN_FilterInitStructure);																//滤波器初始化

	CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);																	//CAN2关联到FIFO0
	
	NVIC_InitStructure.NVIC_IRQChannel 						= CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 2;							// 主优先级为6
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 0;									// 次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}

//CAN2发送函数
u8 CAN2_Send_Msg(u8* msg,u8 len,int id)
{	
	u8 mbox; 
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=id;	 // 标准标识符为0
	TxMessage.IDE=0;		  // 使用扩展标识符
	TxMessage.RTR=0;		  // 消息类型为数据帧，一帧8位
	TxMessage.DLC=len;	 // 发送len帧信息
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];				 // 第一帧信息          
	mbox= CAN_Transmit(CAN2, &TxMessage);   
	i=0;
	while((CAN_TransmitStatus(CAN2, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	if(i>=0XFFF)
		return 0;
	else
		return 1;
	
}
 

//中断服务函数			     
void CAN2_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;
	OSIntEnter();
  	
    CAN_Receive(CAN2, 0, &RxMessage);
	
	if(RxMessage.StdId==0x11)
	{
		memcpy(&realAng, &RxMessage.Data[0], 4);
		memcpy(&realOmiga, &RxMessage.Data[4], 4);
	}
	if(RxMessage.StdId==0x12)
	{
		memcpy(&cnt_x, &RxMessage.Data[0], 4);
		memcpy(&cnt_y, &RxMessage.Data[4], 4);
		current_x=cnt_x*0.0798;                        //CNT转成毫米
		current_y=cnt_y*0.0798;
	}
	OSIntExit();
}


void EncoderSet(double x, double y)
{
	int32_t turn_X,turn_Y;
	CAN2_Send_Msg(Set_Gyro_Cmd,2,0x15);		//初始化陀螺仪
	turn_X=Reverse(x);
	turn_Y=Reverse(y);
	memcpy(&Data_Test,&turn_X,4);
	memcpy(&Data_Test,&turn_Y,4);
	CAN2_Send_Msg(Data_Test,8,0x15);		//初始化陀螺仪
}

int32_t Reverse(double num)
{
	int32_t* res;
	int32_t* temp;
	*temp = (int32_t)(num/0.0798);

	memcpy(res  , temp+3, 1);
	memcpy(res+1, temp+2, 1);
	memcpy(res+2, temp+1, 1);
	memcpy(res+4, temp  , 1);
	
	return *res;
}










