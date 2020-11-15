#include "PR_Task.h"
#include "os.h"
#include "size.h"

extern unsigned int cnt_x;
extern unsigned int cnt_y;
/*摩擦轮转速*/
int FrictionWheel_Vel = 3000;//6710
extern u8 Set_Gyro_Cmd[2];
extern u8 Data_Test[8] ;

//电机控制任务
void Control_task(void *p_arg)
{
	volatile static CPU_SR cpu_sr;
	int j=0;
	p_arg = p_arg;
	while(1)
	{	

		//OS_CRITICAL_ENTER();
		Elmo_PVM(1,vel[0]);
		Elmo_PVM(2,vel[1]);
		Elmo_PVM(3,vel[2]);
		Elmo_PVM(4,vel[3]);		//Elmo_PVM改变要发送的值，每隔100us定时器产生中断将这个值发送出去
		//OS_CRITICAL_EXIT();		//加临界区避免在发送数据更改期间进入中断
		
		if(j<=5)
		{
			PUSH_OUT();
			j++;
		}
		else
			PUSH_IN();
		
		M2006_Set_Speed(FrictionWheel_Vel,2);
		M2006_Set_Speed(-FrictionWheel_Vel,3);
		M2006_Set_Speed(FrictionWheel_Vel,4);
		M2006_Set_Speed(-FrictionWheel_Vel,5);
		
		//printf("%f\r\n",realAng);
		delay_ms(6);
	}
}


//底盘闭环任务
void Loop_task(void *p_arg)
{
	int i=0;
	p_arg = p_arg;
	while(1)
	{
		switch(flag_pos) 
		{
			case 1:
//				if(i == 0)
//				{
//					CAN2_Send_Msg(Set_Gyro_Cmd,2,0x15);		//初始化陀螺仪
//					delay_ms(5);
//					CAN2_Send_Msg(Data_Test,8,0x15);		//初始化陀螺仪
//					EncoderSet(50,28);
//				}
				first_ball();	
				/*i++; */	  		break;
			case 20:
				second_ball();	break;
			case 30:
				third_ball();	break;
			case 40:
				fourth_ball();	break;
			case 50:
				fifth_ball();	break;
			case 200:
				PID_Pos(aim_x, current_x, aim_y, current_y, aim_angle , realAng);  //保持位置不变
				break;
			case FLAG_DEBUG:	//测试函数，置FLAG_DEBUG为222进入debug()之中(main.c 147行）
				debug();		break;
			default:
				PID_Pos(aim_x, current_x, aim_y, current_y, aim_angle , realAng);  //保持位置不变
				break;
		}
			delay_ms(5);
	}
}

//抓球任务
void Catch_task(void *p_arg)
{
	p_arg = p_arg;
	while(1)
	{
		if(flag_arm)
			Throw();
//		else
//			PID_Pos(aim_x ,current_x ,aim_y ,current_y ,aim_angle ,realAng);
		
		delay_ms(3);
	}

}




