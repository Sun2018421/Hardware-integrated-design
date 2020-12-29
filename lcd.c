#include "lcd.h"

void Lcd1602_Delay1ms(uint c){
	uchar a , b ;
	for(	;c>0 ;	c--){
			for(b = 199; b>0 ; b--){
					for(a = 1 ; a > 0 ; a--);
			}
	}
}

void LcdWriteCom(uchar com){
	LCD1602_E = 0 ;
	LCD1602_RS = 0 ;
	LCD1602_RW = 0 ;

	LCD1602_DATAPINS = com;
	Lcd1602_Delay1ms(1);
	
	LCD1602_E = 1;	
	Lcd1602_Delay1ms(5);
	LCD1602_E=0;
}

void LcdWriteData(uchar dat){
	LCD1602_E = 0 ;
	LCD1602_RS = 1 ;
	LCD1602_RW = 0 ;

	LCD1602_DATAPINS = dat;
	Lcd1602_Delay1ms(1);
	
	LCD1602_E = 1;	
	Lcd1602_Delay1ms(5);
	LCD1602_E=0;
}

void LcdInit(){
	LcdWriteCom(0x38);  //����ʾ  0011 1000
	LcdWriteCom(0x0c);  //����ʾ����ʾ��� 0000 1100
	LcdWriteCom(0x06);  //дһ��ָ���1 0000 0110
	LcdWriteCom(0x01);  //���� 0000 0001
	LcdWriteCom(0x80);  //��������ָ����� 1000 0000
}