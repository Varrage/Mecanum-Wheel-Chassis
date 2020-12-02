#include "os.h"
#include "includes.h"
#include "struct.h"

Chassis_t Chassis = {0};

void Control_task(void *p_arg)
{
	while(1)
	{
		//Motor_SetPWM()
	}
}
void Loop_task(void *p_arg)
{}
void Catch_task(void *p_arg)
{}
/*
//电机控制任务

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




//抓球任务

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
*/



