#include "can.h"
#include "delay.h"
#include "string.h"

void CAN1_Init(void)
{
	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
	NVIC_InitTypeDef  		NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	                   											 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8| GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_CAN1); 
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_CAN1); 

	//CAN单元设置
	CAN_InitStructure.CAN_TTCM = DISABLE;				//非时间触发通信模式   
	CAN_InitStructure.CAN_ABOM = DISABLE;				//软件自动离线管理	  
	CAN_InitStructure.CAN_AWUM = DISABLE;				//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN_InitStructure.CAN_NART = ENABLE;				//禁止报文自动传送 
	CAN_InitStructure.CAN_RFLM = DISABLE;				//报文不锁定,新的覆盖旧的  
	CAN_InitStructure.CAN_TXFP = DISABLE;				//优先级由报文标识符决定 
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;		//模式设置 
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;			//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq;			//Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;			//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler = 3;				//分频系数(Fdiv)为brp+1	
	CAN_Init(CAN1, &CAN_InitStructure);				// 初始化CAN1 

	//配置过滤器
	CAN_FilterInitStructure.CAN_FilterNumber = 0;							//过滤器0
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;		//32位 
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;     
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;  
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;				//32位MASK
	
	
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;	//过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;				//激活过滤器0
	CAN_FilterInit(&CAN_FilterInitStructure);							//滤波器初始化


	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.		    

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;     // 主优先级为6
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


CanRxMsg RxMessage;
void CAN1_RX0_IRQHandler(void)
{
	
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	if(RxMessage.StdId == GYRO_ID)
	{
		if(RxMessage.DLC == 4)
		{
			float temp=0;
			memcpy(&temp, &RxMessage.Data[0], 4);
			Chassis.Real_pos.z = temp;
		}
		if(RxMessage.DLC == 8)
		{
			int32_t  temp=0;
			memcpy(&temp, &RxMessage.Data[4], 4);
			Chassis.Real_pos.x = Encoder2RealX(temp);
			memcpy(&temp, &RxMessage.Data[0], 4);
			Chassis.Real_pos.y = Encoder2RealY(temp);
		}
	}
	if(RxMessage.StdId == F1_ID)
	{
		if (RxMessage.DLC == 8)
		{
			int32_t  temp=0;
		
			memcpy(&temp, &RxMessage.Data[4], 4);
			Chassis.Real_pos.x = Encoder2RealX(temp);
			memcpy(&temp, &RxMessage.Data[0], 4);
			Chassis.Real_pos.y = Encoder2RealY(temp);
			
		}
	}	
}

void GyroEncoder_Set(uint32_t id,float ang)	//设置当前陀螺仪角度
{
    CanTxMsg TxMessage;
    float tmp;
    TxMessage.StdId = id;
    TxMessage.IDE = CAN_ID_STD;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = 4;
    tmp = ang;
    memcpy(&TxMessage.Data[0], &tmp, 4);
    CAN1_Send(&TxMessage);
}

uint8_t CAN1_Send(CanTxMsg* TxMessage)
{
    uint8_t mailbox;
    uint16_t i = 0;
    mailbox = CAN_Transmit(CAN1, TxMessage);
    while ((CAN_TransmitStatus(CAN1, mailbox) == CAN_TxStatus_Failed) && (i < 0xFFF)) i++;
    if (i >= 0xFFF) return 1;
    return 0;
}
