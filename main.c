#include "lcd.h"
#include "reg52.h"


typedef unsigned int u16;	  //���������ͽ�����������
typedef unsigned char u8;

#define MAXLEN 15
char Opcodepoint = -1;  // +1 push ָ��ǰλ��
char operandpoint = -1 ;
float Operand[MAXLEN];  //Operadn[MAXLEN-1]Ϊ����������ʱ����λ��
#define temp Operand[MAXLEN-1]
u8 Opcode[MAXLEN];
u8 times = 1;  //ת����float�ĳ��ȱ��
u8 Decimalpoint = 0 ;//����С����

void InitOpcodeStack(){
	Opcodepoint++;
	Opcode[Opcodepoint] = 6;
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
void handleNum(u8 num){
	u8 i ;
	float afterpoint = 0;
	afterpoint += num;
	if(Decimalpoint!=1){    //û��С����
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
	//�������ּ���
	temp = 0;
	Decimalpoint= 0;   //���¿�ʼС��
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
	PushNum();          //todo: ����ʼ�Ƿ���������
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


void Getch(){
	u8 op = KeyDown();
	while(op==127){
		op = KeyDown();
	}
	if(op!=127){
		if(isNum(op)==1){
			handleNum(op);
			LcdWriteData(op+'0');
		}
		else if(op==10){ 
			handleOp(0);
			LcdWriteData('+');
		}
		else if(op==11){
			handleOp(1);
			LcdWriteData('-');
		}
		else if(op==12){
			handleOp(2);
			LcdWriteData('*');
		}
		else if(op==13){
			handleOp(3);
			LcdWriteData('/');
		}
		else if(op==14){
			if(Decimalpoint==0){
					Decimalpoint =1;
					LcdWriteData('.');
			}
			else{
				Error();
			}
		}
		else if(op==15){
			handleOp(6);
			LcdWriteData('=');
			LcdWriteData(Operand[operandpoint]);
		}
	}
}
void main(){
	
	LcdInit();
	//LcdWriteData(1+'0');
	InitOpcodeStack();
	Getch();
	while(1){
			Getch();
			
	}
	
}