#include "lcd.h"
#include "reg52.h"
#include <intrins.h>
#include <stdio.h>

typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;


char Outputchar[18]={48,49,50,51,52,53,54,55,56,57,43,45,42,47,40,41,46,61};

float a = 0.0 , b = 0.0, c =0.0 ,ans = 0.0;

u8 State = 0; //最开始在初始状态
u8 add_sub_flag = 0 ; //1->'+' , 0->'-'
u8 multi_div_flag = 0 ; //1->'*' , 0->'/'

void printans(float a){
	u8 i ;
	char str[8];
	sprintf(str,"%f",a);
	LcdWriteCom(0xc0); //定位到第二行
	for(i= 0 ; i< 8 ; i++){
		LcdWriteData(str[i]);
	}
}
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
#define GPIO_KEY P1 //  0000 0000
#define GPIO_BUTTON P3 //独立按键使用P3
sbit K1 = P3^0; sbit K2 = P3^1;
sbit K3 = P3^2; sbit K4 = P3^3;
sbit K5 = P3^4; sbit K6 = P3^5;
sbit K7 = P3^6; sbit K8 = P3^7;
u8 isNum(u8 num){
	if((num>=0)&&(num<=9))
		return 1;
	return 0;
}

u8 KeyDown(void)
{
	u8 KeyValue= 127;
	char a=0;
	GPIO_KEY=0x0f;
	if(GPIO_KEY!=0x0f)//读取按键是否按下
	{
		delay(1000);//延时10ms进行消抖
		if(GPIO_KEY!=0x0f)//再次检测键盘是否按下
		{	
			//测试列
			GPIO_KEY=0X0F;
			switch(GPIO_KEY)
			{
				case(0X07):	KeyValue=0;break;
				case(0X0b):	KeyValue=1;break;
				case(0X0d): KeyValue=2;break;
				case(0X0e):	KeyValue=3;break;
			}
			//测试行
			GPIO_KEY=0XF0;
			switch(GPIO_KEY)
			{
				case(0X70):	KeyValue=KeyValue;break;
				case(0Xb0):	KeyValue=KeyValue+4;break;
				case(0Xd0): KeyValue=KeyValue+8;break;
				case(0Xe0):	KeyValue=KeyValue+12;break;
			}
			
		}
	}
	//delay(1000);
	while((a<50)&&(GPIO_KEY!=0xf0))	 //检测按键松手检测
	{
		delay(140);
		a++;
	}
	return KeyValue;
}


/*
从独立按键输入字符
*/
u8 keypros(){
	
	
	GPIO_BUTTON = 0xff;
	delay(1000);
	if(K1 == 0){
		while(!K1);
		return 10;  //'+'
	}
	if(K2 == 0){
		while(!K2);
		return 11; 
	}
	if(K3 == 0){
		while(!K3);
		return 12; 
	}
	if(K4 == 0){
		while(!K4);
		return 13; 
	}
	if(K5 == 0){
		while(!K5);
		return 14; 
	}
	if(K6 == 0){
		while(!K6);
		return 15; 
	}
	if(K7 == 0){
		while(!K7);
		return 16; 
	}
	if(K8 == 0){
		while(!K8);
		return 17; 
	}
	return 127;
}

u8 Getch(){
	u8 op = 127;
	while(op==127){
		op = keypros();
		if(op!=127)
			return op ;
		op = KeyDown();
			if(op!=127){
		return op;
		}
	}
	return 127;
}


void function_S0(){
	u8 num = Getch();
	if(num == 17){
		LcdWriteData(Outputchar[num]);
		printans(a);
	}
	else {
		if(isNum(num)==1){
			a = a*10 +num;
		}
		else if(num == 10){  // + -
			b = 0 ;
			add_sub_flag = 1;
			State = 1;
		}
		else if(num == 11){
			b = 0 ;
			add_sub_flag = 0 ;
			State = 1;
		}
		else if(num ==12 ){
			c = 0 ;
			State = 5;
			multi_div_flag = 1;
		}// * /
		else if(num == 13){
			c = 0 ;
			State =5 ;
			multi_div_flag = 0;
		}
		LcdWriteData(Outputchar[num]);
	}
}

	

void function_S1(){
	u8 num = Getch();
	if(isNum(num)==1){
		b = num;
		State = 2;
		LcdWriteData(Outputchar[num]);
	}
	else if(num == 10){ //+
		
	}
	else if(num == 11){ //-
	}
	else {
	}
}

void function_S2(){
	u8 num = Getch();
	if(num==17){
		State = 0;
		if(add_sub_flag ==1){
			a = a+b;
		}
		else 
			a = a-b;
		LcdWriteData(Outputchar[num]);
		printans(a);
	}
	else{
		if(isNum(num) == 1){
			b = b*10 + num;
		}
		else if (num== 12){  //*
			c = 0;
			State = 3;
			multi_div_flag = 1;
		}
		else if (num == 13){ // /
			c = 0;
			State = 3; 
			multi_div_flag = 0;
			LcdWriteData(Outputchar[num]);
		}
		else if (num == 10){
			if(add_sub_flag ==1){
				a = a+b;
			}
			else 
				a = a-b;
			State = 1;
			add_sub_flag = 1;
		}
		else if (num == 11){
			if(add_sub_flag ==1){
				a = a+b;
			}
			else 
				a = a-b;
			State = 1;
			add_sub_flag = 0;
			}
		LcdWriteData(Outputchar[num]);
	}
}

void function_S3(){
	u8 num = Getch();
	if(isNum(num)==1){
		c = num;
		State = 4;
	}
	else {
	}
}

void function_S4(){
	u8 num = Getch();
	if(isNum(num)==1){
		c = c*10+num;
	}
	else if(num == 10){ // +
		if(add_sub_flag == 1){
			if(multi_div_flag ==1)
				a= a+b*c;
			else
				a= a+b/c;
		}
		else{ 
			if(multi_div_flag ==1)
				a= a-b*c;
			else
				a= a-b/c;
		}
		b = 0 ; 
		c = 0;
		add_sub_flag = 1;
		State = 1;
	}
	else if(num == 11){
		if(add_sub_flag == 1){
			if(multi_div_flag ==1)
				a= a+b*c;
			else
				a= a+b/c;
		}
		else{ 
			if(multi_div_flag ==1)
				a= a-b*c;
			else
				a= a-b/c;
		}
		b = 0 ; 
		c = 0;
		add_sub_flag = 0;
		State = 1;
	}
	else if(num == 12){ //*
		if(multi_div_flag == 1){
			b = b*c ;
		}
		else 
			b = b/c;
		c = 0 ;
		State =3;
		multi_div_flag = 1;
	}
	else if(num == 13){ // /
		if(multi_div_flag == 1){
			b = b*c ;
		}
		else 
			b = b/c;
		c = 0 ;
		State =3;
		multi_div_flag = 0;
	}
	else if(num == 17){
		if(add_sub_flag == 1){
			if(multi_div_flag ==1)
				a= a+b*c;
			else
				a= a+b/c;
		}
		else{ 
			if(multi_div_flag ==1)
				a= a-b*c;
			else
				a= a-b/c;
		}
		b = 0 ; 
		c = 0;
		add_sub_flag = 0;
		State = 0;
	}
	if(num!=17){
		LcdWriteData(Outputchar[num]);
	}
	else{
		LcdWriteData(Outputchar[num]);
		printans(a);
	}
}

void function_S5(){
	u8 num = Getch();
}

void function_S6(){
	u8 num = Getch();
}
void function_S7(){
	u8 num = Getch();
}
void main(){
	LcdInit();
	while(1){
		switch(State){
			case 0:
				function_S0();
				break;
			case 1:
				function_S1();
				break;
			case 2:
				function_S2();
				break;
			case 3:
				function_S3();
				break;
			case 4:
				function_S4();
				break;
			case 5:
				function_S5();
				break;
			case 6:
				function_S6();
				break;
			case 7:
				function_S7(); //结果状态
				break;	
		}
	}
	
}