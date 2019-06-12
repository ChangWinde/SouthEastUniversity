/*
 * LCD_128.h
 *
 *  Created on: 2013-3-30
 *      Author: Administrator
 */

#ifndef LCD_128_H_
#define LCD_128_H_

extern void LCD_Clear();
extern void LCD_DisplaySeg(unsigned char SegNum);
extern void LCD_ClearSeg(unsigned char SegNum);
extern void LCD_DisplayDigit(unsigned char Digit,unsigned char Position);
extern void LCD_DisplayNum(  long int Digit);
extern void Calculate_NumBuff(unsigned char Position,unsigned char *Num_Buffer,unsigned char *Addr);

#define LCD_DIGIT_CLEAR 		11
//段码序号宏定义

#define			_LCD_MIN	 			0
#define			_LCD_NEG				1
#define			_LCD_POS				2
#define			_LCD_MAX				3
#define			_LCD_DOT0				8
#define			_LCD_DOT1				16
#define			_LCD_DOT2				24
#define			_LCD_DOT3				52
#define			_LCD_DOT4				60
#define			_LCD_DOT5				107
#define			_LCD_DOT6				123
//冒号
#define			_LCD_COLON0		99
#define			_LCD_COLON1		83
#define			_LCD_COLON2		115


#define			_LCD_BUSY				28
#define			_LCD_ERROR			29
#define			_LCD_AUTO				30
#define			_LCD_RUN				31

#define			_LCD_PAUSE			32
#define			_LCD_STOP				33
#define			_LCD_kPA				34
#define			_LCD_k_g					35

#define			_LCD_pre_s				36
#define			_LCD_m_pre_s			37
#define			_LCD_c_m_pre_s		38
#define			_LCD_g					39

#define			_LCD_TI_logo			40
#define			_LCD_AC					41
#define			_LCD_DC					42
#define			_LCD_Hz					43

#define			_LCD_L						44
#define			_LCD_M3					45
#define			_LCD_M_Hz				46
#define			_LCD_k_Hz				47

#define			_LCD_PRECENT		68
#define			_LCD_k_W				72
#define			_LCD_m_V				73
#define			_LCD_m_A				74
#define			_LCD_k_OHOM		75

#define			_LCD_W					76
#define			_LCD_V					77
#define			_LCD_A					78
#define			_LCD_OHOM			79

#define			_LCD_DEGREE         80
#define			_LCD_dB					81
#define			_LCD_QDU_logo      82

#define			_LCD_3_AT				84
#define			_LCD_2_AT				85
#define			_LCD_AT					86
#define			_LCD_BELL				87

#define			_LCD_AM					88
#define			_LCD_FM					89
#define			_LCD_RX					90
#define			_LCD_LOCK				91

#define			_LCD_PM					92
#define			_LCD_CH					93
#define			_LCD_TX					94
#define			_LCD_LOWBAT		95

//1号位置的七段码对应值
#define			_LCD_1A					11
#define			_LCD_1B					10
#define			_LCD_1C					9
#define			_LCD_1D					4
#define			_LCD_1E					5
#define			_LCD_1F					7
#define			_LCD_1G					6
//2号位置的七段码对应值
#define			_LCD_2A					19
#define			_LCD_2B					18
#define			_LCD_2C					17
#define			_LCD_2D					12
#define			_LCD_2E					13
#define			_LCD_2F					15
#define			_LCD_2G					14
//3号位置的七段码对应值
#define			_LCD_3A					27
#define			_LCD_3B					26
#define			_LCD_3C					25
#define			_LCD_3D					20
#define			_LCD_3E					21
#define			_LCD_3F					23
#define			_LCD_3G					22
//4号位置的七段码对应值
#define			_LCD_4A					55
#define			_LCD_4B					54
#define			_LCD_4C					53
#define			_LCD_4D					48
#define			_LCD_4E					49
#define			_LCD_4F					51
#define			_LCD_4G					50
//5号位置的七段码对应值
#define			_LCD_5A					63
#define			_LCD_5B					62
#define			_LCD_5C					61
#define			_LCD_5D					56
#define			_LCD_5E					57
#define			_LCD_5F					59
#define			_LCD_5G					58
//6号位置的七段码对应值
#define			_LCD_6A					71
#define			_LCD_6B					70
#define			_LCD_6C					69
#define			_LCD_6D					64
#define			_LCD_6E					65
#define			_LCD_6F					67
#define			_LCD_6G					66
//7号位置的七段码对应值
#define			_LCD_7A					96
#define			_LCD_7B					97
#define			_LCD_7C					98
#define			_LCD_7D					103
#define			_LCD_7E					102
#define			_LCD_7F					100
#define			_LCD_7G					101
//8号位置的七段码对应值
#define			_LCD_8A					104
#define			_LCD_8B					105
#define			_LCD_8C					106
#define			_LCD_8D					111
#define			_LCD_8E					110
#define			_LCD_8F					108
#define			_LCD_8G					109
//9号位置的七段码对应值
#define			_LCD_9A					112
#define			_LCD_9B					113
#define			_LCD_9C					114
#define			_LCD_9D					119
#define			_LCD_9E					118
#define			_LCD_9F					116
#define			_LCD_9G					117
//10号位置的七段码对应值
#define			_LCD_10A				120
#define			_LCD_10B				121
#define			_LCD_10C				122
#define			_LCD_10D				127
#define			_LCD_10E					126
#define			_LCD_10F					124
#define			_LCD_10G				125


#endif /* LCD_H_ */
