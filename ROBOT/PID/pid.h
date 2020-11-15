#ifndef	PID_H_
#define PID_H_
#include "sys.h"
#include "math.h"
#include "tb6612.h"
#include "can.h"
#define MY_LIMIT(x,max,min)	((x>=max)?max:((x<=min)?min:x))
#define my_abs(x)			((x>=0)?x:(-x))

#define ENCODER_WIRE		500										//�������߳�
#define DEC_RAYIO			30										//������ٱ�

#define PI					(3.14159265358979323846f)

#define rad2deg(rad)		((rad) / PI * 180)						//�����Ƶ��Ƕ��Ƶ�ת��
#define deg2rad(deg)		((deg) * PI / 180)

//���峤��Ϊ115mm��175mm
#define	CAR_RADUI			(152)									//�������ĵ��������ĵľ���(mm)
#define WHEEL_RADIU			(30)									// ���Ӱ뾶(mm)
#define VZ2RPM(w)			((w)*CAR_RADUI/WHEEL_RADIU)			//Vz������prm��ת��(Vz*2PI/60 * RADUI) = (rpm*2PI/60 *WHEEL_RADIU),Vz��λҲ��rpm
#define V2RPM(v)			((v)*60/(2*PI*WHEEL_RADIU))
//V�ĵ�λΪpulse/ms
//Vx,Vy��λΪmm/s��Vx_max\Vy_max = 500*2pi*30/(60*0.707)=2221���Ƕ��߲���ͬʱȡ�����ֵ
#define theat	45			//������X�᷽��н�
#define V1	(V2RPM(-Vx*cos(deg2rad(theat))+Vy*sin(deg2rad(theat)))+VZ2RPM(Vz))
#define V2	(V2RPM(-Vx*cos(deg2rad(theat))-Vy*sin(deg2rad(theat)))+VZ2RPM(Vz))
#define V3	(V2RPM(Vx*cos(deg2rad(theat))-Vy*sin(deg2rad(theat)))+VZ2RPM(Vz))
#define V4	(V2RPM(Vx*cos(deg2rad(theat))+Vy*sin(deg2rad(theat)))+VZ2RPM(Vz))


//PID��������ṹ��
typedef struct 
{
	float Kp;
	float Ki;
	float Kd;
	
	int Err;
	float LastErr;
	float ErrSum;
	float Summax;
	float Summin;
	
	float Max;
	float Min;
	
	float Pid_out;
	
}PID;

//����λ�ýṹ�壬��mmΪ��λ
typedef struct 
{
	float x;
	float y;
	float z;
}Pos;


extern Pos GoalPos,RealPos;
extern PID Vel_PID[4];

void All_Pid_Init(void);
void PID_Init(PID *PID,float kp,float ki,float kd,float max,float min,float Summax,float Summin);
void PID_Cal(PID *PID,float goal,float now_val);

void Pos_PidCal(Pos goal_pos);
void Vel_PidCal(void);
void Move_Start(void);
void Vz_cal(float goal);

#endif


