#ifndef _TB6612_H_
#define _TB6612_H_
#include "sys.h"
#include "struct.h"


/*头部TB6612设置*/
#define Head_AIN2	PCout(12)//
#define Head_AIN1	PCout(10)//
#define Head_STBY	PAout(12)//
#define Head_BIN1	PAout(8) //
#define Head_BIN2	PCout(8) //

/*尾部TB6612设置*/
#define Tail_AIN2	PAout(6)//
#define Tail_AIN1	PAout(4)//
#define Tail_STBY	PCout(3)//
#define Tail_BIN1	PCout(1)//
#define Tail_BIN2	PFout(9)//


void Motor_Init(void);
void Motor_IOInit(void);
void Motor_PWMInit(void);
void Motor_Stop(void);
void Motor_Disable(void);
void Motor_Enable(void);
void Motor_SetPWM(u8 ID,int pluse);
static void Motor_Setdir(u8 ID,u8 dir);
#endif

