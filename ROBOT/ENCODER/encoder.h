#ifndef _ENCLDER_H
#define _ENCLDER_H
#include "sys.h"


extern float Motor_Speed1;
extern float Motor_Speed2 ;
extern float Motor_Speed3 ;
extern float Motor_Speed4 ;
extern int Encode_Data[4];		//±àÂëÆ÷Êý¾Ý
extern int Encode_Vel[4];
extern float Rpm_Vel[4];
void Encoder_Init(void);
void TIM2_Init(void);
void Encoder_Update(void);
#endif



