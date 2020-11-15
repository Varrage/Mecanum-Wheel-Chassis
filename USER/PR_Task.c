#include "PR_Task.h"
#include "os.h"
#include "size.h"

extern unsigned int cnt_x;
extern unsigned int cnt_y;
/*Ħ����ת��*/
int FrictionWheel_Vel = 3000;//6710
extern u8 Set_Gyro_Cmd[2];
extern u8 Data_Test[8] ;

//�����������
void Control_task(void *p_arg)
{
	volatile static CPU_SR cpu_sr;
	int j=0;
	p_arg = p_arg;
	while(1)
	{	

		//OS_CRITICAL_ENTER();
		Elmo_PVM(1,vel[0]);
		Elmo_PVM(2,vel[1]);
		Elmo_PVM(3,vel[2]);
		Elmo_PVM(4,vel[3]);		//Elmo_PVM�ı�Ҫ���͵�ֵ��ÿ��100us��ʱ�������жϽ����ֵ���ͳ�ȥ
		//OS_CRITICAL_EXIT();		//���ٽ��������ڷ������ݸ����ڼ�����ж�
		
		if(j<=5)
		{
			PUSH_OUT();
			j++;
		}
		else
			PUSH_IN();
		
		M2006_Set_Speed(FrictionWheel_Vel,2);
		M2006_Set_Speed(-FrictionWheel_Vel,3);
		M2006_Set_Speed(FrictionWheel_Vel,4);
		M2006_Set_Speed(-FrictionWheel_Vel,5);
		
		//printf("%f\r\n",realAng);
		delay_ms(6);
	}
}


//���̱ջ�����
void Loop_task(void *p_arg)
{
	int i=0;
	p_arg = p_arg;
	while(1)
	{
		switch(flag_pos) 
		{
			case 1:
//				if(i == 0)
//				{
//					CAN2_Send_Msg(Set_Gyro_Cmd,2,0x15);		//��ʼ��������
//					delay_ms(5);
//					CAN2_Send_Msg(Data_Test,8,0x15);		//��ʼ��������
//					EncoderSet(50,28);
//				}
				first_ball();	
				/*i++; */	  		break;
			case 20:
				second_ball();	break;
			case 30:
				third_ball();	break;
			case 40:
				fourth_ball();	break;
			case 50:
				fifth_ball();	break;
			case 200:
				PID_Pos(aim_x, current_x, aim_y, current_y, aim_angle , realAng);  //����λ�ò���
				break;
			case FLAG_DEBUG:	//���Ժ�������FLAG_DEBUGΪ222����debug()֮��(main.c 147�У�
				debug();		break;
			default:
				PID_Pos(aim_x, current_x, aim_y, current_y, aim_angle , realAng);  //����λ�ò���
				break;
		}
			delay_ms(5);
	}
}

//ץ������
void Catch_task(void *p_arg)
{
	p_arg = p_arg;
	while(1)
	{
		if(flag_arm)
			Throw();
//		else
//			PID_Pos(aim_x ,current_x ,aim_y ,current_y ,aim_angle ,realAng);
		
		delay_ms(3);
	}

}




