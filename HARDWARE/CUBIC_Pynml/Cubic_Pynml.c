#include "Cubic_Pynml.h"


Cubic_Pynml Px_Cubic;
Cubic_Pynml Py_Cubic;
//���ζ���ʽĿ��ֵ�趨
void CubicStruc_Init(Cubic_Pynml *Cubic_struct,float v0,float goal_vel,uint32_t iterat_time,float acc0,float acc1,bool dir)
{
	Cubic_struct->v1 = goal_vel;
	Cubic_struct->iterat_time = iterat_time;
	Cubic_struct->v0 = v0;
	Cubic_struct->acc0 = acc0;
	Cubic_struct->acc1 = acc1;
	Cubic_struct->dir = dir;
}



//���ζ���ʽ��������
void Cubic_CoeffCal(Cubic_Pynml *Cubic_struct)
{
	Cubic_struct->a0 = Cubic_struct->v0;
	Cubic_struct->a1 = Cubic_struct->acc0;
	Cubic_struct->a2 = (3/(float)pow(Cubic_struct->iterat_time,2))*(Cubic_struct->v1 - Cubic_struct->v0)
						-(1/(float)Cubic_struct->iterat_time)*(2*Cubic_struct->a0 + Cubic_struct->a1);
	Cubic_struct->a3 = (2/(float)pow(Cubic_struct->iterat_time,3))*(Cubic_struct->v0 - Cubic_struct->v1)
						+(1/(float)pow(Cubic_struct->iterat_time,2))*(Cubic_struct->a0 + Cubic_struct->a1);
}



//�����ٶ��趨ֵ�͵�ǰʱ�䣬�������ζ���ʽ��ֵ���㵱ǰ�ٶ�
void Vel_NowCal(Cubic_Pynml *Cubic_struct,uint32_t nowtime,float *which_vel)
{
	Cubic_struct->cubic_out = Cubic_struct->a0 + Cubic_struct->a1 * nowtime
								+Cubic_struct->a2 *pow(nowtime,2)+Cubic_struct->a3 *pow(nowtime,3);
	if(Cubic_struct->dir)
		*which_vel = Cubic_struct->cubic_out;
	else
		*which_vel = Cubic_struct->v1 - Cubic_struct->cubic_out;
}
