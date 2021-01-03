#include "lcd.h"
#include "reg52.h"


typedef unsigned int u16;	  //���������ͽ�����������
typedef unsigned char u8;


char Outputchar[18]={48,49,50,51,52,53,54,55,56,57,43,45,42,47,40,41,46,61};
#define MAXLEN 15
char Opcodepoint = -1;  // +1 push ָ��ǰλ��
char Operandpoint = -1 ;
char Statepoint = -1;
float Operand[MAXLEN];  //Operadn[MAXLEN-1]Ϊ����������ʱ����λ��
float a = 0.0 , b = 0.0, c =0.0;

u8 Opcode[MAXLEN];
u8 StateStack[MAXLEN];

u8 times = 1;  //ת����float�ĳ��ȱ��
u8 Decimalpoint = 0 ;//����С����
u8 State = 0; //�ʼ�ڳ�ʼ״̬
u8 add_sub_flag = 0 ; //1->'+' , 0->'-'
u8 mlti_div_flag = 0 ; //1->'*' , 0->'/'

void changeState(){
}
void InitOpcodeStack(){
	Opcodepoint = -1;
}
void InitOperandStack(){
	Operandpoint = -1;
}
void InitStateStack(){
	Statepoint = -1;
}
/*
	���ȼ���
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
#define GPIO_BUTTON P3 //��������ʹ��P3
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
	if(GPIO_KEY!=0x0f)//��ȡ�����Ƿ���
	{
		delay(1000);//��ʱ10ms��������
		if(GPIO_KEY!=0x0f)//�ٴμ������Ƿ���
		{	
			//������
			GPIO_KEY=0X0F;
			switch(GPIO_KEY)
			{
				case(0X07):	KeyValue=0;break;
				case(0X0b):	KeyValue=1;break;
				case(0X0d): KeyValue=2;break;
				case(0X0e):	KeyValue=3;break;
			}
			//������
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
	while((a<50)&&(GPIO_KEY!=0xf0))	 //��ⰴ�����ּ��
	{
		delay(140);
		a++;
	}
	return KeyValue;
}


void Error(){
	
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
	Operandpoint++;
	Operand[Operandpoint] = temp;
	//�������ּ���
	temp = 0;
	Decimalpoint= 0;   //���¿�ʼС��
}

void Pushans(float ans){
	Operandpoint++;
	Operand[Operandpoint] = ans;
}
float PopNum(){
	float num = Operand[Operandpoint];
	Operandpoint--;
	return num;
}


/*
�Ӷ������������ַ�
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
}


void function_S0(){
	u8 num = Getch();
	if(isNum(num)==1){
		a = a*10 +num;
	}
	else if(num == 10){  // + -
		b = 0 ;
		add_sub_flag = 1;
		state = 1;
	}
	else if(num == 11){
		b = 0 ;
		add_sub_flag = 0 ;
		state = 1;
	}
	else if(num ==12 ){
		c = 0 ;
		state = 5;
		multi_div_flag = 1;
	}// * /
	else if(num == 13){
		c = 0 ;
		state =5 ;
		multi_div_flat = 0;
	}
	else if(num == 17){
	}// =
	LcdWriteData(Outputchar[num]);
}

void function_S1(){
}

void function_S2(){
}

void function_S3(){
}

void function_S4(){
}

void function_S5(){
}

void function_S6(){
}
void main(){
	LcdInit();
	InitOpcodeStack();
	InitOperandStack();
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
		}
	}
	
}