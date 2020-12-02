#ifndef STRUCT_H
#define STRUCT_H

#include "stdbool.h"
#include "sys.h"

#ifdef	CHASSIS_GLOBAL 
	#define CHASSIS_EXT
#else
	#define CHASSIS_EXT extern
#endif

#define PI						(3.141592653f)
/*码盘从动轮直径mm*/
#define SUBWHEEL_DIAMETER		(50.7f)
/*底盘旋转半径mm*/	
#define CHASSIS_RADIUS			(227)

	
/*码盘X轴偏移量*/
#define XOFFSET					(50.0f)
/*码盘Y轴偏移量*/
#define YOFFSET					(50.0f)

	
/*码盘数据转化为真实坐标值mm*/
#define Encoder2Real(x)			((x) * (SUBWHEEL_DIAMETER * PI) / 2000)
/*角度制与弧度制的相互转换*/
#define rad2deg(rad)		((rad) / PI * 180)						
#define deg2rad(deg)		((deg) * PI / 180)
	
/*车体前进速度mm/s转化为轮子的转速rps*/
#define	MMS_2_RPS(x)			((x)/(PI*WHEEL_DIAMETER))
/*30速度转换*/
#define RPS2JV(rps)				((int)(rps*(2000*EC30RR)))
	


/*限幅*/
#define AMP_LIMIT(x,max,min)	(((x)>(max)) ? (max) : ( ((x)<(min)) ? (min) : (x)))
#define my_abs(x)				((x>=0)?x:(-x))

typedef struct
{
	float x;
	float y;
	float z;
}Pos_t;


typedef struct
{
	/*需要设置的参数*/
	float Kp;
	float Ki;
	float Kd;

	float SumMax;
	float SumMin;
	float Max;
	float Min;
	
	/*实时产生的参数*/
	float Err;
	float Err_1;
	float Err_sum;
	
	float P_out;
	float I_out;
	float D_out;
	float PID_OUT;

}PID_t;

typedef struct
{
	/*底盘位置*/
	Pos_t Real_pos;
	Pos_t Goal_pos;
	PID_t pid_x;
	PID_t pid_y;
	PID_t pid_z;
	
	/*底盘速度*/
	float *Vel_x;
	float *Vel_y;
	float *Vel_z;
	
	/*轮子速度*/
	float Real_wspeed[4];
	float Goal_wspeed[4];
	PID_t pid_vel;

	/*PWM数值*/
	int Pulse_width[4];
	
}Chassis_t;


CHASSIS_EXT Chassis_t Chassis;

#endif
