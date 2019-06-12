/*
 * LCD_128.c
 *
 *  Created on: 2013-3-30
 *      Author: Administrator
 */
#include "msp430g2553.h"
#include "LCD_128.h"


unsigned int LCD_Buffer[8]={0,0,0,0,0,0,0,0};	//全局变量，显存


/***********************************************************************
 * 名称：LCD_Clear( )
 * 功能：清屏
  ********************************************************************** */
void LCD_Clear()
{
	unsigned char i=0;
  for ( i=0;i<=7;i++) LCD_Buffer[i]=0;
 }
/***********************************************************************
 * 名称：LCD_DisplaySeg()
 * 功能：显示一段段码
 * 传入参数：SegNum:0~127段号码
 * 举例：LCD_DisplaySeg(_LCD_TI_logo)，显示TI logo
  ********************************************************************** */
void LCD_DisplaySeg(unsigned char SegNum)
{
	LCD_Buffer[SegNum/16]  |= 1<<(SegNum%16);
 }
/***********************************************************************
 * 名称：LCD_ClearSeg()
 * 功能：清除一段段码
 * 传入参数：SegNum:0~127段号码
 * 举例：LCD_ClearSeg(_LCD_TI_logo)，显示TI logo
  ********************************************************************** */
void LCD_ClearSeg(unsigned char SegNum)
{
	LCD_Buffer[SegNum/16]  &= ~(1<<(SegNum%16));
 }
/***********************************************************************
 *名称：LCD_DisplayDigit(unsigned char Digit,unsigned char Position )
 * 功能：让128段式液晶的特定“8字”段显示0~9
 * 说明：大数码“8字”编号左至右为0~5，小数码“8字”编号右至左为6~9；
 *传入参数：Digit：想显示的数 0~9，传入其他数字则为消隐
 *					Position：显示的数位（第几个“8字”）
 *举例：LCD_DisplayDigit(9,1 )，第1个“8字”段显示9
 * 			   LCD_DisplayDigit(LCD_DIGIT_CLEAR,1)，第1个 “8字”段消隐
 ********************************************************************** */
void LCD_DisplayDigit(unsigned char Digit,unsigned char Position )
{
  switch (Position)
  {
  case 1:
      LCD_Buffer[0] &= 0xf10f;     		//先完全清除第0个“8字”段
      switch (Digit)
    {
    case 0:LCD_Buffer[0] |= 0x0eb0;      break;
    case 1:LCD_Buffer[0] |= 0x0600;      break;
    case 2:LCD_Buffer[0] |= 0x0c70;      break;
    case 3:LCD_Buffer[0] |= 0x0e50;      break;
    case 4:LCD_Buffer[0] |= 0x06c0;      break;
    case 5:LCD_Buffer[0] |= 0x0ad0;      break;
    case 6:LCD_Buffer[0] |= 0x0af0;      break;
    case 7:LCD_Buffer[0] |= 0x0e00;      break;
    case 8:LCD_Buffer[0] |= 0x0ef0;      break;
    case 9:LCD_Buffer[0] |= 0x0ed0;     break;
    default: 	break;				//这种情况说明是"消隐"该“8字”段
    }
      break;
  case 2:
	 LCD_Buffer[0] &= 0x0fff;			//第1个”8“字涉及2个缓存位置
	 LCD_Buffer[1] &= 0xfff1;			//不管三七二十一，先清除第1个”8“字
	 switch (Digit)
	{
	case 0:
	  LCD_Buffer[0] |= 0xB000;	  LCD_Buffer[1] |= 0x000E;	  break;
	case 1:
	  LCD_Buffer[1] |= 0x0006;	  break;
	case 2:
	  LCD_Buffer[0] |= 0x7000;	  LCD_Buffer[1] |= 0x000C;	  break;
	case 3:
	  LCD_Buffer[0] |= 0x5000;	  LCD_Buffer[1] |= 0x000E;	  break;
	case 4:
	   LCD_Buffer[0] |= 0xC000;  LCD_Buffer[1] |= 0x0006;	  break;
	case 5:
	  LCD_Buffer[0] |= 0xD000;	  LCD_Buffer[1] |= 0x000A;	  break;
	case 6:
	  LCD_Buffer[0] |= 0xF000;	  LCD_Buffer[1] |= 0x000A;	  break;
	case 7:
	  LCD_Buffer[0] |= 0x0000;	  LCD_Buffer[1] |= 0x000E;	  break;
	case 8:
	  LCD_Buffer[0] |= 0xF000;	  LCD_Buffer[1] |= 0x000E;	  break;
	case 9:
	  LCD_Buffer[0] |= 0xD000;	  LCD_Buffer[1] |= 0x000E;	  break;
	default: break;			//这种情况说明是"消隐"该“8字”段
	}
     break;
  case 3:
	LCD_Buffer[1] &= 0xF10F;		//先完全清除第2个“8字”段
	switch (Digit)
	{
	case 0:LCD_Buffer[1] |= 0x0EB0;  break;
	case 1:LCD_Buffer[1] |= 0x0600;  break;
	case 2:LCD_Buffer[1] |= 0x0C70;  break;
	case 3:LCD_Buffer[1] |= 0x0E50;  break;
	case 4:LCD_Buffer[1] |= 0x06C0;  break;
	case 5:LCD_Buffer[1] |= 0x0AD0;  break;
	case 6:LCD_Buffer[1] |= 0x0AF0;  break;
	case 7:LCD_Buffer[1] |= 0x0E00;  break;
	case 8:LCD_Buffer[1] |= 0x0EF0;  break;
	case 9:LCD_Buffer[1] |= 0x0ED0;  break;
	default: 		break;
	}
    break;
  case 4:
	LCD_Buffer[3] &= 0xFF10;		//先完全清除第3个“8字”段
	switch (Digit)
	{
	case 0:LCD_Buffer[3] |= 0x00EB;  break;
	case 1:LCD_Buffer[3] |= 0x0060;  break;
	case 2:LCD_Buffer[3] |= 0x00C7;  break;
	case 3:LCD_Buffer[3] |= 0x00E5;  break;
	case 4:LCD_Buffer[3] |= 0x006C;  break;
	case 5:LCD_Buffer[3] |= 0x00AD;  break;
	case 6:LCD_Buffer[3] |= 0x00AF;  break;
	case 7:LCD_Buffer[3] |= 0x00E0;  break;
	case 8:LCD_Buffer[3] |= 0x00EF;  break;
	case 9:LCD_Buffer[3] |= 0x00ED;  break;
	default: break;
	}
    break;

  case 5:
	LCD_Buffer[3] &= 0x10FF;
	switch (Digit)
	{
	case 0:LCD_Buffer[3] |= 0xEB00;  break;
	case 1:LCD_Buffer[3] |= 0x6000;  break;
	case 2:LCD_Buffer[3] |= 0xC700;  break;
	case 3:LCD_Buffer[3] |= 0xE500;  break;
	case 4:LCD_Buffer[3] |= 0x6C00;  break;
	case 5:LCD_Buffer[3] |= 0xAD00;  break;
	case 6:LCD_Buffer[3] |= 0xAF00;  break;
	case 7:LCD_Buffer[3] |= 0xE000;  break;
	case 8:LCD_Buffer[3] |= 0xEF00;  break;
	case 9:LCD_Buffer[3] |= 0xED00;  break;
	default: break;
	}
    break;

  case 6:
	LCD_Buffer[4] &= 0xFF10;
	switch (Digit)
	{
	case 0:LCD_Buffer[4] |= 0x00EB;	  break;
	case 1:LCD_Buffer[4] |= 0x0060;	  break;
	case 2:LCD_Buffer[4] |= 0x00C7;	  break;
	case 3:LCD_Buffer[4] |= 0x00E5;	  break;
	case 4:LCD_Buffer[4] |= 0x006C;	  break;
	case 5:LCD_Buffer[4] |= 0x00AD;	  break;
	case 6:LCD_Buffer[4] |= 0x00AF;	  break;
	case 7:LCD_Buffer[4] |= 0x00E0;	  break;
	case 8:LCD_Buffer[4] |= 0x00EF;	  break;
	case 9:LCD_Buffer[4] |= 0x00ED;	  break;
	default: break;
	}
    break;

  case 7:
	LCD_Buffer[6] &= 0xFF08;
	switch (Digit)
	{
	case 0:LCD_Buffer[6] |= 0x00D7;	  break;
	case 1:LCD_Buffer[6] |= 0x0006;	  break;
	case 2:LCD_Buffer[6] |= 0x00E3;	  break;
	case 3:LCD_Buffer[6] |= 0x00A7;	  break;
	case 4:LCD_Buffer[6] |= 0x0036;	  break;
	case 5:LCD_Buffer[6] |= 0x00B5;	  break;
	case 6:LCD_Buffer[6] |= 0x00F5;	  break;
	case 7:LCD_Buffer[6] |= 0x0007;	  break;
	case 8:LCD_Buffer[6] |= 0x00F7;	  break;
	case 9:LCD_Buffer[6] |= 0x00B7;	  break;
	default: break;
	}
    break;

  case 8:
	LCD_Buffer[6] &= 0x08ff;
	switch (Digit)
	{
	case 0:LCD_Buffer[6] |= 0xD700;	  break;
	case 1:LCD_Buffer[6] |= 0x0600;	  break;
	case 2:LCD_Buffer[6] |= 0xE300;	  break;
	case 3:LCD_Buffer[6] |= 0xA700;	  break;
	case 4:LCD_Buffer[6] |= 0x3600;	  break;
	case 5:LCD_Buffer[6] |= 0xB500;	  break;
	case 6:LCD_Buffer[6] |= 0xF500;	  	 break;
	case 7:LCD_Buffer[6] |= 0x0700;	  break;
	case 8:LCD_Buffer[6] |= 0xF700;	  	  break;
	case 9:LCD_Buffer[6] |= 0xB700;	  break;
	default: break;
	}
    break;

  case 9:
	LCD_Buffer[7] &= 0xFF08;
	switch (Digit)
	{
	case 0:LCD_Buffer[7] |= 0x00D7;	  break;
	case 1:LCD_Buffer[7] |= 0x0006;	  break;
	case 2:LCD_Buffer[7] |= 0x00E3;	  break;
	case 3:LCD_Buffer[7] |= 0x00A7;	  break;
	case 4:LCD_Buffer[7] |= 0x0036;	  break;
	case 5:LCD_Buffer[7] |= 0x00B5;	  break;
	case 6:LCD_Buffer[7] |= 0x00F5;	  break;
	case 7:LCD_Buffer[7] |= 0x0007;	  break;
	case 8:LCD_Buffer[7] |= 0x00F7;	  break;
	case 9:LCD_Buffer[7] |= 0x00B7;	  break;
	default: break;
	}
    break;

  case 10:
	LCD_Buffer[7] &= 0x08ff;
	switch (Digit)
	{
	case 0:LCD_Buffer[7] |= 0xD700;	  break;
	case 1:LCD_Buffer[7] |= 0x0600;	  break;
	case 2:LCD_Buffer[7] |= 0xE300;	  break;
	case 3:LCD_Buffer[7] |= 0xA700;	  break;
	case 4:LCD_Buffer[7] |= 0x3600;	  break;
	case 5:LCD_Buffer[7] |= 0xB500;	  break;
	case 6:LCD_Buffer[7] |= 0xF500;	  break;
	case 7:LCD_Buffer[7] |= 0x0700;	  break;
	case 8:LCD_Buffer[7] |= 0xF700;	  break;
	case 9:LCD_Buffer[7] |= 0xB700;	  break;
	default: break;
	}
    break;
	default:break;
  }
}
  /****************************************************************************
  * 名    称：LCD_DisplayNum()
  * 功    能：在LCD上连续显示一个整型数据。
  * 入口参数：Digit:显示数值  (-32768~32767)
  * 出口参数：无
  * 范    例: LCD_DisplayNum( 12345); 显示结果: 12345
              	  LCD_DisplayNum(-12345); 显示结果:-12345
  * 说    明: 该函数仅限在大屏幕的8字段上显示
  ****************************************************************************/
  void LCD_DisplayNum( long int Digit)
  {
	unsigned char i=0;
	unsigned   char DispBuff[6]={0};
    if(Digit<0)
	{
	Digit=-Digit;		//处理负数
	LCD_DisplaySeg( _LCD_NEG); 		//显示负号
	}
    else
    	LCD_ClearSeg(_LCD_NEG); 		//清除负号
    for(i=0;i<6;i++)                       		//拆分数字
     {
       DispBuff[i]=Digit%10;
       Digit/=10;
     }
    for(i=5;i>1;i--)                     			//消隐无效"0"
     {
       if (DispBuff[i]==0) DispBuff[i]=LCD_DIGIT_CLEAR;
       else break;
     }
    for(i=0;i<6;i++)
      {
    	LCD_DisplayDigit(DispBuff[i],6-i);
      }
  }
  /****************************************************************************
  * 名    称：Calculate_NumBuff()
  * 功    能：计算单个8字的显存和起始地址
  * 入口参数：Position：8字段的位置
  * 				 *Num_Buffer：需要更新的段码的指针
  * 				 *Addr：需要更新的段码的起始地址的指针
  * 出口参数：无
  * 范         例: 无
  * 说         明: 无
  ****************************************************************************/
  void Calculate_NumBuff(unsigned char Position,unsigned char *Num_Buffer,unsigned char *Addr)
   {
 	  switch(Position)
 	  {
 	  case 1: *Num_Buffer=(LCD_Buffer[0]&0x0ff0)>>4; 	*Addr=4/4;break;
 	  case 2: *Num_Buffer=((LCD_Buffer[0]&0xf000)>>12+((LCD_Buffer[1]&0x000f)<<4));	*Addr=12/4;break;
 	  case 3: *Num_Buffer=(LCD_Buffer[1]&0x0ff0)>>4; 	*Addr=20/4;break;
 	  case 4: *Num_Buffer=(LCD_Buffer[3]&0x00ff); 	*Addr=48/4;break;
 	  case 5: *Num_Buffer=(LCD_Buffer[3]&0xff00)>>8;	 *Addr=56/4;break;
 	  case 6: *Num_Buffer=(LCD_Buffer[4]&0x00ff); 	*Addr=64/4;break;
 	  case 7: *Num_Buffer=(LCD_Buffer[6]&0x00ff);	*Addr=96/4;break;
 	  case 8: *Num_Buffer=(LCD_Buffer[6]&0xff00)>>8;	*Addr=104/4;break;
 	  case 9: *Num_Buffer=(LCD_Buffer[7]&0x00ff);	*Addr=112/4;break;
 	  case 10: *Num_Buffer=(LCD_Buffer[7]&0xff00)>>8; 	*Addr=120/4;break;
 	  default: break;
 	  }
   }
