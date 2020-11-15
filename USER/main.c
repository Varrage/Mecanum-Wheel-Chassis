#include "includes.h"
#include "touch_switch.h"
#include "PR_Task.h"
#include "move.h"
#include "path.h"
#include "size.h"


/*�������ȼ�*/
#define START_TASK_PRIO		3
#define CONTROL_TASK_PRIO	10
#define LOOP_TASK_PRIO		13
#define CATCH_TASK_PRIO		15

/*�����ջ��С*/	
#define START_STK_SIZE 		128
#define CONTROL_STK_SIZE 	128
#define LOOP_STK_SIZE 		128
#define CATCH_STK_SIZE 		128

/*������ƿ鶨��*/
OS_TCB StartTaskTCB;
OS_TCB Control_taskTCB;
OS_TCB Loop_taskTCB;
OS_TCB Catch_taskTCB;

/*�����ջ����*/	
CPU_STK START_TASK_STK[START_STK_SIZE];
CPU_STK CONTROL_TASK_STK[CONTROL_STK_SIZE];
CPU_STK LOOP_TASK_STK[LOOP_STK_SIZE];
CPU_STK CATCH_TASK_STK[CATCH_STK_SIZE];

//����������*/
void Start_task(void *p_arg);		//�������������
void Control_task(void *p_arg);		//�����������
void Loop_task(void *p_arg);		//���̱ջ�����
void Catch_task(void *p_arg);		//ץ������

u8 reset_gyro_cmd[2]= {0x55,0XAA};
	
				

//������
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	
	delay_init(168);  //ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
	uart_init(115200);   //���ڳ�ʼ��
	
	Elmo_Init(CAN1, 1,0);
	delay_ms(100);
	CAN2_Init();
	Touch_Switch_Init();
	Cylinder_Init();
	//Throw_Init();	
	PID_Init(&Pid_Pos_X,300,0,0,500000,-500000,1000000,-1000000);
	PID_Init(&Pid_Pos_Y,300,0,0,500000,-500000,1000000,-1000000);
	PID_Init(&Pid_Pos_Ang,500,0,6,500000,-500000,1000000,-1000000);
	

 	OSInit(&err);		    //��ʼ��UCOSIII
	OS_CRITICAL_ENTER();	//�����ٽ���			 
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"Start_task", 		//��������
                 (OS_TASK_PTR )Start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);      //����UCOSIII
}


//��ʼ����������
void Start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	
	CPU_Init();
//	CAN2_Send_Msg(reset_gyro_cmd,2,0x15);		//��ʼ��������	
//	delay_ms(4000);
	
	
	//OSStatTaskCPUUsageInit(&err);  	//ͳ������                
	//ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	//OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
	OS_CRITICAL_ENTER();	//�����ٽ���
	//���������������
	OSTaskCreate((OS_TCB 	* )&Control_taskTCB,		
				 (CPU_CHAR	* )"Control_task", 		
                 (OS_TASK_PTR )Control_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )CONTROL_TASK_PRIO,     
                 (CPU_STK   * )&CONTROL_TASK_STK[0],	
                 (CPU_STK_SIZE)CONTROL_STK_SIZE/10,	
                 (CPU_STK_SIZE)CONTROL_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	//�������̱ջ�����
	OSTaskCreate((OS_TCB 	* )&Loop_taskTCB,		
				 (CPU_CHAR	* )"Loop_task", 		
                 (OS_TASK_PTR )Loop_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LOOP_TASK_PRIO,     	
                 (CPU_STK   * )&LOOP_TASK_STK[0],	
                 (CPU_STK_SIZE)LOOP_STK_SIZE/10,	
                 (CPU_STK_SIZE)LOOP_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);		
	//����ץ������			 
	OSTaskCreate((OS_TCB 	* )&Catch_taskTCB,		
				 (CPU_CHAR	* )"Catch_task", 		
                 (OS_TASK_PTR )Catch_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )CATCH_TASK_PRIO,     	
                 (CPU_STK   * )&CATCH_TASK_STK[0],	
                 (CPU_STK_SIZE)CATCH_STK_SIZE/10,	
                 (CPU_STK_SIZE)CATCH_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);			 

//	flag_pos = FLAG_DEBUG;
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������
}

