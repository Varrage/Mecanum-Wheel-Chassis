#include "pid.h"


void All_Pid_Init(void)
{
	/*轮子速度PID*/
	PID_Init(&Chassis.pid_vel,0.8,0,5,TIM_PERIOD,-TIM_PERIOD,100,-100);
	/*三轴位置PID*/
	PID_Init(&(Chassis.pid_x), 4,0,5, VX_LIMIT,-VX_LIMIT, 100,-100);
	PID_Init(&(Chassis.pid_y), 4,0,5, VY_LIMIT,-VY_LIMIT, 100,-100);
	PID_Init(&(Chassis.pid_z), 2,0,1, VZ_LIMIT,-VZ_LIMIT, 100,-100);
}

void PID_Init(PID_t* PID,float kp,float ki,float kd,float max,float min,float Summax,float Summin)
{
	PID->Kp = kp;
	PID->Ki = ki;
	PID->Kd = kd;
	
	PID->Max = max;
	PID->Min = min ;
	PID->SumMax = Summax;
	PID->SumMin = Summin;
	
	PID->Err = 0;
	PID->Err_1 = 0;
	PID->Err_sum = 0;
	PID->P_out = 0;
	PID->I_out = 0;
	PID->D_out = 0;

}

void PID_Cal(PID_t *PID, float goal, float now_val)
{
	float delta_err = 0;
	
	PID->Err = (int)(goal - now_val);
	delta_err = PID->Err - PID->Err_1;
	PID->Err_sum += PID->Err;
	PID->Err_sum = AMP_LIMIT(PID->Err_sum,PID->SumMax,PID->SumMin);
	PID->Err_1 = PID->Err;
	
	PID->P_out = PID->Kp * PID->Err;
	PID->I_out = PID->Ki * PID->Err_sum;	
	PID->D_out = PID->Kd * delta_err;
	
	PID->PID_OUT = PID->P_out + PID->I_out + PID->D_out;
	PID->PID_OUT = MY_LIMIT(PID->PID_OUT, PID->Max, PID->Min);
}


void Pos_PidCal(Pos_t goal_pos)
{
	PID_Cal(&(Chassis.pid_x), Chassis.Goal_pos.x, Chassis.Real_pos.x);
	PID_Cal(&(Chassis.pid_y), Chassis.Goal_pos.y, Chassis.Real_pos.y);
	PID_Cal(&(Chassis.pid_z), Chassis.Goal_pos.z, Chassis.Real_pos.z);
}


//全速时速度值能达到2000(cnt/5ms),即err最大可达到4000
//V1~V4取值就在±2850之间(2000/0.7)
//速度PID计算
//vx,vy,vz:目标速度
//计算结果：脉宽增量
void Vel_PidCal(void)
{
	for(u8 i=0;i<4;i++)
	{
		PID_Cal(&Chassis.pid_vel,Chassis.Goal_wspeed[i],Chassis.Real_wspeed[i]);
		Chassis.Pulse_width[i] += (int)Chassis.pid_vel.PID_OUT;
	}

}

