#include "os.h"
#include "includes.h"
#include "struct.h"

Chassis_t Chassis = {0};

void Control_task(void *p_arg)
{

		#if Wheel_pid_test
			while(1)
			{
				PID_Cal(&Chassis.pid_vel[0], 500 ,Chassis.Real_wspeed[0]);
				Chassis.Pulse_width[0] += (int)(Chassis.pid_vel[0].PID_OUT);
				Motor_SetPWM(1,Chassis.Pulse_width[0]);
				printf("%f\r\n",Chassis.Real_wspeed[0]);
				delay_ms(5);
			}
		#endif

		#if Pos_pid_test
			
			while(1)
			{
				//PID_Cal(&(Chassis.pid_x), 1000, Chassis.Real_pos.x);
				//PID_Cal(&(Chassis.pid_y), 0, Chassis.Real_pos.y);
				//PID_Cal(&(Chassis.pid_z), 0, Chassis.Real_pos.z);
				//GyroEncoder_Set(0x11,300);
				Speed_distribution(200, 0, 0, Chassis.Goal_wspeed);
				Vel_PidCal();
				Wheel_Move();
				delay_ms(5);
			}

		#endif

		#if Normal_run
			while(1)
			{
				Pos_PidCal();
				Speed_distribution(*Chassis.Vel_x, *Chassis.Vel_y, *Chassis.Vel_z, Chassis.Goal_wspeed);
				Vel_PidCal();
				Wheel_Move();
				delay_ms(5);
			}
		#endif


}
void Loop_task(void *p_arg)
{
	while(1)
	{
		//探测到东西（10cm），记录下点，进行膨胀，然后记录下点的位置
		delay_ms(1000);
	}
}
void Catch_task(void *p_arg)
{
	while(1)
	{
		
		delay_ms(1000);
	}
}
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



