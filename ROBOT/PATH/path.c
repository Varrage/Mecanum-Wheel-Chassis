#include "path.h"

Path_t Path1SL   = { 
							.StaPoint = {.x = XOFFSET, .y = YOFFSET, .z = 0 },
							.StpPoint = {.x = XOFFSET   , .y = YOFFSET, .z = 0 },
							.Execution = 1000,
							.type = Path1_StraightLine,
						  };

//Path_t StraightPathBack = {
//							.StaPoint = {.x = 7500   , .y = YOffset, .z = 0 },
//							.AccPoint = {.x = 5500   , .y = YOffset, .z = 0 },
//							.ConPoint = {.x = 1500   , .y = YOffset, .z = 0 },
//							.DecPoint = {.x = 700    , .y = YOffset, .z = 0 },
//							.xdisp = 0,
//							.ydisp = 0,
//							.zdisp = 0,
//							.xDecdisp = 0,
//							.yDecdisp = 0,
//							.zDecdisp = 0,
//							.AccPtime = 3000,
//							.ConPtime = 4000,
//							.DecPtime = 1000
//						  };

						  



float LinearEquation(float start,float end, int Nowtime, int Execution)
{
 	float range = end - start;
    float slope = range  * AMP_LIMIT(Nowtime,Execution,0) / (float)(Execution);
    return (range * slope + start);//返回值从start~end随时间线性变化
}


						  
						  
						  
						  
						  
						  
						  
						  
						  
						  
			  
						  
						  
//void PathCal(Chassis_t *chassis_t,Path_t *path_t)
//{	
//	if(fabs(((chassis_t->Chassis_pos.x)-(path_t->StaPoint.x)))<=(fabs((path_t->AccPoint.x)-(path_t->StaPoint.x))))
//	{
//		PID_Init(&chassis_t->Chassis_pid_x,5.0f,0.0f,0.0f,CAR_MAX_VEL,-CAR_MAX_VEL,5,-5);
//		PID_Init(&chassis_t->Chassis_pid_y,2.0f,0.0f,0.0f,CAR_MAX_VEL,-CAR_MAX_VEL,5,-5);
//		PID_Init(&chassis_t->Chassis_pid_z,0.5f,0.0f,0.0f,90,-90,5,-5);
//		
//		path_t->xdisp += ((path_t->AccPoint.x)-(path_t->StaPoint.x))/(path_t->AccPtime/10);
//		path_t->ydisp += ((path_t->AccPoint.y)-(path_t->StaPoint.y))/(path_t->AccPtime/10);
//		path_t->zdisp += ((path_t->AccPoint.z)-(path_t->StaPoint.z))/(path_t->AccPtime/10);
//		
//		chassis_t->Goal_pos.x = path_t->StaPoint.x + path_t->xdisp;
//		chassis_t->Goal_pos.y = path_t->StaPoint.y + path_t->ydisp;
//		chassis_t->Goal_pos.z = path_t->StaPoint.z + path_t->zdisp;	
//	}
//	else if((fabs(chassis_t->Chassis_pos.x-path_t->StaPoint.x)> fabs(path_t->AccPoint.x-path_t->StaPoint.x))&&
//			 fabs(chassis_t->Chassis_pos.x-path_t->StaPoint.x)<=fabs(path_t->ConPoint.x-path_t->StaPoint.x))
//	{
//		PID_Init(&chassis_t->Chassis_pid_x,4.0f,0.0f,0.0f,CAR_MAX_VEL,-CAR_MAX_VEL,5,-5);
//		PID_Init(&chassis_t->Chassis_pid_y,2.0f,0.0f,0.0f,CAR_MAX_VEL,-CAR_MAX_VEL,5,-5);
//		PID_Init(&chassis_t->Chassis_pid_z,0.5f,0.0f,0.0f,90,-90,5,-5);
//		
//		path_t->xdisp += (path_t->ConPoint.x-path_t->AccPoint.x)/(path_t->ConPtime/10);
//		path_t->ydisp += (path_t->ConPoint.y-path_t->AccPoint.y)/(path_t->ConPtime/10);
//		path_t->zdisp += (path_t->ConPoint.z-path_t->AccPoint.z)/(path_t->ConPtime/10);
//		
//		chassis_t->Goal_pos.x = path_t->StaPoint.x + path_t->xdisp;
//		chassis_t->Goal_pos.y = path_t->StaPoint.y + path_t->ydisp;
//		chassis_t->Goal_pos.z = path_t->StaPoint.z + path_t->zdisp;	
//	}	
//	else if((fabs(chassis_t->Chassis_pos.x-path_t->StaPoint.x)> fabs(path_t->ConPoint.x-path_t->StaPoint.x))&&
//			 fabs(chassis_t->Chassis_pos.x-path_t->StaPoint.x)<=fabs(path_t->DecPoint.x-path_t->StaPoint.x))
//	{	
//		PID_Init(&chassis_t->Chassis_pid_x,5.0f,0.0f,0.0f,CAR_MAX_VEL,-CAR_MAX_VEL,5,-5);
//		PID_Init(&chassis_t->Chassis_pid_y,2.0f,0.0f,0.0f,CAR_MAX_VEL,-CAR_MAX_VEL,5,-5);
//		PID_Init(&chassis_t->Chassis_pid_z,0.5f,0.0f,0.0f,90,-90,5,-5);

//		path_t->xDecdisp += (path_t->DecPoint.x-path_t->ConPoint.x)/(path_t->DecPtime/10);
//		path_t->yDecdisp += (path_t->DecPoint.y-path_t->ConPoint.y)/(path_t->DecPtime/10);
//		path_t->zDecdisp += (path_t->DecPoint.z-path_t->ConPoint.z)/(path_t->DecPtime/10);	
//		
//		if(fabs(chassis_t->Goal_pos.x-path_t->StaPoint.x)<=fabs(path_t->DecPoint.x-path_t->StaPoint.x))
//			chassis_t->Goal_pos.x = path_t->ConPoint.x + path_t->xDecdisp;
//		else 
//			chassis_t->Goal_pos.x = path_t->DecPoint.x;	
//		chassis_t->Goal_pos.y = path_t->ConPoint.y + path_t->yDecdisp;
//		chassis_t->Goal_pos.z = path_t->ConPoint.z + path_t->zDecdisp;
//	}
//	else
//	{
//		PID_Init(&chassis_t->Chassis_pid_x,2.0f,0.0f,0.0f,CAR_MAX_VEL,-CAR_MAX_VEL,5,-5);
//		PID_Init(&chassis_t->Chassis_pid_y,2.0f,0.0f,0.0f,CAR_MAX_VEL,-CAR_MAX_VEL,5,-5);
//		PID_Init(&chassis_t->Chassis_pid_z,0.5f,0.0f,0.0f,90,-90,5,-5);
//		
//		chassis_t->Goal_pos.x = path_t->DecPoint.x;
//		chassis_t->Goal_pos.y = path_t->DecPoint.y;
//		chassis_t->Goal_pos.z = path_t->DecPoint.z;
//	}
//}
