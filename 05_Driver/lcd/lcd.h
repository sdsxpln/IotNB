//=====================================================================
//文件名称：slcd.h
//功能概要：GPIO底层驱动构件头文件
//制作单位：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版    本：2017-04-19  V1.0;
//适用芯片：KL36
//=====================================================================

#ifndef _SLCD_H_
#define _SLCD_H_

#include "common.h"
#include "stdlib.h"
#include "string.h"

typedef union
{
	uint_64 Byte;
	struct
	{
		uint_8 SEG0 : 1;
		uint_8 SEG1 : 1;
		uint_8 SEG2 : 1;
		uint_8 SEG3 : 1;
		uint_8 SEG4 : 1;
		uint_8 SEG5 : 1;
		uint_8 SEG6 : 1;
		uint_8 SEG7 : 1;
		uint_8 SEG8 : 1;
		uint_8 SEG9 : 1;
		uint_8 SEG10 : 1;
		uint_8 SEG11 : 1;
		uint_8 SEG12 : 1;
		uint_8 SEG13 : 1;
		uint_8 SEG14 : 1;
		uint_8 SEG15 : 1;
		uint_8 SEG16 : 1;
		uint_8 SEG17 : 1;
		uint_8 SEG18 : 1;
		uint_8 SEG19 : 1;
		uint_8 SEG20 : 1;
		uint_8 SEG21 : 1;
		uint_8 SEG22 : 1;
		uint_8 SEG23 : 1;
		uint_8 SEG24 : 1;
		uint_8 SEG25 : 1;
		uint_8 SEG26 : 1;
		uint_8 SEG27 : 1;
		uint_8 SEG28 : 1;
		uint_8 SEG29 : 1;
		uint_8 SEG30 : 1;
		uint_8 SEG31 : 1;
		uint_8 SEG32 : 1;
		uint_8 SEG33 : 1;
		uint_8 SEG34 : 1;
		uint_8 SEG35 : 1;
		uint_8 SEG36 : 1;
		uint_8 SEG37 : 1;
		uint_8 SEG38 : 1;
		uint_8 SEG39 : 1;
		uint_8 SEG40 : 1;
		uint_8 SEG41 : 1;
		uint_8 SEG42 : 1;
		uint_8 SEG43 : 1;
		uint_8 SEG44 : 1;
		uint_8 SEG45 : 1;
		uint_8 SEG46 : 1;
		uint_8 SEG47 : 1;
		uint_8 SEG48 : 1;
		uint_8 SEG49 : 1;
		uint_8 SEG50 : 1;
		uint_8 SEG51 : 1;
		uint_8 SEG52 : 1;
		uint_8 SEG53 : 1;
		uint_8 SEG54 : 1;
		uint_8 SEG55 : 1;
		uint_8 SEG56 : 1;
		uint_8 SEG57 : 1;
		uint_8 SEG58 : 1;
		uint_8 SEG59 : 1;
		uint_8 SEG60 : 1;
		uint_8 SEG61 : 1;
		uint_8 SEG62 : 1;
		uint_8 SEG63 : 1;
	}SEGs;
}SLCD_COM;

struct _LCD
{
	SLCD_COM COM[8];
};

struct _LCD lcd;

#define D1_A lcd.COM[3].SEGs.SEG13
#define D1_B lcd.COM[2].SEGs.SEG13
#define D1_C lcd.COM[1].SEGs.SEG13
#define D1_D lcd.COM[0].SEGs.SEG12
#define D1_E lcd.COM[1].SEGs.SEG12
#define D1_F lcd.COM[3].SEGs.SEG12
#define D1_G lcd.COM[2].SEGs.SEG12
#define DP1 lcd.COM[0].SEGs.SEG13

#define D2_A lcd.COM[3].SEGs.SEG15
#define D2_B lcd.COM[2].SEGs.SEG15
#define D2_C lcd.COM[1].SEGs.SEG15
#define D2_D lcd.COM[0].SEGs.SEG14
#define D2_E lcd.COM[1].SEGs.SEG14
#define D2_F lcd.COM[3].SEGs.SEG14
#define D2_G lcd.COM[2].SEGs.SEG14
#define DP2 lcd.COM[0].SEGs.SEG15

#define D3_A lcd.COM[3].SEGs.SEG21
#define D3_B lcd.COM[2].SEGs.SEG21
#define D3_C lcd.COM[1].SEGs.SEG21
#define D3_D lcd.COM[0].SEGs.SEG20
#define D3_E lcd.COM[1].SEGs.SEG20
#define D3_F lcd.COM[3].SEGs.SEG20
#define D3_G lcd.COM[2].SEGs.SEG20
#define DP3 lcd.COM[0].SEGs.SEG21

#define D4_A lcd.COM[3].SEGs.SEG23
#define D4_B lcd.COM[2].SEGs.SEG23
#define D4_C lcd.COM[1].SEGs.SEG23
#define D4_D lcd.COM[0].SEGs.SEG22
#define D4_E lcd.COM[1].SEGs.SEG22
#define D4_F lcd.COM[3].SEGs.SEG22
#define D4_G lcd.COM[2].SEGs.SEG22
#define DP4 lcd.COM[0].SEGs.SEG23

#define D5_A lcd.COM[3].SEGs.SEG25
#define D5_B lcd.COM[2].SEGs.SEG25
#define D5_C lcd.COM[1].SEGs.SEG25
#define D5_D lcd.COM[0].SEGs.SEG24
#define D5_E lcd.COM[1].SEGs.SEG24
#define D5_F lcd.COM[3].SEGs.SEG24
#define D5_G lcd.COM[2].SEGs.SEG24
#define DP5 lcd.COM[0].SEGs.SEG25

#define D6_A lcd.COM[3].SEGs.SEG27
#define D6_B lcd.COM[2].SEGs.SEG27
#define D6_C lcd.COM[1].SEGs.SEG27
#define D6_D lcd.COM[0].SEGs.SEG26
#define D6_E lcd.COM[1].SEGs.SEG26
#define D6_F lcd.COM[3].SEGs.SEG26
#define D6_G lcd.COM[2].SEGs.SEG26
#define DP6 lcd.COM[0].SEGs.SEG27

#define D7_A lcd.COM[3].SEGs.SEG41
#define D7_B lcd.COM[2].SEGs.SEG41
#define D7_C lcd.COM[1].SEGs.SEG41
#define D7_D lcd.COM[0].SEGs.SEG40
#define D7_E lcd.COM[1].SEGs.SEG40
#define D7_F lcd.COM[3].SEGs.SEG40
#define D7_G lcd.COM[2].SEGs.SEG40
#define DP7 lcd.COM[0].SEGs.SEG41

#define D8_A lcd.COM[3].SEGs.SEG43
#define D8_B lcd.COM[2].SEGs.SEG43
#define D8_C lcd.COM[1].SEGs.SEG43
#define D8_D lcd.COM[0].SEGs.SEG42
#define D8_E lcd.COM[1].SEGs.SEG42
#define D8_F lcd.COM[3].SEGs.SEG42
#define D8_G lcd.COM[2].SEGs.SEG42

#define FP_NUM 16
#define BP_NUM 4


extern uint_32* LCD_PORT[61];
extern uint_8 FP_Table[FP_NUM];
extern uint_8 BP_Table[BP_NUM];



//=====================================================================
//函数名称：LCDInit
//函数返回：无
//参数说明：无
//功能概要：LCD初始化
//=====================================================================
void LCDInit();

//=====================================================================
//函数名称：LEDshow
//函数返回：无
//参数说明：data[8]：显示的内容。可显示的数字0~9,0.~9.,E,F,全亮，全暗（见显示码表）
//功能概要：将数组data对应的码表值内容显示在LED上
//=====================================================================
void LCDShow(uint_8 data[8]);

//=====================================================================
//函数名称：LCDShowTime
//函数返回：
//参数说明：time:传入的时间数组，如123456
//功能概要：液晶根据time数组显示时间，例123456显示为12.34.56
//=====================================================================
void LCDShowTime(uint_8 time[6]);

//=====================================================================
//函数名称：LCDShowDate
//函数返回：
//参数说明：date:传入的日期数组，如19491001
//功能概要：液晶根据time数组显示时间，例19491001显示为19491001
//=====================================================================
void LCDShowDate(uint_8 date[8]);

//=====================================================================
//函数名称：LCDShowNum
//函数返回：
//参数说明：num:传入的需要显示的数字
//功能概要：液晶根据num数组显示相应的数字
//=====================================================================
void LCDShowNum(uint_32 num);

void LCDShowDouble(double value);
//=====================================================================
//函数名称：LCDShowProMsg
//函数返回：
//参数说明：
//功能概要：
//=====================================================================
void LCDShowRunMsg(uint_8 type,uint_8 num,int_16 data);


//=====================================================================
//函数名称：LCDClear
//函数返回：无
//参数说明：无
//功能概要：清除SLCD所显示的所有内容
//=====================================================================
void LCDClear();

//=====================================================================
//函数名称：LCDRefresh
//函数返回：无
//参数说明：无
//功能概要：刷新SLCD的显示
//=====================================================================
void LCDRefresh();

#endif
