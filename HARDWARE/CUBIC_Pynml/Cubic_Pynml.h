#ifndef	_CUBIC_PYNMAL_H
#define _CUBIC_PYNMAL_H
#include "sys.h"
#include "math.h"
#include "stdbool.h"

//���ζ���ʽ��ֵ
//y=a0+a1*t+a2*t^2+a3*t^3;
//%��֪����ǰ�ٶ�:V0     Ŀ���ٶȣ�V1    ����ʱ�䣺t1
//%ָ������ǰʱ��t0=0    ʼĩ���ٶȣ�A0=A1=0
//%��Ҫ���㣺a0,a1,a2,a3,�ĸ�ϵ��
//%�������̣��ɼ�������ĸ�ϵ������ָ����ʱ���ڰ���
//%��ʽV=a0+a1*t+a2*t^2+a3*t^3�������ÿ����ֵ����ٶ�
//a0=V0;
//a1=A0;
//a2=(3/(t1)^2)*(V1-V0)-(1/t1)*(2*A0+A1);
//a3=(2/(t1)^3)*(V0-V1)+(1/t1^2)*(A0+A1);
typedef struct
{
	float v0,acc0,acc1;	//�߽�����
	float v1;			//Ŀ���ٶ�
	uint32_t  iterat_time;	//����ʱ��
	float a0,a1,a2,a3;	//����ʽϵ��
	float cubic_out;	//����ʽ���
	bool dir;
}Cubic_Pynml;

extern Cubic_Pynml Px_Cubic,Py_Cubic;
void CubicStruc_Init(Cubic_Pynml *Cubic_struct,float v0,float goal_vel,uint32_t iterat_time,float acc0,float acc1,bool dir);
void Cubic_CoeffCal(Cubic_Pynml *Cubic_struct);
void Vel_NowCal(Cubic_Pynml *Cubic_struct,uint32_t nowtime,float *which_vel);

#endif
