#include "can.h"
#include "delay.h"
#include "usart.h"
#include "string.h"

void MY_CAN_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	                   											 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_CAN1); 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_CAN1); 

	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM = DISABLE;				//��ʱ�䴥��ͨ��ģʽ   
	CAN_InitStructure.CAN_ABOM = DISABLE;				//����Զ����߹���	  
	CAN_InitStructure.CAN_AWUM = DISABLE;				//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART = ENABLE;				//��ֹ�����Զ����� 
	CAN_InitStructure.CAN_RFLM = DISABLE;				//���Ĳ�����,�µĸ��Ǿɵ�  
	CAN_InitStructure.CAN_TXFP = DISABLE;				//���ȼ��ɱ��ı�ʶ������ 
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	//ģʽ���� 
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;			//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq;			//Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;			//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler = 3;				//��Ƶϵ��(Fdiv)Ϊbrp+1	
	CAN_Init(CAN1, &CAN_InitStructure);				// ��ʼ��CAN1 

	//���ù�����
	CAN_FilterInitStructure.CAN_FilterNumber = 0;							//������0
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;		//32λ 
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;     
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;  
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;				//32λMASK
	
	
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;	//������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;				//���������0
	CAN_FilterInit(&CAN_FilterInitStructure);							//�˲�����ʼ��


	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;     // �����ȼ�Ϊ6
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

float RealPos_x = 0;
float RealPos_y = 0;
float RealPos_z = 0;
float x_pos = 0;
float y_pos = 0;
float w_pos = 0;

CanRxMsg RxMessage;
void CAN1_RX0_IRQHandler(void)
{
	
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	if(RxMessage.StdId == GORY_ID)
	{
		if(RxMessage.DLC == 4)
		{
			float temp=0;
			memcpy(&temp, &RxMessage.Data[0], 4);
			w_pos = temp;
		}
		if(RxMessage.DLC == 8)
		{
			int32_t  temp=0;
			memcpy(&temp, &RxMessage.Data[4], 4);
			//RealPos_x = Encoder2RealX(temp);
			memcpy(&temp, &RxMessage.Data[0], 4);
			//RealPos_y = Encoder2RealY(temp);
		}
	}
	if(RxMessage.StdId == F1_ID)
	{
		if (RxMessage.DLC == 8)
		{
			int32_t  temp=0;
			
			memcpy(&temp, &RxMessage.Data[4], 4);
			y_pos = Encoder2RealY(temp);
			memcpy(&temp, &RxMessage.Data[0], 4);
			x_pos = Encoder2RealX(temp);
			
		}
	}	
}




void claw_control(int ID,int command)		//֪ͨ�����Ǹ�λ
{
    CanTxMsg TxMessage;
    TxMessage.StdId = 0x606;
    TxMessage.IDE = CAN_ID_STD;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = 1;
	if( ID ==0 && command == 0)  		//��Ŷ������צ�ӣ�
	{
		TxMessage.Data[0] = 0x0;
	}
	else if( ID == 0 && command == 1)   //��Ŷ����
	{
		TxMessage.Data[0] = 0x1;
	}
	else if( ID ==1 && command == 0)	//һ�Ŷ������������
	{
		TxMessage.Data[0] = 0x2;
	}
	else if ( ID == 1 && command == 1)	//һ�Ŷ����
	{
		TxMessage.Data[0] = 0x3;
	}
    CAN1_Send(&TxMessage);
}

void GyroEncoder_Set(uint32_t id,float ang)	//���õ�ǰ�����ǽǶ�
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
