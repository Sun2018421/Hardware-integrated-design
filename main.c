#include "lcd.h"
#include "reg52.h"


typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;

void delay(u16 i){	
	while(i--);
}
/*
	P17->H1
	P16->H2
	P15->H3
	P14->H4

	P13->L1
	P12->L2
	P11->L3
	P10->L4
*/
#define GPIO_KEY P1
void KeyDown(void){
	u8 a = 0;
	GPIO_KEY = 0x0f;  //0000 1111
	if(GPIO_KEY!=0x0f){
		delay(1000); //延时消抖
		//测试列
		GPIO_KEY = 0x0f;
	}
}
void main(){
	LcdInit();

	while(1){
		
	}
	
}