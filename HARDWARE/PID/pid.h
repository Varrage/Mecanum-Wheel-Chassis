#ifndef	PID_H_
#define PID_H_
#include "sys.h"
#include "math.h"
#include "tb6612.h"
#include "struct.h"

#define ENCODER_WIRE		500										//编码器线程
#define DEC_RAYIO			30										//电机减速比


//车体长宽为115mm，175mm
#define	CAR_RADUI			(152)								//轮子中心到车体中心的距离(mm)
#define WHEEL_RADIU			(30)								// 轮子半径(mm)
#define VZ2RPM(w)			((w)*CAR_RADUI/WHEEL_RADIU)			//Vz到轮子prm的转换(Vz*2PI/60 * RADUI) = (rpm*2PI/60 *WHEEL_RADIU),Vz单位也是rpm
#define V2RPM(v)			((v)*60/(2*PI*WHEEL_RADIU))
//V的单位为pulse/ms
//Vx,Vy单位为mm/s，Vx_max\Vy_max = 500*2pi*30/(60*0.707)=2221但是二者不能同时取到最大值
#define theat	45			//轮子与X轴方向夹角
#define V1	(V2RPM(-Vx *cos(deg2rad(theat)) + Vy * sin(deg2rad(theat))) + VZ2RPM(Vz))
#define V2	(V2RPM(-Vx *cos(deg2rad(theat)) - Vy * sin(deg2rad(theat))) + VZ2RPM(Vz))
#define V3	(V2RPM( Vx *cos(deg2rad(theat)) - Vy * sin(deg2rad(theat))) + VZ2RPM(Vz))
#define V4	(V2RPM( Vx *cos(deg2rad(theat)) + Vy * sin(deg2rad(theat))) + VZ2RPM(Vz))


void All_Pid_Init(void);
void PID_Init(PID_t* PID,float kp,float ki,float kd,float max,float min,float Summax,float Summin);
void PID_Cal(PID_t *PID,float goal,float now_val);

void Pos_PidCal(Pos_t goal_pos);
void Vel_PidCal(void);
void Move_Start(void);


#endif


