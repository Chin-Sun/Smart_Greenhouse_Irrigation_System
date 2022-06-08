#include<reg51.h>	//加载头文件
#include<intrins.h>

#include "uart.h"

#define uchar unsigned char	//宏定义
#define uint unsigned int
#define ulong unsigned long
#define LCD_START_ROW 0xc0	//起始行命令
#define LCD_PAGE 0xB8		//页指令
#define LCD_COL  0x40		//列指令
#define LCD_DATA P0	     	//液晶数据线端口DB0-DB7
sbit LCD_RS=P2^0;			//LCD液晶引脚定义
sbit LCD_RW=P2^1;
sbit LCD_E=P2^2;
sbit LCD_CS1=P2^3;
sbit LCD_CS2=P2^4;
sbit DIO=P2^5;	  //DS1302数据线
sbit DCLK=P2^6;	  //DS1302时钟线
sbit DRST=P2^7;	  //DS1302复位线
sbit KEY_SET=P3^4;	   //设置键
sbit KEY_JIA=P3^5;	   //加1
sbit KEY_JIAN=P3^6;	   //减1
sbit KEY_OUT=P3^7;	   //退出
sbit JDQ1=P1^0;		//温度高于上限
sbit JDQ2=P1^1;		//温度低于下限
sbit JDQ3=P1^2;		//湿度高于上限
sbit JDQ4=P1^3;		//湿度低于下限
sbit LED=P1^5;		//LED报警接口
sbit SPK=P1^6;		//蜂鸣器报警接口
sbit DHT=P1^7;		//DHT11接口定义
uchar TempH=35,TempL=15;	//温度上下限
uchar HumiH=80,HumiL=50;	//湿度上下限
uchar s1,s2,w1,w2;	//湿度，温度变量
uchar TH_data,TL_data,RH_data,RL_data,CK_data;	//DHT11用变量
uchar TH_temp,TL_temp,RH_temp,RL_temp,CK_temp;
uchar com_data,untemp,temp;
uchar respond;
bit Reverse_Display=0;//是否反相显示（白底黑字/黑底白字）
char Adjust_Index=-1;  //当前调节的时间对象：秒，分，时，日，月，年（0，1，2，3，4，6）
uchar MonthsDays[]={0,31,0,31,30,31,30,31,31,30,31,30,31};   // 一年中每个月的天数，二月的天数由年份决定
uchar DateTime[7];//所读取的日期时间

uchar tcount=0;	//定时器用变量	

uchar code TAB_16X16[]={	
//星期夜晚图标空设置年月日时分秒	
0x00,0x08,0xFF,0xA8,0xA8,0xA8,0xFF,0x08,0x00,0xFE,0x22,0x22,0x22,0xFF,0x02,0x00,
0x84,0x44,0x37,0x04,0x04,0x14,0xA7,0x44,0x24,0x1F,0x02,0x42,0x82,0x7F,0x00,0x00,//期，0

0x00,0x00,0xBE,0x2A,0x2A,0x2A,0x2A,0xEA,0x2A,0x2A,0x2A,0x2A,0xBF,0x02,0x00,0x00,
0x48,0x44,0x4B,0x49,0x49,0x49,0x49,0x7F,0x49,0x49,0x49,0x4D,0x49,0x61,0x40,0x00,//星，1
	
0xFF,0xFF,0xFF,0xFF,0xFD,0xFF,0xFD,0xFD,0xFD,0xFB,0xF3,0xC7,0x0F,0x1F,0xFF,0xFF,
0xFF,0xF7,0xEF,0xDF,0xDF,0xBF,0xBF,0xBF,0x9F,0xDF,0xCF,0xE3,0xF0,0xFC,0xFF,0xFF,//夜晚图标，2

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//空,3 

0x40,0x40,0x42,0xCC,0x00,0x40,0xA0,0x9F,0x81,0x81,0x81,0x9F,0xA0,0x20,0x20,0x00,
0x00,0x00,0x00,0x7F,0xA0,0x90,0x40,0x43,0x2C,0x10,0x28,0x26,0x41,0xC0,0x40,0x00,//设，4

0x00,0x10,0x17,0xD5,0x55,0x57,0x55,0x7D,0x55,0x57,0x55,0xD5,0x17,0x10,0x00,0x00,
0x40,0x40,0x40,0x7F,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x7F,0x40,0x60,0x40,0x00,//置，5

0x00,0x20,0x10,0xCC,0x47,0x44,0x44,0xFC,0x44,0x44,0x44,0x64,0x46,0x04,0x00,0x00,
0x04,0x04,0x04,0x07,0x04,0x04,0x04,0xFF,0x04,0x04,0x04,0x04,0x04,0x06,0x04,0x00,//年，6

0x00,0x00,0x00,0x00,0xFE,0x22,0x22,0x22,0x22,0x22,0x22,0xFF,0x02,0x00,0x00,0x00,
0x00,0x80,0x40,0x30,0x0F,0x02,0x02,0x02,0x02,0x42,0x82,0x7F,0x00,0x00,0x00,0x00,//月，7

0x00,0x00,0x00,0xFE,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0xFF,0x02,0x00,0x00,0x00,
0x00,0x00,0x00,0x7F,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x7F,0x00,0x00,0x00,0x00,//日，8

0x00,0xFC,0x84,0x84,0x84,0xFE,0x14,0x10,0x90,0x10,0x10,0x10,0xFF,0x10,0x10,0x00,
0x00,0x3F,0x10,0x10,0x10,0x3F,0x00,0x00,0x00,0x23,0x40,0x80,0x7F,0x00,0x00,0x00,//时,9

0x00,0x80,0x40,0x20,0x98,0x86,0x80,0x80,0x83,0x8C,0x90,0x20,0xC0,0x80,0x80,0x00,
0x01,0x00,0x80,0x40,0x20,0x1F,0x00,0x40,0x80,0x40,0x3F,0x00,0x00,0x01,0x00,0x00,//分，10

0x20,0x24,0x24,0xA4,0xFE,0xA3,0x22,0x80,0x70,0x00,0xFF,0x00,0x10,0x20,0x60,0x00,
0x10,0x08,0x06,0x01,0xFF,0x00,0x81,0x80,0x40,0x20,0x17,0x08,0x04,0x03,0x00,0x00,//秒，11

0x00,0x06,0x09,0x09,0xE6,0x10,0x08,0x08,0x08,0x08,0x18,0x30,0x78,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x1F,0x20,0x40,0x40,0x40,0x40,0x40,0x20,0x10,0x00,0x00,0x00,//℃，12

0x10,0x60,0x02,0x8C,0x00,0x00,0xFE,0x92,0x92,0x92,0x92,0x92,0xFE,0x00,0x00,0x00,
0x04,0x04,0x7E,0x01,0x40,0x7E,0x42,0x42,0x7E,0x42,0x7E,0x42,0x42,0x7E,0x40,0x00,//温，13

0x10,0x60,0x02,0x8C,0x00,0xFE,0x92,0x92,0x92,0x92,0x92,0x92,0xFE,0x00,0x00,0x00,
0x04,0x04,0x7E,0x01,0x44,0x48,0x50,0x7F,0x40,0x40,0x7F,0x50,0x48,0x44,0x40,0x00,//湿，14

0x00,0x00,0xFC,0x24,0x24,0x24,0xFC,0x25,0x26,0x24,0xFC,0x24,0x24,0x24,0x04,0x00,
0x40,0x30,0x8F,0x80,0x84,0x4C,0x55,0x25,0x25,0x25,0x55,0x4C,0x80,0x80,0x80,0x00,//度，15

0x00,0x00,0x38,0x44,0x44,0x44,0x38,0x00,0x80,0x40,0x20,0x10,0x08,0x04,0x00,0x00,
0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x01,0x38,0x44,0x44,0x44,0x38,0x00,0x00,0x00,//%,16

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x20,0x20,0x20,0x30,0x20,0x00,0x00,0x00,
0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x7F,0x40,0x40,0x40,0x40,0x40,0x60,0x40,0x00,//上,17

0x02,0x02,0x02,0x02,0x02,0x02,0xFE,0x02,0x22,0x42,0x82,0x82,0x02,0x03,0x02,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,//下,18

0x00,0xFE,0x02,0x22,0xDA,0x06,0x00,0xFE,0x92,0x92,0x92,0x92,0xFF,0x02,0x00,0x00,
0x00,0xFF,0x08,0x10,0x08,0x07,0x00,0xFF,0x42,0x24,0x08,0x14,0x22,0x61,0x20,0x00,//限,19

0x00,0x00,0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//:,20
};
uchar code TAB_WEEK[]={
//日一二三四五六
0x00,0x00,0x00,0xFE,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0xFF,0x02,0x00,0x00,0x00,
0x00,0x00,0x00,0x7F,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x7F,0x00,0x00,0x00,0x00,//日，0

0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xC0,0x80,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//一，1

0x00,0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x0C,0x08,0x00,0x00,0x00,
0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x18,0x10,0x00,//二，2

0x00,0x04,0x84,0x84,0x84,0x84,0x84,0x84,0x84,0x84,0x84,0xC4,0x86,0x04,0x00,0x00,
0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x30,0x20,0x00,//三，3

0x00,0xFC,0x04,0x04,0x04,0xFC,0x04,0x04,0x04,0xFC,0x04,0x04,0x04,0xFE,0x04,0x00,
0x00,0x7F,0x20,0x28,0x24,0x23,0x20,0x20,0x20,0x21,0x22,0x22,0x22,0x7F,0x00,0x00,//四，4

0x00,0x04,0x84,0x84,0x84,0x84,0xFC,0x84,0x84,0x84,0x84,0xC4,0x86,0x84,0x00,0x00,
0x40,0x40,0x40,0x40,0x78,0x47,0x40,0x40,0x40,0x40,0x40,0x7F,0x40,0x60,0x40,0x00,//五，5

0x20,0x20,0x20,0x20,0x20,0x22,0x2C,0x38,0x20,0x20,0x20,0x20,0x20,0x30,0x20,0x00,
0x00,0x40,0x20,0x18,0x06,0x03,0x00,0x00,0x00,0x01,0x02,0x0C,0x38,0x70,0x00,0x00,//六，6
};
uchar code TAB_8X16[]={
//8X16点阵数字字符 
0x00,0xF0,0x08,0x04,0x04,0x04,0x08,0xF0,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,//0,0
0x00,0x00,0x08,0x08,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,//1,1
0x00,0x18,0x04,0x04,0x04,0x84,0x78,0x00,0x00,0x38,0x24,0x22,0x21,0x20,0x20,0x00,//2,2
0x00,0x18,0x84,0x84,0x84,0x84,0x78,0x00,0x00,0x18,0x20,0x20,0x20,0x20,0x1F,0x00,//3,3
0x00,0x00,0x00,0xC0,0x30,0x0C,0xFC,0x00,0x00,0x04,0x07,0x04,0x04,0x04,0x3F,0x04,//4,4
0x00,0xFC,0x44,0x44,0x44,0x44,0x84,0x00,0x00,0x18,0x20,0x20,0x20,0x20,0x1F,0x00,//5,5
0x00,0xF8,0x84,0x84,0x84,0x84,0x18,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//6,6
0x00,0x04,0x04,0x04,0xC4,0x34,0x0C,0x00,0x00,0x00,0x00,0x3E,0x01,0x00,0x00,0x00,//7,7
0x00,0x78,0x84,0x84,0x84,0x84,0x78,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//8,8
0x00,0xF8,0x04,0x04,0x04,0x04,0xF8,0x00,0x00,0x18,0x21,0x21,0x21,0x21,0x1F,0x00,//9,9
0x00,0x38,0x44,0x44,0xB8,0x60,0x18,0x04,0x00,0x20,0x18,0x06,0x1D,0x22,0x22,0x1C,//%,10
0x00,0xFC,0x04,0x04,0x04,0x04,0x88,0x70,0x00,0x3F,0x01,0x01,0x01,0x01,0x02,0x3C,//R,11
0x00,0xFC,0x80,0x80,0x80,0x80,0x80,0xFC,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x3F,//H,12
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x00,//.,13
0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//-,14
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x33,0x00,0x00,0x00,0x00,//:,15
};
void Delay_MS(uchar ms)	//MS延时函数
{       
	uchar i;
	while(ms--)
	{
		for(i=0;i<250;i++)
		{
			_nop_();_nop_();_nop_();_nop_();
		}
	}
}
void Bi(ulong t)		//蜂鸣器响函数
{
	ulong c;
	uint n;
	for(c=0;c<t;c++)
	{
		for(n=0;n<50;n++);	//延时
		SPK=~SPK;		    //取反输出到喇叭的信号
	}
}
void Lcd_Scan_Busy() 	//LCD查忙函数
{
	LCD_RW=1;
	LCD_RS=0;
}
void Lcd_W_Com(uchar com) 	//LCD写指令函数
{
	Lcd_Scan_Busy();  
	LCD_RW=0;
	LCD_RS=0;
	LCD_DATA=com;
	LCD_E=1;
	LCD_E=0;
}
void Lcd_W_Dat(uchar dat)	//LCD写数据函数
{
	Lcd_Scan_Busy();  
	LCD_RW=0;
	LCD_RS=1;
	if(!Reverse_Display)	   //根据Reverse_Display决定是否反相显示
		LCD_DATA=dat;
	else 
		LCD_DATA=~dat;
	LCD_E=1;
	LCD_E=0;
}
void Lcd_Set_Line(uchar Line) 	//设置行地址(页面)0-7
{
	Line=Line & 0x07;           //0<=line<=7
	Line=Line | 0xb8;           //1011 1xxx
	Lcd_W_Com(Line);	        //向LCD发送命令
}
void Lcd_Select_Screen(uchar screen)	//选择屏幕函数	
{ 
	switch(screen)
	{   
		case 0:
			LCD_CS1=1;
			LCD_CS2=0;
			break;		 //左屏
	    case 1: 
			LCD_CS1=0;
			LCD_CS2=1;
			break;		 //右屏
	}
}
void Lcd_Clr_Scr(uchar screen) 	//清屏函数,=0是左屏，=1是右屏
{ 
	uchar i,j;
	Lcd_Select_Screen(screen);
	for(i=0;i<8;i++) 
	{ 
		Lcd_Set_Line(i); 
		for(j=0;j<64;j++) 
		{ 
			Lcd_W_Dat(0x00); 
		} 
	}	
}
void Lcd_Init()	//LCD初始化函数
{
  	LCD_CS1=1;
  	LCD_CS2=1;
	Lcd_Clr_Scr(0);
	Lcd_Clr_Scr(1);
  	Lcd_W_Com(0x38);
	Delay_MS(10);
  	Lcd_W_Com(0x0F);
	Delay_MS(10);
  	Lcd_W_Com(0x06);
	Delay_MS(10);
  	Lcd_W_Com(0x01);
	Delay_MS(10);
}
//从第P页第L列显示W个字节数据，具体显示的数据在r所指的数组中
void Lcd_Show(uchar P,uchar L,uchar W,uchar *r) reentrant 	//液晶显示函数
{  
    uchar i;
	TR0=0;
	if(L<64)
	{
		LCD_CS1=1;
		LCD_CS2=0;
		Lcd_W_Com(LCD_PAGE+P);
		Lcd_W_Com(LCD_COL+L);
		if(L+W<64)
		{ 
			for(i=0;i<W;i++)
				Lcd_W_Dat(r[i]);
		}
		else
		{
			for(i=0;i<64-L;i++)
				Lcd_W_Dat(r[i]);
			LCD_CS1=0;
			LCD_CS2=1;
			Lcd_W_Com(LCD_PAGE+P);
			Lcd_W_Com(LCD_COL);
			for(i=64-L;i<W;i++)
				Lcd_W_Dat(r[i]);
		}
	}
	else
	{
		LCD_CS1=0;
		LCD_CS2=1;
		Lcd_W_Com(LCD_PAGE+P);
		Lcd_W_Com(LCD_COL+L-64);
		for(i=0;i<W;i++)
			Lcd_W_Dat(r[i]);
	}		
	TR0=1;
}
void Lcd_Show_8X16(uchar P1,uchar L1,uchar *M) reentrant	//显示8X16点阵字符
{
   Lcd_Show(P1,L1,8,M);
   Lcd_Show(P1+1,L1,8,M+8); 
}
//汉字上半部分与下半部分分别处在相邻两页中
void Lcd_Show_16X16(uchar P2,uchar L2,uchar *M) reentrant	 //显示16X16点阵字符
{											 
   Lcd_Show(P2,L2,16,M); 
   Lcd_Show(P2+1,L2,16,M+16); 
}
void Delay_5US()		//5US延时函数
{
	uchar i;
    i--;i--;i--;i--;i--;i--;
}
char DHT_Receive() 	//接收信号
{    
	uchar i;
	com_data=0;
	for(i=0;i<=7;i++)    
	{
		respond=2;
		while((!DHT)&&respond++);
		Delay_5US();
		Delay_5US();
		Delay_5US();
		if(DHT)
		{
			temp=1;
			respond=2;
			while((DHT)&&respond++); 
		} 
		else
			temp=0;
		com_data<<=1;
		com_data|=temp;    
	}
	return(com_data);  
}
//湿度读取子程序 
//温度高8位== TL_data
//温度低8位== TH_data
//湿度高8位== RH_data
//湿度低8位== RH_data
//校验 8位 == CK_data
//调用的程序有 delay();, Delay_5us();,DHT_Receive(); 
void DHT_Read()	//读取数据
{
	//主机拉低18ms 
	DHT=0;
	Delay_MS(18);
	DHT=1;
	//DATA总线由上拉电阻拉高 主机延时20us
	Delay_5US();
	Delay_5US();
	Delay_5US();
	Delay_5US();
	
	//主机设为输入 判断从机响应信号 
	DHT=1;
	//判断DHT11是否有低电平响应信号 如不响应则跳出，响应则向下运行   
	if(!DHT)   
	{
		respond=2;
		//判断DHT11发出 80us 的低电平响应信号是否结束 
		while((!DHT)&& respond++);
		respond=2;
		//判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
		while(DHT && respond++);
		//数据接收状态   
		RH_temp = DHT_Receive(); 
		RL_temp = DHT_Receive(); 
		TH_temp = DHT_Receive(); 
		TL_temp = DHT_Receive(); 
		CK_temp = DHT_Receive();
		DHT=1;   
		//数据校验 
		untemp=(RH_temp+RL_temp+TH_temp+TL_temp);
		if(untemp==CK_temp)
		{
			RH_data = RH_temp;
			RL_data = RL_temp;
			TH_data = TH_temp; 
			TL_data = TL_temp;
			CK_data = CK_temp;
		}
	}
	s1 = (char)(RH_data/10);
	s2 = (char)(RH_data%10);
	w1 = (char)(TH_data/10);
	w2 = (char)(TH_data%10);
}
void DS_Write_Byte(uchar X)    //向DS1302写入一个字节
{
	uchar i;
	for(i=0;i<8;i++)
	{
	  	DIO=X&1;
		DCLK=1;
		DCLK=0;
		X>>=1;
	}
}
uchar DS_Read_Byte()		//从DS1302中读取一个字节
{
	uchar i,byte,t;
   	for(i=0;i<8;i++)
   	{
		byte>>=1;
	 	t=DIO;
	  	byte|=t<<7;
	  	DCLK=1;
	  	DCLK=0;
	}
	//BCD码转换
	return byte/16*10+byte%16;
}
uchar DS_Read_Dat(uchar addr)  //从DS1302指定位置读取数据
{
	uchar dat;
	DRST=0;
	DCLK=0;
	DRST=1;
	DS_Write_Byte(addr);  //向DS1302写入一个地址
	dat=DS_Read_Byte(); //在上面写入的地址中读取数据
	DCLK=1;
	DRST=0;
	return dat;
}
void DS_Write_Dat(uchar addr,uchar dat)  //向DS1302指定位置写入数据, 写数据
{
	DCLK=0;
	DRST=1;
	DS_Write_Byte(addr);
	DS_Write_Byte(dat);
	DCLK=1;
	DRST=0;
}
void DS_Set_Time()	  //设置时间
{
  	uchar i;
   	DS_Write_Dat(0x8E,0x00); //写控制字，取消写保护
   	// 分，时，日，月，年依次写入
   	for(i=0;i<7;i++)
   	{
		//分的起始地址是10000010(0x82)，后面依次是时，日，月，周，年，写入地址每次递增2
	   	DS_Write_Dat(0x80+2*i,(DateTime[i]/10<<4)|(DateTime[i]%10));
	}
   	DS_Write_Dat(0x8E,0x80); //写控制字，加写保护
}
void DS_Get_Time()		  //读取当前时间
{
	uchar i;
  	for(i=0;i<7;i++)
    {
		DateTime[i]=DS_Read_Dat(0x81+2*i);
	}
}
uchar Is_Leapyear(uint year) //判断是否为闰年
{
	return (year%4==0&&year%100!=0)||(year%400==0) ;
}
//计算自2000.1.1开始的任何一天是星期几？
void Refresh_Week_Day()	//计算星期
{
	uint i,d,w=5;  //已知1999年12.31是星期五
	for(i=2000;i<2000+DateTime[6];i++)
   	{
	  	d=Is_Leapyear(i)?366:365;
		w=(w+d)%7;
	}
	d=0;
	for (i=1;i<DateTime[4];i++)
	{
		d+=MonthsDays[i];
	}
	d+=DateTime[3];//保存星期，0-6表示星期日，星期一至星期六，为了与DS1302的星期格式匹配，返回值需要加1
	DateTime[5]=(w+d)%7+1;
}
void Date_Time_Adjust(char X) 	//年，月，日和时，分++/--
{
	switch(Adjust_Index)
    {	
		case 10:	//温度下限调整
			if(X==1&&TempL<TempH)	//不高于上限，加
				TempL++;
			if(X==-1&&TempL>0)	  	//不小于0，减
				TempL--;
			break;
		case 9:		//温度上限调整
			if(X==1&&TempH<50) 		//不高于50，加
				TempH++;
			if(X==-1&&TempH>TempL) 	//不大于下限，减
				TempH--;
			break;
		case 8:	//湿度下限调整
			if(X==1&&HumiL<HumiH)	//不高于上限，加
				HumiL++;
			if(X==-1&&HumiL>20)	  	//不小于20，减
				HumiL--;
			break;
		case 7:		//湿度上限调整
			if(X==1&&HumiH<90) 		//不高于90，加
				HumiH++;
			if(X==-1&&HumiH>HumiL) 	//不大于下限，减
				HumiH--;
			break;

		case 6:	 //年调整，00-99
			if(X==1&&DateTime[6]<99)
			{ 
				DateTime[6]++;
			}
			if(X==-1&&DateTime[6]>0)
			{ 
				DateTime[6]--;
			}
			//获取2月天数
			MonthsDays[2]=Is_Leapyear(2000+DateTime[6])?29:28;
			//如果年份变化后当前月份的天数大于上限则设为上限
			if(DateTime[3]>MonthsDays[DateTime[4]])
			{
				DateTime[3]=MonthsDays[DateTime[4]];
			}
			Refresh_Week_Day();  //刷新星期
			break;
	   case 4:  //月调整 01-12
	    	if(X==1&&DateTime[4]<12)
			{ 
				DateTime[4]++;
			}
			if(X==-1&&DateTime[4]>1)
			{ 
				DateTime[4]--;
			}
			//获取2月天数
			MonthsDays[2]=Is_Leapyear(2000+DateTime[6])?29:28;
			//如果年份变化后当前月份的天数大于上限则设为上限
			if(DateTime[3]>MonthsDays[DateTime[4]])
			{
				DateTime[3]=MonthsDays[DateTime[4]];
			}
			Refresh_Week_Day();  //刷新星期
			break;
	   case 3:	 //	 日调整00-28或00-29或00-30或00-31
	          	//调节之前首先根据当前年份得出该年中2月的天数
			MonthsDays[2]=Is_Leapyear(2000+DateTime[6])?29:28;
			//根据当前月份决定调节日期的上限
			if(X==1&&DateTime[3]<MonthsDays[DateTime[4]])
			{ 
				DateTime[3]++;
			}
			if(X==-1&&DateTime[3]>1)
			{ 
				DateTime[3]--;
			}
			Refresh_Week_Day();  //刷新星期
	        break;
	   case 2:	  //时调整
			if(X==1&&DateTime[2]<23)
			{ 
				DateTime[2]++;
			}
			if(X==-1&&DateTime[2]>0)
			{ 
				DateTime[2]--;
			}
	        break;
	   case 1:	  //  分调整
			if(X==1&&DateTime[1]<59)
			{ 
				DateTime[1]++;
			}
			if(X==-1&&DateTime[1]>0)
			{ 
				DateTime[1]--;
			}
	        break;
		case 0:	  // 秒调整
			if(X==1&&DateTime[0]<59)
			{ 
				DateTime[0]++;
			}
			if(X==-1&&DateTime[0]>0)
			{ 
				DateTime[0]--;
			}
	        break;
	}
}
void Timer_Init()	//定时器初始化
{
	IE=0x83;
	IP=0x01;
	IT0=0X01;
	TH0=(65536-50000)/256;	//定时器赋初值
	TL0=(65536-50000)%256;
	TR0=1;		//定时器0启动
}



void System_Init()	//系统初始化函数
{
	Lcd_Init();		//液晶初始化函数
	Timer_Init();	//定时器初始化
	InitUART();
	
	
	Lcd_Show_8X16(0,0+0,TAB_8X16+2*16);//显示2
	Lcd_Show_8X16(0,8+0,TAB_8X16);     //显示0

	Lcd_Show_8X16(0,32+0,TAB_8X16+14*16);    //-
	Lcd_Show_8X16(0,56+0,TAB_8X16+14*16);    //-
	Lcd_Show_16X16(0,80+0,TAB_16X16+1*32);  //星
	Lcd_Show_16X16(0,96+0,TAB_16X16+0*32);  //期

	Lcd_Show_16X16(4,0,TAB_16X16+13*32);  	//温度
	Lcd_Show_16X16(4,16,TAB_16X16+15*32);
	Lcd_Show_16X16(4,32,TAB_16X16+20*32); 	//冒号    
	Lcd_Show_16X16(4,64,TAB_16X16+12*32); 	//℃
	
	Lcd_Show_16X16(6,0,TAB_16X16+14*32);  	//湿度
	Lcd_Show_16X16(6,16,TAB_16X16+15*32);
	Lcd_Show_16X16(6,32,TAB_16X16+20*32); 	//冒号  
	Lcd_Show_16X16(6,64,TAB_16X16+16*32); 	//%
}

void delay1s(void)
{
unsigned char h,i,j,k;
for(h=5;h>0;h--)
for(i=4;i>0;i--)
for(j=116;j>0;j--)
for(k=214;k>0;k--);
} //延时1min
void main()	//主函数
{
    System_Init();	//系统初始化函数
	while(1)
	{
		DHT_Read();	//获取温湿度
		if(Adjust_Index==-1) 
		{
			DS_Get_Time();	//获取日期时间信息
			Lcd_Show_16X16(2,64+0,TAB_16X16+3*32);  //空
			Lcd_Show_16X16(2,80+0,TAB_16X16+3*32); //空
			Lcd_Show_16X16(2,96+0,TAB_16X16+3*32); //空
			Lcd_Show_16X16(2,112+0,TAB_16X16+3*32); //空
			
			if((RH_data>HumiH)||(RH_data<HumiL)||(TH_data>TempH)||(TH_data<TempL)) 	//超范围声光报警
			{
				LED=0; 	//声光报警
				Bi(50);
			}
			else	  	//正常范围
			{
				LED=1; 	//声光不报警
				SPK=1;
			}
			if(TH_data>TempH)	//温度高于上限；温度>35
				JDQ1=0;			//继电器1闭合；风扇打开
			else
				JDQ1=1;			//继电器1断开；风扇关闭
			if(TH_data<TempL) 	//温度低于下限;	温度< 15
				JDQ2=0;			//继电器2闭合 ; 取暖器打开
			else
				JDQ2=1;			//继电器2断开； 取暖器关闭
			if(RH_data>HumiH)  	//湿度高于上限 ；湿度>80
				JDQ3=0;			//继电器3闭合	 除湿机打开
			else
				JDQ3=1;			//继电器3断开	 除湿机关闭
			if(RH_data<HumiL) 	//湿度低于下限 ；湿度< 50
				JDQ4=0;			  //继电器4闭合  ；抽水泵打开			
      else if(TH_data>TempH)	//温度高于上限；温度>35
				JDQ4=0;			//继电器1闭合；风扇打开			
			else
				JDQ4=1;			//继电器4断开；浇水关闭
			if(JDQ4==0)
				delay1s();
			  JDQ4=1;
			  delay1s();//延时1s
		}
		else
		{
			if((Adjust_Index==10)||(Adjust_Index==9))	//温度
			{
				Lcd_Show_16X16(2,64+0,TAB_16X16+13*32);  //温度
				Lcd_Show_16X16(2,80+0,TAB_16X16+15*32); 
			}	
			else if((Adjust_Index==8)||(Adjust_Index==7))	//湿度
			{
				Lcd_Show_16X16(2,64+0,TAB_16X16+14*32);  	//湿度
				Lcd_Show_16X16(2,80+0,TAB_16X16+15*32); 
			}
			else
			{
				Lcd_Show_16X16(2,64+0,TAB_16X16+4*32);  //设
				Lcd_Show_16X16(2,80+0,TAB_16X16+5*32); 	//置
				Lcd_Show_16X16(2,112+0,TAB_16X16+3*32); //空
			}

			if((Adjust_Index==10)||(Adjust_Index==8))		//下限
			{
				Lcd_Show_16X16(2,96+0,TAB_16X16+18*32); 	//下
				Lcd_Show_16X16(2,112+0,TAB_16X16+19*32); 	//限
			}
			else if((Adjust_Index==9)||(Adjust_Index==7))	//上限
			{
				Lcd_Show_16X16(2,96+0,TAB_16X16+17*32); 	//上
				Lcd_Show_16X16(2,112+0,TAB_16X16+19*32); 	//限
			}
			else if(Adjust_Index==6)	//年
				Lcd_Show_16X16(2,96+0,TAB_16X16+6*32); //年
			else if(Adjust_Index==4)	//月
				Lcd_Show_16X16(2,96+0,TAB_16X16+7*32); //月
			else if(Adjust_Index==3)	//日
				Lcd_Show_16X16(2,96+0,TAB_16X16+8*32); //日
			else if(Adjust_Index==2)	//时
				Lcd_Show_16X16(2,96+0,TAB_16X16+9*32); //时
			else if(Adjust_Index==1)	//分
				Lcd_Show_16X16(2,96+0,TAB_16X16+10*32); //分
			else if(Adjust_Index==0)	//秒
				Lcd_Show_16X16(2,96+0,TAB_16X16+11*32); //秒	
		} 
	 	Lcd_Show_8X16(0,16+0,TAB_8X16+DateTime[6]/10*16);	//年后两位
	 	Lcd_Show_8X16(0,24+0,TAB_8X16+DateTime[6]%10*16);

		Lcd_Show_8X16(0,40+0,TAB_8X16+DateTime[4]/10*16);	//月
	 	Lcd_Show_8X16(0,48+0,TAB_8X16+DateTime[4]%10*16);
	
	 	Lcd_Show_8X16(0,64+0,TAB_8X16+DateTime[3]/10*16);	//日
		Lcd_Show_8X16(0,72+0,TAB_8X16+DateTime[3]%10*16);

	 	Lcd_Show_16X16(0,112+0,TAB_WEEK+(DateTime[5]%10-1)*32); //星期

	 	Lcd_Show_8X16(2,0+0,TAB_8X16+DateTime[2]/10*16);	//时
		Lcd_Show_8X16(2,8+0,TAB_8X16+DateTime[2]%10*16);    
		Lcd_Show_8X16(2,16+0,TAB_8X16+15*16);	//冒号		
		Lcd_Show_8X16(2,24+0,TAB_8X16+DateTime[1]/10*16);	//分
		Lcd_Show_8X16(2,32+0,TAB_8X16+DateTime[1]%10*16);
		Lcd_Show_8X16(2,40+0,TAB_8X16+15*16);	//冒号		    

		Lcd_Show_8X16(4,48,TAB_8X16+w1*16);
		Lcd_Show_8X16(4,56,TAB_8X16+w2*16);		//温度
		
		Lcd_Show_8X16(6,48,TAB_8X16+s1*16);
		Lcd_Show_8X16(6,56,TAB_8X16+s2*16);		//湿度

		Lcd_Show_8X16(4,88,TAB_8X16+TempL/10*16);
		Lcd_Show_8X16(4,96,TAB_8X16+TempL%10*16);		//温度下限
		Lcd_Show_8X16(4,104,TAB_8X16+14*16);    		//-
		Lcd_Show_8X16(4,112,TAB_8X16+TempH/10*16);
		Lcd_Show_8X16(4,120,TAB_8X16+TempH%10*16);		//温度上限

		Lcd_Show_8X16(6,88,TAB_8X16+HumiL/10*16);
		Lcd_Show_8X16(6,96,TAB_8X16+HumiL%10*16);		//湿度下限
		Lcd_Show_8X16(6,104,TAB_8X16+14*16);    		//-
		Lcd_Show_8X16(6,112,TAB_8X16+HumiH/10*16);
		Lcd_Show_8X16(6,120,TAB_8X16+HumiH%10*16);		//湿度上限
	}
}
void EXIT0()interrupt 0		//外部中断0中断处理函数，用于键盘
{
	if(KEY_SET==0)
	{
		if(Adjust_Index==-1||Adjust_Index==0)
		{
			Adjust_Index=11;
		}
		Adjust_Index--;
		if(Adjust_Index==5)
		{
			Adjust_Index=4;
		}	 //跳过对星期的调节
	}
   	else if(KEY_JIA==0)			 //加
	{
		Date_Time_Adjust(1);
	}
    else if(KEY_JIAN==0)			//减
	{
		Date_Time_Adjust(-1);
	}
    else if(KEY_OUT==0)
	{
		DS_Set_Time();	//设置日期时间
		Adjust_Index=-1;  //操作索引重设为-1，时间继续正常显示
	}
}
void Timer0()interrupt 1 	//定时器0中断处理函数，用于刷新秒显示
{
	TH0=(65536-50000)/256;	//定时器赋初值
	TL0=(65536-50000)%256;
	if(++tcount!=2)
		return;
	tcount=0;
	Lcd_Show_8X16(2,48,TAB_8X16+DateTime[0]/10*16);	//秒显示           
	Lcd_Show_8X16(2,56,TAB_8X16+DateTime[0]%10*16);
	
	UART_IDLE();
}

