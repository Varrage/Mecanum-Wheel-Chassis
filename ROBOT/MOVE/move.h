#ifndef MOVE_H
#define MOVE_H
#include "sys.h"
#include "struct.h"
#include "math.h"
#include "string.h"

//车体长宽为115mm，175mm
#define	CAR_RADUI			(152)								//轮子中心到车体中心的距离(mm)
#define WHEEL_RADIU			(30)								// 轮子半径(mm)
#define VZ2RPM(w)			((w)*CAR_RADUI/WHEEL_RADIU)			//Vz到轮子prm的转换(Vz*2PI/60 * RADUI) = (rpm*2PI/60 *WHEEL_RADIU),Vz单位也是rpm
#define V2RPM(v)			((v)*60/(2*PI*WHEEL_RADIU))
#define theat	45			//轮子与X轴方向夹角

void Chassis_Init(void);
void Speed_distribution(float Vcar_x, float Vcar_y, float Wcar, float *Goal_speed);
void Wheel_Move(void);

#endif
