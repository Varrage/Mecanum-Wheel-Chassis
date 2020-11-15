#include "touch_switch.h"
#include "delay.h"

uint32_t SWITCH_PIN[4] = {GPIO_Pin_10,GPIO_Pin_11,GPIO_Pin_13,GPIO_Pin_14};

void Touch_Switch_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);          
    GPIO_Initure.GPIO_Pin=SWITCH_PIN[0]|SWITCH_PIN[2];            
    GPIO_Initure.GPIO_Mode=GPIO_Mode_IN;      
    GPIO_Initure.GPIO_PuPd=GPIO_PuPd_UP;       	
    GPIO_Initure.GPIO_Speed=GPIO_Medium_Speed;     
    GPIO_Init(SWITCH_GPIOX,&GPIO_Initure);
	
	GPIO_Initure.GPIO_Pin=SWITCH_PIN[1]|SWITCH_PIN[3]; 
	GPIO_Initure.GPIO_Mode=GPIO_Mode_OUT; 	
	GPIO_Initure.GPIO_PuPd=GPIO_PuPd_DOWN;
	GPIO_Init(GPIOD,&GPIO_Initure);
	
	GPIO_ResetBits(GPIOD,SWITCH_PIN[1]);
	GPIO_ResetBits(GPIOD,SWITCH_PIN[3]);
}

unsigned char Switch_Upper(void)
{
	if(!GPIO_ReadInputDataBit(SWITCH_GPIOX,SWITCH_PIN[0]))
	{
		delay_ms(10);
		if(!GPIO_ReadInputDataBit(SWITCH_GPIOX,SWITCH_PIN[0]))
			return 1;
		else 
			return 0;
	}
	
	return 0;
}	//触碰开关被按下返回1

unsigned char Switch_Below(void)
{
	if(!GPIO_ReadInputDataBit(SWITCH_GPIOX,SWITCH_PIN[2]))
	{
		delay_ms(10);
		if(!GPIO_ReadInputDataBit(SWITCH_GPIOX,SWITCH_PIN[2]))
			return 1;
		else
			return 0;
	}
	
	return 0;
}	//触碰开关被按下返回1


