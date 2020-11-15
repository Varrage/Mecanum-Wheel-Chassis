#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"
#include "pid.h"	
#include "FreeRTOS.h"
#include "task.h"

#define GORY_ID			0x7AD	
#define F1_ID           0x7AC

#define SUBWHEEL_DIAMETER		(58.0f)												// 码盘从动轮直径

#define Encoder2RealX(x)		((x) * (SUBWHEEL_DIAMETER * PI) / 4096 )		//码盘实际坐标计算,单位（mm）
#define Encoder2RealY(y)		(((-y)) * (SUBWHEEL_DIAMETER * PI) / 4096)

extern float x_pos;
extern float y_pos;
extern float w_pos;
extern float RealPos_x;
extern float RealPos_y;
extern float RealPos_z;


void MY_CAN_Init(void);
uint8_t CAN1_Send(CanTxMsg* TxMessage);
void claw_control(int ID,int command);
void GyroEncoder_Set(uint32_t id,float ang);
void CAN1_Init(void);
#endif

















