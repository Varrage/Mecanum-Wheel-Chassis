#ifndef MOVE_H
#define MOVE_H
#include "sys.h"
#include "struct.h"
#include "math.h"
#include "string.h"
#include "path.h"

void Chassis_Init(void);
void speed_distribution(float Vcar_x, float Vcar_y, float Wcar, float *Goal_speed);
void Move_Start(void);

//void PID_Init(PID_t *pid,float kf,float kp,float ki,float kd,float max,float min,float summax,float summin);
//void PID_Cal(PID_t *pid, float nextgoal,float goal,float now_val);


#endif
