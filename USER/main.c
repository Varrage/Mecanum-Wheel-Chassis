#include "includes.h"


/*任务优先级*/
#define START_TASK_PRIO		3
#define CONTROL_TASK_PRIO	10
#define LOOP_TASK_PRIO		13
#define CATCH_TASK_PRIO		15

/*任务堆栈大小*/	
#define START_STK_SIZE 		128
#define CONTROL_STK_SIZE 	128
#define LOOP_STK_SIZE 		128
#define CATCH_STK_SIZE 		128

/*任务控制块定义*/
OS_TCB StartTaskTCB;
OS_TCB Control_taskTCB;
OS_TCB Loop_taskTCB;
OS_TCB Catch_taskTCB;

/*任务堆栈定义*/	
CPU_STK START_TASK_STK[START_STK_SIZE];
CPU_STK CONTROL_TASK_STK[CONTROL_STK_SIZE];
CPU_STK LOOP_TASK_STK[LOOP_STK_SIZE];
CPU_STK CATCH_TASK_STK[CATCH_STK_SIZE];

//任务函数声明*/
void Start_task(void *p_arg);		//创建任务的任务
void Control_task(void *p_arg);		//电机控制任务
void Loop_task(void *p_arg);		//底盘闭环任务
void Catch_task(void *p_arg);		//抓球任务

u8 reset_gyro_cmd[2]= {0x55,0XAA};
	
				

//主函数
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	
	delay_init(168);  //时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
	uart_init(115200);   //串口初始化

    /*自定义初始化*/
	Chassis_Init();
    CAN1_Init();
    Encoder_Init();
	UpdateTim_Init();
    LED_Init();
	Motor_Init();

 	OSInit(&err);		    //初始化UCOSIII
	OS_CRITICAL_ENTER();	//进入临界区			 
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"Start_task", 		//任务名字
                 (OS_TASK_PTR )Start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);      //开启UCOSIII
}


//开始任务任务函数
void Start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	
	CPU_Init();

	//OSStatTaskCPUUsageInit(&err);  	//统计任务                
	//使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	//OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
	OS_CRITICAL_ENTER();	//进入临界区
	//创建电机控制任务
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
	//创建底盘闭环任务
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
	//创建抓球任务			 
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

	OS_CRITICAL_EXIT();	//退出临界区
	OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身
}

