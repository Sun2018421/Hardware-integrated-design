#include "lcd.h"
#include "reg52.h"


typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;


void main(){
	u8 Display[] = "Sun xiao feng";
	u8 size = sizeof(Display);
	u8 i ;
	LcdInit();
	for(i=0;i<13;i++){
		LcdWriteData(Display[i]);
	}
	while(1);
	
}