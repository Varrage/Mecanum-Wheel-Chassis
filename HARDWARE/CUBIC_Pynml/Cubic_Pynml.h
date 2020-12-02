#ifndef	_CUBIC_PYNMAL_H
#define _CUBIC_PYNMAL_H
#include "sys.h"
#include "math.h"
#include "stdbool.h"

//三次多项式插值
//y=a0+a1*t+a2*t^2+a3*t^3;
//%已知：当前速度:V0     目标速度：V1    迭代时间：t1
//%指定：当前时间t0=0    始末加速度：A0=A1=0
//%需要计算：a0,a1,a2,a3,四个系数
//%迭代过程：由计算出的四个系数，在指定的时间内按照
//%公式V=a0+a1*t+a2*t^2+a3*t^3计算出在每个插值点的速度
//a0=V0;
//a1=A0;
//a2=(3/(t1)^2)*(V1-V0)-(1/t1)*(2*A0+A1);
//a3=(2/(t1)^3)*(V0-V1)+(1/t1^2)*(A0+A1);
typedef struct
{
	float v0,acc0,acc1;	//边界条件
	float v1;			//目标速度
	uint32_t  iterat_time;	//迭代时间
	float a0,a1,a2,a3;	//多项式系数
	float cubic_out;	//多项式输出
	bool dir;
}Cubic_Pynml;

extern Cubic_Pynml Px_Cubic,Py_Cubic;
void CubicStruc_Init(Cubic_Pynml *Cubic_struct,float v0,float goal_vel,uint32_t iterat_time,float acc0,float acc1,bool dir);
void Cubic_CoeffCal(Cubic_Pynml *Cubic_struct);
void Vel_NowCal(Cubic_Pynml *Cubic_struct,uint32_t nowtime,float *which_vel);

#endif
