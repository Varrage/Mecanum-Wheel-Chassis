#ifndef _PATH_H
#define _PATH_H
#include "sys.h"
#include "struct.h"
#include "includes.h"

typedef enum
{
	Wait4Start,
	
	Path1_StraightLine,
	Path2_Bezier
	
}PathType_t;


typedef struct
{
	Pos_t StaPoint;
	Pos_t StpPoint;
	
	int Execution;
	u32 CurrentTime;
	
	PathType_t type;
}Path_t;



extern Path_t Path1SL;
extern Path_t StraightPathBack;

void Set_Goal(PathType_t PathType,int timenow);
float LinearEquation(float start,float end, int Nowtime, int Execution);
//void PathCal(Chassis_t *chassis_t,Path_t *path_t);

#endif
