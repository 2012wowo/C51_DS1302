   /*******************************************************************************************
程序名： LCD1602显示万年历
编写人： 花花雪
编写时间： 2016 年 7月25日
硬件支持：AT89S52 /LCD1602 /DS1302
接口说明：
修改日志：
NO.1
/*******************************************************************************************
//说明 ：
			
*******************************************************************************************/
  //[单片机常用SFR头文件]
# include <REG52.h> 
 // [常用定义声明]
# define uint unsigned int
# define uchar unsigned char

uchar code init_display1[]="2010-09-08";//液晶屏初始化显示第一行数据
uchar code init_display2[]="14:33:12";//液晶屏初始化显示第一行数据
uchar code init_display3[]="0123456789";
sbit lcden=P3^4;		//LCD使能端接的哪个端口就写哪个
sbit lcdrs=P3^5;		//LCD数据命令选择端

sbit RST=P2^0;
sbit SCLK=P2^1;
sbit DATA=P2^2;

sbit key1=P1^2;
sbit key2=P1^3;
sbit key3=P1^4;
sbit key4=P1^5;

char year=10,mon=9,day=8,hou=14,min=33,sec=12;

uchar key1pushflag,key2pushflag,key3pushflag,key4pushflag;

uchar countkey1,runmode, num;
/*******************************************************************************************
函数名：delay_ms
调  用：delay_ms(x)
参  数：uint x
返回值：无
结  果：
注  释：延时程序
*******************************************************************************************/
void delay_ms(uint x)
	{
	  	uint i,j;
		for(i=x;i>0;i--)
			for(j=110;j>0;j--);
	}

void delaynus(unsigned char n)
{
 unsigned char i;
 for(i=0;i<n;i++)
      ;
}
/*******************************************************************************************
函数名：write_com
调  用：write_com(com)
参  数：uchar com
返回值：
结  果：
注  释：写命令函数，设置液晶光标显示/不显示、光标闪烁/不闪烁、需不需要移屏、在什么位置显示，
	   具体命令数值，参考数据手册
*******************************************************************************************/
void write_com (uchar com)
{
	lcdrs=0;		//选择写命令模式
	P0=com;			//把命令送到P0口，连接的哪个口，写哪个口
	delay_ms(5);		//看时序图，可以得出，写程序需要时间
	lcden=1;		//液晶屏使能端
	delay_ms(5);		
	lcden=0;
}

 /*******************************************************************************************
函数名：write_data
调  用：write_data(data)
参  数：uchar data
返回值：
结  果：
注  释：写数据函数，显示什么内容
*******************************************************************************************/
void write_data(uchar date)
{
	lcdrs=1;//选择写数据模式
	P0=date;//把命令送到P0口，连接的哪个口，写哪个口
	delay_ms(5);//看时序图，可以得出，写程序需要时间
	lcden=1;//液晶屏使能端
	delay_ms(5);		
	lcden=0;
}
 /*******************************************************************************************
函数名：显示时钟数据
调  用：write_time_to_lcd(add,dat)
参  数：uchar add,uchar dat
返回值：
结  果：
注  释：在那里显示，显示什么内容
*******************************************************************************************/
void write_time_to_lcd(uchar add,uchar dat)
{
	int i,j;
	i=dat/10+'0';
	j=dat%10+'0';
 	write_com(0x80+add);
//	write_data(init_display3[i]);
//	write_data(init_display3[j]);
	write_data(i);
	write_data(j);
	delay_ms(5);
}

/*******************************************************************************************
函数名：LCD初始化
调  用：Init_LCD()
参  数：
返回值：
结  果：
注  释：初始化LCD
*******************************************************************************************/
void Init_LCD()
{
	lcden=0;
	write_com(0x38);	//初始化设置（1）：显示模式设置。0011 1000，编码代表什么功能，查手册，想实现什么功能，写什么编码
	write_com(0x0c);	//初始化设置（2）：显示开关及光标设置。0000 1100，编码代表什么功能，查手册，想实现什么功能，写什么编码
	write_com(0x06);	//
	write_com(0x01);	//其他设置
	write_com(0x80+0x03); //初始化显示
	for(num=0;num<10;num++)
	{
		write_data(init_display1[num]);
		delay_ms(5);
	}
	write_com(0x80+0x42); //初始化显示
	for(num=0;num<8;num++)
	{
		write_data(init_display2[num]);
		delay_ms(5);
	}
}

/*******************************************************************************************
函数名：向DS1302 写一个字节的数据
调  用：Write1302(dat)
参  数：
返回值：
结  果：
注  释：参考DS1302写数据的时序图
*******************************************************************************************/
void Write1302(unsigned char dat)
{
  unsigned char i; 
  SCLK=0;            //拉低SCLK，为脉冲上升沿写入数据做好准备
 delaynus(2);       //稍微等待，使硬件做好准备
  for(i=0;i<8;i++)      //连续写8个二进制位数据
    {
	    DATA=dat&0x01;    //取出dat的第0位数据写入1302
		delaynus(2);       //稍微等待，使硬件做好准备
		SCLK=1;           //上升沿写入数据
		delaynus(2);      //稍微等待，使硬件做好准备
		SCLK=0;           //重新拉低SCLK，形成脉冲
		dat>>=1;          //将dat的各数据位右移1位，准备写入下一个数据位
	  }
	
 }
/*******************************************************************************************
函数名：向特定寄存器写一个字节的数据
调  用：WriteSet1302( Cmd, dat)
参  数：
返回值：
结  果：
注  释：向秒钟等寄存器写数据，具体地址参考DS1302时钟寄存器说明
*******************************************************************************************/
void WriteSet1302(unsigned char Cmd,unsigned char dat)
 {
   	RST=0;           //禁止数据传递
   	SCLK=0;          //确保写数居前SCLK被拉低
	RST=1;           //启动数据传输
	delaynus(2);     //稍微等待，使硬件做好准备
	Write1302(Cmd);  //写入命令字
	Write1302(dat);  //写数据
	SCLK=1;          //将时钟电平置于已知状态
	RST=0;           //禁止数据传递
 }
/*******************************************************************************************
函数名：读一个字节的数据
调  用：Read1302()
参  数：
返回值：dat
结  果：
注  释：向秒钟等寄存器读数据，具体地址参考DS1302时钟寄存器说明
*******************************************************************************************/
unsigned char Read1302(void)
 {
   unsigned char i,dat;
	delaynus(2);       //稍微等待，使硬件做好准备
	for(i=0;i<8;i++)   //连续读8个二进制位数据
	 {
	   dat>>=1;       //将dat的各数据位右移1位，因为先读出的是字节的最低位
		if(DATA==1)    //如果读出的数据是1
		 dat|=0x80;    //将1取出，写在dat的最高位
		 SCLK=1;       //将SCLK置于高电平，为下降沿读出
		 delaynus(2);  //稍微等待
		 SCLK=0;       //拉低SCLK，形成脉冲下降沿
		 delaynus(2);  //稍微等待
	  }	 
  return dat;        //将读出的数据返回
}  
/*******************************************************************************************
函数名：读特定寄存器一个字节的数据
调  用：ReadSet1302( Cmd)
参  数：
返回值：dat
结  果：
注  释：向秒钟等寄存器读数据，具体地址参考DS1302时钟寄存器说明
*******************************************************************************************/
unsigned char  ReadSet1302(unsigned char Cmd)
 {
  	unsigned char dat;
  	RST=0;                 //拉低RST
  	SCLK=0;                //确保写数居前SCLK被拉低
  	RST=1;                 //启动数据传输
  	Write1302(Cmd);       //写入命令字
  	dat=Read1302();       //读出数据
  	SCLK=1;              //将时钟电平置于已知状态
  	RST=0;               //禁止数据传递
  	return dat;          //将读出的数据返回
}
/*******************************************************************************************
函数名：DS1302初始化
调  用：Init_DS1302()
参  数：
返回值：dat
结  果：
注  释：掉电以后继续走
*******************************************************************************************/
/*void Init_DS1302(void)
{	
   	unsigned char flag;  
    flag= ReadSet1302(0x81);
	if(flag&0x80)	
	{      //判断时钟芯片是否关闭
  		WriteSet1302(0x8E,0x00);                 //根据写状态寄存器命令字，写入不保护指令 
  		WriteSet1302(0x80,((55/10)<<4|(55%10)));   //根据写秒寄存器命令字，写入秒的初始值
		WriteSet1302(0x82,((59/10)<<4|(59%10)));   //根据写分寄存器命令字，写入分的初始值
		WriteSet1302(0x84,((23/10)<<4|(23%10))); //根据写小时寄存器命令字，写入小时的初始值
		WriteSet1302(0x86,((18/10)<<4|(18%10))); //根据写日寄存器命令字，写入日的初始值
		WriteSet1302(0x88,((6/10)<<4|(6%10))); //根据写月寄存器命令字，写入月的初始值
		WriteSet1302(0x8c,((9/10)<<4|(9%10)));   //根据写小时寄存器命令字，写入小时的初始值
		WriteSet1302(0x90,0xa5); 
		WriteSet1302(0x8E,0x80);
  	}
}*/  

void Init_DS1302(void)
{
 	RST=0;
	SCLK=0;
	WriteSet1302(0x8e,0x00);
	WriteSet1302(0x80,0x00);
}  
 /*******************************************************************************************
函数名：BCD转10进制
调  用：BCD_Decimal( bcd);
参  数：
返回值：Decimal
结  果：
注  释：
*******************************************************************************************/
uchar BCD_Decimal(uchar bcd)
{
 	uchar Decimal;
	Decimal=(bcd/16)*10+(bcd%16);
	return(Decimal);
}

/*******************************************************************************************
函数名：键盘扫描
调  用：keyscan()
参  数：
返回值：dat
结  果：
注  释：是否按下功能键，按下哪个功能键
*******************************************************************************************/
void keyscan()
{
	
 	if(key1==0)
	{
	 	delay_ms(5);	 		//软件去抖
		if(key1==0)
		{
			while(!key1); 		//等待按键抬起
			runmode=0;			//功能键被按下，进入万年历参数设置
			key1pushflag=1;		// 功能键被按下标志位
			countkey1++;		//功能键被按下的次数
			if(countkey1>6)
				countkey1=1;	
		}
	}
	if(key1pushflag==1)
	{
		if(key2==0)
	 	{	delay_ms(10);	 		//软件去抖
			if(key2==0)
			{
				while(!key2); 		//等待按键抬起
				key2pushflag=1;	
			}
		}
		if(key3==0)
	 	{	delay_ms(10);	 		//软件去抖
			if(key3==0)
			{
				while(!key3); 		//等待按键抬起
				key3pushflag=1;	
			}
		}
		if(key4==0)
	 	{	delay_ms(10);	 		//软件去抖
			if(key4==0)
			{
				while(!key4); 		//等待按键抬起
				key1pushflag=0;	
				key4pushflag=1;	
			}
		}
   	}
}
/*******************************************************************************************
函数名：按键功能处理程序
调  用：Init_DS1302()
参  数：
返回值：dat
结  果：
注  释：掉电以后继续走
*******************************************************************************************/
void keyhandle()
{
	uchar temp;
	switch(countkey1)
	{
	 	case 1:						//功能键按下1次，处理年的数据
			write_com(0x80+0x05);	//光标置于年的位置
			write_com(0x0f);		//光标闪烁
			if(key2pushflag==1)
			{
				year++;
				if(year>99)
				{
					year=0;	
				}
				key2pushflag=0;		//清楚标志，防止按1次该键，数值一直增加
				write_time_to_lcd(0x05,year);
				write_com(0x80+0x05);	
			}
			else if(key3pushflag==1)
			{
				year--;
				if(year<0)
				{
					year=99;	
				}
				key3pushflag=0;		//清楚标志，防止按1次该键，数值一直增加
				write_time_to_lcd(0x05,year);
				write_com(0x80+0x05);
			}
			break;
		case 2:						//功能键按下1次，处理年的数据
			write_com(0x80+0x08);	//光标置于年的位置
			write_com(0x0f);		//光标闪烁
			if(key2pushflag==1)
			{
				mon++;
				if(mon>12)
				{
					mon=1;	
				}
				key2pushflag=0;		//清楚标志，防止按1次该键，数值一直增加
				write_time_to_lcd(0x08,mon);
				write_com(0x80+0x08);	
			}
			else if(key3pushflag==1)
			{
				mon--;
				if(mon<0)
				{
					mon=12;	
				}
				key3pushflag=0;		//清楚标志，防止按1次该键，数值一直增加
				write_time_to_lcd(0x08,mon);
				write_com(0x80+0x08);
			}
			break;
		case 3:						//功能键按下1次，处理年的数据
			write_com(0x80+0x0b);	//光标置于年的位置
			write_com(0x0f);		//光标闪烁
			if(key2pushflag==1)
			{
				day++;
				if(day>30)
				{
					day=1;	
				}
				key2pushflag=0;		//清楚标志，防止按1次该键，数值一直增加
				write_time_to_lcd(0x0b,day);
				write_com(0x80+0x0b);	
			}
			else if(key3pushflag==1)
			{
				day--;
				if(day<0)
				{
					day=30;	
				}
				key3pushflag=0;		//清楚标志，防止按1次该键，数值一直增加
				write_time_to_lcd(0xb,day);
				write_com(0x80+0x0b);
			}
			break;
		case 4:						//功能键按下1次，处理年的数据
			write_com(0x80+0x42);	//光标置于年的位置
			write_com(0x0f);		//光标闪烁
			if(key2pushflag==1)
			{
				hou++;
				if(hou>23)
				{
					hou=0;	
				}
				key2pushflag=0;		//清楚标志，防止按1次该键，数值一直增加
				write_time_to_lcd(0x42,hou);
				write_com(0x80+0x42);	
			}
			else if(key3pushflag==1)
			{
				hou--;
				if(hou<0)
				{
					hou=23;	
				}
				key3pushflag=0;		//清楚标志，防止按1次该键，数值一直增加
				write_time_to_lcd(0x42,hou);
				write_com(0x80+0x42);
			}
			break;
		case 5:						//功能键按下1次，处理年的数据
			write_com(0x80+0x45);	//光标置于年的位置
			write_com(0x0f);		//光标闪烁
			if(key2pushflag==1)
			{
				min++;
				if(min>59)
				{
					min=0;	
				}
				key2pushflag=0;		//清楚标志，防止按1次该键，数值一直增加
				write_time_to_lcd(0x45,min);
				write_com(0x80+0x45);	
			}
			else if(key3pushflag==1)
			{
				min--;
				if(min<0)
				{
					min=59;	
				}
				key3pushflag=0;		//清楚标志，防止按1次该键，数值一直增加
				write_time_to_lcd(0x45,min);
				write_com(0x80+0x45);
			}
			break;
		case 6:						//功能键按下1次，处理年的数据
			write_com(0x80+0x48);	//光标置于年的位置
			write_com(0x0f);		//光标闪烁
			if(key2pushflag==1)
			{
				sec++;
				if(sec>59)
				{
					sec=0;	
				}
				key2pushflag=0;		//清楚标志，防止按1次该键，数值一直增加
				write_time_to_lcd(0x48,sec);
				write_com(0x80+0x48);	
			}
			else if(key3pushflag==1)
			{
				sec--;
				if(sec<0)
				{
					sec=59;	
				}
				key3pushflag=0;		//清楚标志，防止按1次该键，数值一直增加
				write_time_to_lcd(0x48,sec);
				write_com(0x80+0x48);
			}
			break;
	}
	if(key4pushflag==1)	 //转换成BCD码写入1302
	{
		temp=(sec)/10*16+sec%10;
		WriteSet1302(0x8e,0x00);
		WriteSet1302(0x80,temp);

		temp=(min)/10*16+min%10;
		WriteSet1302(0x8e,0x00);
		WriteSet1302(0x82,temp);

		temp=(hou)/10*16+hou%10;
		WriteSet1302(0x8e,0x00);
		WriteSet1302(0x84,temp);

		temp=(day)/10*16+day%10;
		WriteSet1302(0x8e,0x00);
		WriteSet1302(0x86,temp);

		temp=(mon)/10*16+mon%10;
		WriteSet1302(0x8e,0x00);
		WriteSet1302(0x88,temp);
	
		temp=(year)/10*16+year%10;
		WriteSet1302(0x8e,0x00);
		WriteSet1302(0x8c,temp);
		write_com(0x0c);
		key4pushflag=0;
		runmode=1;
		delay_ms(10);	
	}
}
/*******************************************************************************************
函数名：主程序
调  用：
参  数：
返回值：
结  果：
注  释：
*******************************************************************************************/
void main()
{	char sec_bcd;
	runmode=1;
 	Init_LCD();
	Init_DS1302();
	while(1)
	{	
		keyscan();
		if(runmode==1)
		{	
/*		 	sec=BCD_Decimal(ReadSet1302(0x81));
			min=BCD_Decimal(ReadSet1302(0x83));
			hou=BCD_Decimal(ReadSet1302(0x85));
			day=BCD_Decimal(ReadSet1302(0x87));
			mon=BCD_Decimal(ReadSet1302(0x89));
			year=BCD_Decimal(ReadSet1302(0x8d));
*/
			sec_bcd=ReadSet1302(0x81);
			sec=BCD_Decimal(sec_bcd);
			min=BCD_Decimal(ReadSet1302(0x83));
			hou=BCD_Decimal(ReadSet1302(0x85));
			day=BCD_Decimal(ReadSet1302(0x87));
			mon=BCD_Decimal(ReadSet1302(0x89));
			year=BCD_Decimal(ReadSet1302(0x8d));


			write_com(0x0c);
			write_time_to_lcd(0x48,sec);
			write_time_to_lcd(0x45,min);
			write_time_to_lcd(0x42,hou);
			write_time_to_lcd(0x0b,day);
			write_time_to_lcd(0x08,mon);
			write_time_to_lcd(0x05,year);
		}
		else
		{
			keyhandle();
		}
	}
}
