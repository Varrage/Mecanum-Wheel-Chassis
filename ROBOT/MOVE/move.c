#include "move.h"
#include "pid.h"

/*底盘初始化*/
void Chassis_Init(void)
{
	
	Chassis.Goal_pos.x = 0;
	Chassis.Goal_pos.y = 0;
	Chassis.Goal_pos.z = 0;

	Chassis.Real_pos.x = 0;
	Chassis.Real_pos.y = 0;
	Chassis.Real_pos.z = 0;
	
	All_Pid_Init();

	Chassis.Vel_x = &Chassis.pid_x.PID_OUT;
	Chassis.Vel_y = &Chassis.pid_y.PID_OUT;
	Chassis.Vel_z = &Chassis.pid_z.PID_OUT;
	
	for(int i=0; i<3; i++)
	{
		Chassis.Real_wspeed[i] = 0;
		Chassis.Goal_wspeed[i] = 0;
		Chassis.Pulse_width[i] = 0;
	}

}

void Speed_distribution(float Vcar_x, float Vcar_y, float Wcar, float *Goal_speed)
{
	float a = 8, b = 13;
	
	Goal_speed[0] = -Vcar_y - Vcar_x + Wcar*(a+b);     //4个目标速度   左前
	Goal_speed[1] =  Vcar_y - Vcar_x + Wcar*(a+b);	   //注意模型代换  右前
	Goal_speed[2] = -Vcar_y + Vcar_x + Wcar*(a+b);	   //	           左后
	Goal_speed[3] =  Vcar_y + Vcar_x - Wcar*(a+b);
	/*
	Goal_speed[0] = V2RPM(-Vx *cos(deg2rad(theat)) + Vy * sin(deg2rad(theat))) + VZ2RPM(Vz)
	Goal_speed[1] = V2RPM(-Vx *cos(deg2rad(theat)) - Vy * sin(deg2rad(theat))) + VZ2RPM(Vz))
	Goal_speed[2] = V2RPM( Vx *cos(deg2rad(theat)) - Vy * sin(deg2rad(theat))) + VZ2RPM(Vz))
	Goal_speed[3] = V2RPM( Vx *cos(deg2rad(theat)) + Vy * sin(deg2rad(theat))) + VZ2RPM(Vz))
	*/

}

void Wheel_Move(void)
{
	for(u8 i=0;i<4;i++)
	{
		Motor_SetPWM(i,Chassis.Pulse_width[i]);
	}
}

/*PID初始化
void Per_Axis_Vel_Cal(void)
{
	PID_Cal(&Chassis.Chassis_pid_x,Chassis.Ff_Pos.x,Chassis.Goal_pos.x,Chassis.Chassis_pos.x);	//mm/s
	PID_Cal(&Chassis.Chassis_pid_y,Chassis.Ff_Pos.y,Chassis.Goal_pos.y,Chassis.Chassis_pos.y);	//mm/s
	PID_Cal(&Chassis.Chassis_pid_z,Chassis.Ff_Pos.z,Chassis.Goal_pos.z,Chassis.Chassis_pos.z);	//°/s
}






void PID_Init(PID_t *pid,float kf,float kp,float ki,float kd,float max,float min,float summax,float summin)
{
	pid->Kf = kf;
	pid->Kp = kp;
	pid->Ki = ki;
	pid->Kd = kd;
	pid->Err = 0;
	pid->Err_1 = 0;
	pid->Err_sum = 0;
	pid->SumMax = summax;
	pid->SumMin = summin;
	pid->PID_OUT = 0;
	pid->Max = max;
	pid->Min = min ;
}



void PID_Cal(PID_t *pid, float nextgoal,float goal,float now_val)
{
	pid->Err = goal - now_val;
	pid->Err_sum += pid->Err;
	pid->Err_sum = AMP_LIMIT(pid->Err_sum,pid->SumMax,pid->SumMin);
	pid->Err_1 = pid->Err;
	
	pid->P_out = pid->Kp * pid->Err;
	pid->I_out = pid->Ki * pid->Err_sum;
	pid->D_out = pid->Kd * (pid->Err - pid->Err_1);
	
	pid->F_out = pid->Kf * (nextgoal - goal);
	
	pid->PID_OUT = pid->P_out + pid->I_out + pid->D_out + pid->F_out;
	pid->PID_OUT = AMP_LIMIT(pid->PID_OUT,pid->Max,pid->Min);
}
*/