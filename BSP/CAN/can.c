#include "can.h"
#include "delay.h"
#include "usart.h"
#include "os.h"
#include "string.h"
#include "frictionwheel.h"
#include "elmo.h"
//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ42M,�������CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//������Ϊ:42M/((6+7+1)*6)=500Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��; 

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

	//ʹ�����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��PORTAʱ��	                   											 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	

	//��ʼ��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��PA11,PA12

	//���Ÿ���ӳ������
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1); //GPIOA11����ΪCAN1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1); //GPIOA12����ΪCAN1

	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM=DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
	CAN_InitStructure.CAN_ABOM=DISABLE;	//����Զ����߹���	  
	CAN_InitStructure.CAN_AWUM=DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART=ENABLE;	//��ֹ�����Զ����� 
	CAN_InitStructure.CAN_RFLM=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
	CAN_InitStructure.CAN_TXFP=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;	 //ģʽ���� 
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=CAN_BS1_9tq; //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=CAN_BS2_4tq;//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=3;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
	CAN_Init(CAN1, &CAN_InitStructure);   // ��ʼ��CAN1 

	//���ù�����
	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0
	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��

	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	return 0;
}   
 


//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:1,�ɹ�;
//		 0,ʧ��;
u8 CAN1_Send_Msg(u8* msg,u8 len,int id)
{	
	u8 mbox; 
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=id;	 // ��׼��ʶ��Ϊ0
	TxMessage.IDE=0;		  // ʹ����չ��ʶ��
	TxMessage.RTR=0;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	TxMessage.DLC=len;	 // ����len֡��Ϣ
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];				 // ��һ֡��Ϣ          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0;
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
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

	//ʹ�����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);					//ʹ��PORTBʱ��	                   											 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);					//ʹ��CAN2ʱ��	

	//��ʼ��GPIO
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_12 | GPIO_Pin_13; 
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;							//���ù���
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;						//�������
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;				//100MHz
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_UP;							//����
	GPIO_Init(GPIOB, &GPIO_InitStructure);										//��ʼ��PB12,PB13

	//���Ÿ���ӳ������
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_CAN2);					//GPIOB12����ΪCAN2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_CAN2);					//GPIOB13����ΪCAN2

	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM		= DISABLE;								//��ʱ�䴥��ͨ��ģʽ   
	CAN_InitStructure.CAN_ABOM		= DISABLE;								//����Զ����߹���	  
	CAN_InitStructure.CAN_AWUM		= DISABLE;								//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART		= DISABLE;								//��ֹ�����Զ����� 
	CAN_InitStructure.CAN_RFLM		= DISABLE;								//���Ĳ�����,�µĸ��Ǿɵ�  
	CAN_InitStructure.CAN_TXFP		= ENABLE;									//���ȼ��ɱ��ı�ʶ������ 
	CAN_InitStructure.CAN_Mode		= CAN_Mode_Normal;				//����ģʽ 
	CAN_InitStructure.CAN_SJW		= CAN_SJW_1tq;							//����ͬ����Ծ���(Tsjw)CAN_SJW_1tq+1��ʱ�䵥λ
	CAN_InitStructure.CAN_BS1		= CAN_BS1_9tq;							//Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2		= CAN_BS2_4tq;							//Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler	= 3;										//��Ƶϵ��(Fdiv)3+1	
	CAN_Init(CAN2, &CAN_InitStructure);											// ��ʼ��CAN1 

	//���ù�����
	CAN_FilterInitStructure.CAN_FilterNumber			=14;											//(CAN2ֻ���ù�����14~28��С��14�Ľ������ж�)						//������14
	CAN_FilterInitStructure.CAN_FilterMode				=CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale				=CAN_FilterScale_32bit;		//32λ 
	CAN_FilterInitStructure.CAN_FilterIdHigh			=0x0000;									//32λID
	CAN_FilterInitStructure.CAN_FilterIdLow				=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh		=0x0000;								//32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow			=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment	=CAN_Filter_FIFO0;		//������14������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation		=ENABLE;								//���������0
	CAN_FilterInit(&CAN_FilterInitStructure);																//�˲�����ʼ��

	CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);																	//CAN2������FIFO0
	
	NVIC_InitStructure.NVIC_IRQChannel 						= CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 2;							// �����ȼ�Ϊ6
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 0;									// �����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}

//CAN2���ͺ���
u8 CAN2_Send_Msg(u8* msg,u8 len,int id)
{	
	u8 mbox; 
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=id;	 // ��׼��ʶ��Ϊ0
	TxMessage.IDE=0;		  // ʹ����չ��ʶ��
	TxMessage.RTR=0;		  // ��Ϣ����Ϊ����֡��һ֡8λ
	TxMessage.DLC=len;	 // ����len֡��Ϣ
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];				 // ��һ֡��Ϣ          
	mbox= CAN_Transmit(CAN2, &TxMessage);   
	i=0;
	while((CAN_TransmitStatus(CAN2, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
	if(i>=0XFFF)
		return 0;
	else
		return 1;
	
}
 

//�жϷ�����			     
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
		current_x=cnt_x*0.0798;                        //CNTת�ɺ���
		current_y=cnt_y*0.0798;
	}
	OSIntExit();
}


void EncoderSet(double x, double y)
{
	int32_t turn_X,turn_Y;
	CAN2_Send_Msg(Set_Gyro_Cmd,2,0x15);		//��ʼ��������
	turn_X=Reverse(x);
	turn_Y=Reverse(y);
	memcpy(&Data_Test,&turn_X,4);
	memcpy(&Data_Test,&turn_Y,4);
	CAN2_Send_Msg(Data_Test,8,0x15);		//��ʼ��������
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










