#include "lcd.h"
#include "reg52.h"


typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;


char Outputchar[18]={48,49,50,51,52,53,54,55,56,57,43,45,42,47,40,41,46,61};
#define MAXLEN 15
char Opcodepoint = -1;  // +1 push 指向当前位置
char operandpoint = -1 ;
float Operand[MAXLEN];  //Operadn[MAXLEN-1]为整数部分临时保存位置
#define temp Operand[MAXLEN-1]
u8 Opcode[MAXLEN];
u8 times = 1;  //转换成float的长度标记
u8 Decimalpoint = 0 ;//有无小数点
u8 State = 0; //最开始在初始状态
void changeState(){
}
void InitOpcodeStack(){
	Opcodepoint++;
	Opcode[Opcodepoint] = 6;
}
/*
	优先级表
	-1 = < , 0 = = ,1 = >,2 = error
	+ 0,- 1* 2,/ 3,( 4,) 5,# 6
	= -> #
	*/
char OpPri[7][7]={  
	{1,1,-1,-1,-1,1,1},
	{1,1,-1,-1,-1,1,1},
	{1,1,1,1,-1,1,1},
	{1,1,1,1,-1,1,1},
	{-1,-1,-1,-1,-1,0,2},
	{1,1,1,1,2,1,1},
	{-1,-1,-1,-1,-1,2,0}
};

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


void Error(){
	
}
void handleNum(u8 num){
	u8 i ;
	float afterpoint = 0;
	afterpoint += num;
	if(Decimalpoint!=1){    //没有小数点
			temp = temp *10 + num;
	}
	else{
			for(i=0;i<times;i++){
					afterpoint = afterpoint*0.1;
			}
			temp = temp+afterpoint;
	}
}
char Precede(u8 op1,u8 op2){
	if(OpPri[op1][op2]==-1){
		return '<';
	}
	else if(OpPri[op1][op2]==0){
		return '=';
	}
	else if(OpPri[op1][op2]==1){
		return '>';
	}
	else {
		return '?';
	}
}

void PushOP(u8 op){
	Opcodepoint++;
	Opcode[Opcodepoint] = op;
}

u8 PopOP(){
	u8 op = Opcode[Opcodepoint];
	Opcodepoint--;
	return op;
}
void PushNum(){
	operandpoint++;
	Operand[operandpoint] = temp;
	//重置数字计数
	temp = 0;
	Decimalpoint= 0;   //重新开始小数
}

void Pushans(float ans){
	operandpoint++;
	Operand[operandpoint] = ans;
}
float PopNum(){
	float num = Operand[operandpoint];
	operandpoint--;
	return num;
}
float operate(float a, float b,u8 theta){
	if(theta==0){
		return a+b;
	}
	else if(theta ==1){
		return a-b;
	}
	else if(theta ==2){
		return a*b;
	}
	else 
		return a/b;
	
}
void handleOp(u8 op){
	char P = Precede(Opcode[Opcodepoint],op);
	u8 theta;
	float a, b;
	PushNum();          //todo: 如果最开始是符号有问题
	switch(P){
		case '<':
			PushOP(op);
			break;
		case '=':
			PopOP();
			break;
		case '>':
			theta = PopOP();
			b = PopNum();
		  a = PopNum();
			Pushans(operate(a,b,theta));
			break;
		case '?':
			break;
	}
}

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
}
void main(){
	u8 num ;
	LcdInit();
	//LcdWriteData(1+'0');
	InitOpcodeStack();
	while(1){
		switch(State){
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				break;
		}
		num = Getch();
		LcdWriteData(Outputchar[num]);
	}
	
}