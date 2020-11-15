#ifndef TOUCH_SWITCH_H
#define TOUCH_SWITCH_H
#include "sys.h"
 
#define SWITCH_GPIOX	GPIOD

extern uint32_t WHEEL_PIN[4];

void Touch_Switch_Init(void);
unsigned char Switch_Upper(void);
unsigned char Switch_Below(void);
#endif



