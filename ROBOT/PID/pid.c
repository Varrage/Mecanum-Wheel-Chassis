#include "pid.h"

float Vx=0;
float Vy=0;
float Vz=0;

Pos GoalPos = {0,0,0};
Pos RealPos = {0,0,0};
//电机PID参数
PID Vel_PID[4];
//位置PID参数
PID Pos_PID[3];

void All_Pid_Init(void)
{
	for(u8 i=0;i<4;i++)
		PID_Init(&Vel_PID[i],0.8,0,5,TIM_PERIOD,-TIM_PERIOD,100,-100);
	PID_Init(&Pos_PID[0],4,0,5,VX_LIMIT,-VX_LIMIT,100,-100);
	PID_Init(&Pos_PID[1],4,0,5,VY_LIMIT,-VY_LIMIT,100,-100);
	PID_Init(&Pos_PID[2],2,0,1,VZ_LIMIT,-VZ_LIMIT,100,-100);
}

void PID_Init(PID *PID,float kp,float ki,float kd,float max,float min,float Summax,float Summin)
{
	PID->Kp = kp;
	PID->Ki = ki;
	PID->Kd = kd;
	PID->Err = 0;
	PID->ErrSum = 0;
	PID->Max = max;
	PID->Min = min ;
	PID->Summax = Summax;
	PID->Summin = Summin;
}
void PID_Cal(PID *PID,float goal,float now_val)
{
	static float errsum = 0;
	float pout = 0;
	float iout = 0;
	float dout = 0;
	float pidout = 0;
	float delta_err = 0;
	
	PID->Err = (int)(goal - now_val);
	delta_err = PID->Err - PID->LastErr;
	errsum += PID->Err;
	PID->ErrSum = MY_LIMIT(errsum,PID->Summax,PID->Summin);
	PID->LastErr = PID->Err;
	
	pout = PID->Kp * PID->Err;
	iout = PID->Ki * PID->ErrSum;	
	dout = PID->Kd * delta_err;
	
	pidout = pout + iout + dout;
	PID->Pid_out = MY_LIMIT(pidout,PID->Max,PID->Min);
}


void Pos_PidCal(Pos goal_pos)
{
	PID_Cal(&Pos_PID[0],goal_pos.x,RealPos.x);
	PID_Cal(&Pos_PID[1],goal_pos.y,RealPos.y);
	PID_Cal(&Pos_PID[2],goal_pos.z,RealPos.z);
	Vx = Pos_PID[0].Pid_out;
	Vy = Pos_PID[1].Pid_out;
	Vz = Pos_PID[2].Pid_out;
}


//全速时速度值能达到2000(cnt/5ms),即err最大可达到4000
//V1~V4取值就在±2850之间(2000/0.7)
//速度PID计算
//vx,vy,vz:目标速度
//计算结果：脉宽增量
void Vel_PidCal(void)
{
	PID_Cal(&Vel_PID[0],(float)V1,Rpm_Vel[0]);
	PID_Cal(&Vel_PID[1],(float)V2,Rpm_Vel[1]);
	PID_Cal(&Vel_PID[2],(float)V3,Rpm_Vel[2]);
	PID_Cal(&Vel_PID[3],(float)V4,Rpm_Vel[3]);
	
	Pulse_width[0] += (int)Vel_PID[0].Pid_out;
	Pulse_width[1] += (int)Vel_PID[1].Pid_out;
	Pulse_width[2] += (int)Vel_PID[2].Pid_out;
	Pulse_width[3] += (int)Vel_PID[3].Pid_out;
}

void Move_Start(void)
{
	Motor_SetPWM(1,Pulse_width[0]);
	Motor_SetPWM(2,Pulse_width[1]);
	Motor_SetPWM(3,Pulse_width[2]);
	Motor_SetPWM(4,Pulse_width[3]);
}



