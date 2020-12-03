#ifndef	PID_H_
#define PID_H_
#include "sys.h"
#include "math.h"
#include "struct.h"

void All_Pid_Init(void);
void PID_Init(PID_t* PID,float kp,float ki,float kd,float max,float min,float Summax,float Summin);
void PID_Cal(PID_t *PID,float goal,float now_val);
void Pos_PidCal(void);
void Vel_PidCal(void);

#endif


