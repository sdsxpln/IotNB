#include "lcd.h"

//============================内部函数===================================

void LCDSetValue(uint_8 index,uint_8 value);
void LCDShow1(uint_8 index,uint_8 value);

//显示码表
const uint_8 LEDcodetable[30] =
//  0    1    2    3    4    5    6    7    8    9
//  0    1    2    3    4    5    6    7    8    9
  {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,
//  10   11   12   13   14   15   16   17   18   19
//  0.   1.   2.   3.   4.   5.   6.   7.   8.   9.
   0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,
//  20   21   22   23   24   25   26   27 28(全亮)29(全暗)
//  E    F    H    -    C    L    P    U
   0x79,0x71,0x76,0x40,0x39,0x38,0x73,0x3E,0xFF,0x00};


uint_8 FP_Table[FP_NUM] =
{
		12,13,14,15,
		20,21,22,23,
		24,25,26,27,
		40,41,42,43
};

uint_8 BP_Table[BP_NUM] =
{
		0,1,2,3
};

uint_32* LCD_PORT[61] =
{
		(uint_32*) &PORT_PCR_REG(PORTB,0),	//LCD_P0
		(uint_32*) &PORT_PCR_REG(PORTB,1),	//LCD_P1
		(uint_32*) &PORT_PCR_REG(PORTB,2),	//LCD_P2
		(uint_32*) &PORT_PCR_REG(PORTB,3),	//LCD_P3
		(uint_32*) &PORT_PCR_REG(PORTC,20),	//LCD_P4
		(uint_32*) &PORT_PCR_REG(PORTC,21),	//LCD_P5
		(uint_32*) &PORT_PCR_REG(PORTC,22),	//LCD_P6
		(uint_32*) NULL,					//LCD_P7
		(uint_32*) NULL,					//LCD_P8
		(uint_32*) NULL,					//LCD_P9
		(uint_32*) NULL,					//LCD_P10
		(uint_32*) NULL,					//LCD_P11
		(uint_32*) &PORT_PCR_REG(PORTB,16),	//LCD_P12
		(uint_32*) &PORT_PCR_REG(PORTB,17),	//LCD_P13
		(uint_32*) &PORT_PCR_REG(PORTB,18),	//LCD_P14
		(uint_32*) &PORT_PCR_REG(PORTB,19),	//LCD_P15
		(uint_32*) NULL,					//LCD_P16
		(uint_32*) NULL,					//LCD_P17
		(uint_32*) NULL,					//LCD_P18
		(uint_32*) NULL,					//LCD_P19
		(uint_32*) &PORT_PCR_REG(PORTC,0),	//LCD_P20
		(uint_32*) &PORT_PCR_REG(PORTC,1),	//LCD_P21
		(uint_32*) &PORT_PCR_REG(PORTC,2),	//LCD_P22
		(uint_32*) &PORT_PCR_REG(PORTC,3),	//LCD_P23
		(uint_32*) &PORT_PCR_REG(PORTC,4),	//LCD_P24
		(uint_32*) &PORT_PCR_REG(PORTC,5),	//LCD_P25
		(uint_32*) &PORT_PCR_REG(PORTC,6),	//LCD_P26
		(uint_32*) &PORT_PCR_REG(PORTC,7),	//LCD_P27
		(uint_32*) NULL,					//LCD_P28
		(uint_32*) NULL,					//LCD_P29
		(uint_32*) NULL,					//LCD_P30
		(uint_32*) NULL,					//LCD_P31
		(uint_32*) NULL,					//LCD_P32
		(uint_32*) NULL,					//LCD_P33
		(uint_32*) NULL,					//LCD_P34
		(uint_32*) NULL,					//LCD_P35
		(uint_32*) NULL,					//LCD_P36
		(uint_32*) NULL,					//LCD_P37
		(uint_32*) NULL,					//LCD_P38
		(uint_32*) &PORT_PCR_REG(PORTD,0),	//LCD_P40
		(uint_32*) &PORT_PCR_REG(PORTD,1),	//LCD_P41
		(uint_32*) &PORT_PCR_REG(PORTD,2),	//LCD_P42
		(uint_32*) &PORT_PCR_REG(PORTD,3),	//LCD_P43
		(uint_32*) &PORT_PCR_REG(PORTD,4),	//LCD_P44
		(uint_32*) &PORT_PCR_REG(PORTD,5),	//LCD_P45
		(uint_32*) &PORT_PCR_REG(PORTD,6),	//LCD_P46
		(uint_32*) &PORT_PCR_REG(PORTD,7),	//LCD_P47
		(uint_32*) &PORT_PCR_REG(PORTE,0),	//LCD_P48
		(uint_32*) &PORT_PCR_REG(PORTE,1),	//LCD_P49
		(uint_32*) NULL,					//LCD_P50
		(uint_32*) NULL,					//LCD_P51
		(uint_32*) NULL,					//LCD_P52
		(uint_32*) NULL,					//LCD_P53
		(uint_32*) NULL,					//LCD_P54
		(uint_32*) &PORT_PCR_REG(PORTE,16),	//LCD_P55
		(uint_32*) &PORT_PCR_REG(PORTE,17),	//LCD_P56
		(uint_32*) &PORT_PCR_REG(PORTE,18),	//LCD_P57
		(uint_32*) &PORT_PCR_REG(PORTE,19),	//LCD_P58
		(uint_32*) &PORT_PCR_REG(PORTE,20),	//LCD_P59
		(uint_32*) &PORT_PCR_REG(PORTE,21),	//LCD_P60
};

//=====================================================================
//函数名称：LCDInit
//函数返回：无
//参数说明：无
//功能概要：LCD初始化
//=====================================================================
void LCDInit()
{
	uint_8 i;

	//使能LCD时钟门
	SIM->SCGC5 |= SIM_SCGC5_SLCD_MASK;

	//使能内部参考时钟，并允许内部参考时钟在STOP模式下运行
	MCG->C1 = MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK;
	//选择对应的内部参考时钟，0-32KHZ，1-4MHZ
	MCG->C2 &= ~MCG_C2_IRCS_MASK;


	for (i = 0; i<BP_NUM; i++)
	{
		lcd.COM[i].Byte |= (uint_64)1 << BP_Table[i];
		//BSET(BP_Table[i],lcd.COM[i].Byte);
	}

	PORT_PCR_REG(PORTC, 20) = 0x00000000;	//VLL2
	PORT_PCR_REG(PORTC, 21) = 0x00000000;	//VLL1
	PORT_PCR_REG(PORTC, 22) = 0x00000000;	//VCAP2
	PORT_PCR_REG(PORTC, 23) = 0x00000000;	//VCAP1

	//设置MUX
	for (i = 0; i < FP_NUM; i++)
	{
		*LCD_PORT[FP_Table[i]] &= ~PORT_PCR_MUX_MASK;
		*LCD_PORT[FP_Table[i]] |= PORT_PCR_MUX(7);
	}
	for (i = 0; i < BP_NUM; i++)
	{
		*LCD_PORT[BP_Table[i]] &= ~PORT_PCR_MUX_MASK;
		*LCD_PORT[BP_Table[i]] |= PORT_PCR_MUX(7);
	}

	//LCD通用寄存器设置
	LCD_GCR_REG(LCD) = 0x0;
	LCD_GCR_REG(LCD) = (
			!LCD_GCR_RVEN_MASK
			| LCD_GCR_RVTRIM(0)    //0-15
			| LCD_GCR_CPSEL_MASK
			| LCD_GCR_LADJ(1)     //0-3
			| !LCD_GCR_VSUPPLY_MASK
			| !LCD_GCR_PADSAFE_MASK
			| !LCD_GCR_FDCIEN_MASK
			| LCD_GCR_ALTDIV(0)  //0-3
			| !LCD_GCR_ALTSOURCE_MASK
			| LCD_GCR_FFR_MASK
			| !LCD_GCR_LCDDOZE_MASK
			| !LCD_GCR_LCDSTP_MASK
			| !LCD_GCR_LCDEN_MASK
			| LCD_GCR_SOURCE_MASK
			| LCD_GCR_LCLK(4)   //0-7,define frame frequency
			| LCD_GCR_DUTY(3)   //0-7,3-1/4duty
	);
	//使能LCD引脚
	LCD_PEN_REG(LCD, 0) = 0x0;
	LCD_PEN_REG(LCD, 1) = 0x0;
	for (i = 0; i < FP_NUM; i++)
	{
		if (FP_Table[i] < 32)
		{
			BSET(FP_Table[i], LCD_PEN_REG(LCD, 0));
		}
		else
		{
			BSET(FP_Table[i] - 32, LCD_PEN_REG(LCD, 1));
		}
	}
	for (i = 0; i < BP_NUM; i++)
	{
		if (BP_Table[i] < 32)
		{
			BSET(BP_Table[i], LCD_PEN_REG(LCD, 0));
		}
		else
		{
			BSET(BP_Table[i] - 32, LCD_PEN_REG(LCD, 1));
		}
	}
	//LCD底板使能寄存器设置
	LCD_BPEN_REG(LCD, 0) = 0x0;
	LCD_BPEN_REG(LCD, 1) = 0x0;
	for (i = 0; i < BP_NUM; i++)
	{
		if (BP_Table[i] < 32)
		{
			BSET(BP_Table[i], LCD_BPEN_REG(LCD, 0));
		}
		else
		{
			BSET(BP_Table[i] - 32, LCD_BPEN_REG(LCD, 1));
		}
	}

	LCD_AR_REG(LCD) = 0x0;
	//启动LCD控制器波形发生器
	LCD_GCR_REG(LCD) |= LCD_GCR_LCDEN_MASK;	
}

//=====================================================================
//函数名称：LCDShow1
//函数返回：
//参数说明：index:需要显示字符的位置
//       value:需要显示的码表中的字符
//功能概要：液晶显示某一字符
//=====================================================================
void LCDShow1(uint_8 index,uint_8 value)
{
	//LCDClear();//先清段式液晶值
	LCDSetValue(index,value);
	LCDRefresh();
}

//=====================================================================
//函数名称：LEDshow
//函数返回：无
//参数说明：data[8]：显示的内容。可显示的数字0~9,0.~9.,E,F,全亮，全暗（见显示码表）
//功能概要：将数组data内容显示在LED上
//=====================================================================
void LCDShow(uint_8 data[8])
{
	uint_8 i;

	for(i = 0;i < 8;i++)
	{
		if(data[i] > 29)               //需要显示的字符超出码表，提示"F1-5001"错误
		{
			LCDShowRunMsg('F',1,5001);
		}
		LCDSetValue(i+1,data[i]);      //设置每一位上显示的值
	}
	LCDRefresh();
}

//=====================================================================
//函数名称：LCDShowTime
//函数返回：
//参数说明：time:传入的时间数组，如123456
//功能概要：液晶根据time数组显示时间，例123456显示为12.34.56
//=====================================================================
void LCDShowTime(uint_8 time[6])
{
	LCDClear();//先清段式液晶值
	LCDSetValue(3,time[0]);
	LCDSetValue(4,time[1]+10);
	LCDSetValue(5,time[2]);
	LCDSetValue(6,time[3]+10);
	LCDSetValue(7,time[4]);
	LCDSetValue(8,time[5]);
	LCDRefresh();
}

//=====================================================================
//函数名称：LCDShowDate
//函数返回：
//参数说明：date:传入的日期数组，如19491001
//功能概要：液晶根据time数组显示时间，例19491001显示为19491001
//=====================================================================
void LCDShowDate(uint_8 date[8])
{
	LCDClear();//先清段式液晶值
	LCDSetValue(1,date[0]);
	LCDSetValue(2,date[1]);
	LCDSetValue(3,date[2]);
	LCDSetValue(4,date[3]+10);
	LCDSetValue(5,date[4]);
	LCDSetValue(6,date[5]+10);
	LCDSetValue(7,date[6]);
	LCDSetValue(8,date[7]);
	LCDRefresh();
}

//=====================================================================
//函数名称：LCDShowNum
//函数返回：
//参数说明：num:传入的需要显示的数字
//功能概要：液晶根据num数组显示相应的数字
//=====================================================================
void LCDShowNum(uint_32 num)
{
	uint_8 i,tmp;

	LCDClear();//先清段式液晶值
	for(i = 8;i > 0;i--)
	{
		tmp = num %10;
		LCDSetValue(i,tmp);
		num /= 10;
		if(num == 0)
			break;
	}
	LCDRefresh();
}

void LCDShowDouble(double value)
{
	uint_32 num;
	uint_8 i,tmp;

	num = value*10;
	LCDClear();//先清段式液晶值
	LCDSetValue(1,22);
	LCDSetValue(2,0);
	LCDSetValue(3,23);
	for(i = 8;i > 3;i--)
	{
		tmp = num %10;
		if(i == 7)
			LCDSetValue(i,tmp+10);
		else
			LCDSetValue(i,tmp);
		num /= 10;
		if(num == 0)
			break;
	}
	LCDRefresh();
}

//=====================================================================
//函数名称：LCDClear
//函数返回：无
//参数说明：无
//功能概要：清除SLCD所显示的所有内容
//=====================================================================
void LCDClear()
{
	D1_A = 0; D1_B = 0; D1_C = 0; D1_D = 0; D1_E = 0; D1_F = 0; D1_G = 0;
	D2_A = 0; D2_B = 0; D2_C = 0; D2_D = 0; D2_E = 0; D2_F = 0; D2_G = 0;
	D3_A = 0; D3_B = 0; D3_C = 0; D3_D = 0; D3_E = 0; D3_F = 0; D3_G = 0;
	D4_A = 0; D4_B = 0; D4_C = 0; D4_D = 0; D4_E = 0; D4_F = 0; D4_G = 0;
	D5_A = 0; D5_B = 0; D5_C = 0; D5_D = 0; D5_E = 0; D5_F = 0; D5_G = 0;
	D6_A = 0; D6_B = 0; D6_C = 0; D6_D = 0; D6_E = 0; D6_F = 0; D6_G = 0;
	D7_A = 0; D7_B = 0; D7_C = 0; D7_D = 0; D7_E = 0; D7_F = 0; D7_G = 0;
	D8_A = 0; D8_B = 0; D8_C = 0; D8_D = 0; D8_E = 0; D8_F = 0; D8_G = 0;
	DP1 = 0; DP2 = 0; DP3 = 0; DP4 = 0; DP5 = 0; DP6 = 0; DP7 = 0;
	LCDRefresh();
}

//=====================================================================
//函数名称：LCDRefresh
//函数返回：无
//参数说明：无
//功能概要：刷新SLCD的显示
//=====================================================================
void LCDRefresh()
{
	uint_8 i, j;
	uint_8 tmp;

	for (i = 0; i < 64; i++)
	{
		tmp = 0;
		for (j = 0; j < 8; j++)
		{
			tmp |= (uint_8)(BGET(i, lcd.COM[j].Byte) << j);
		}

		LCD_WF8B_REG(LCD, i) = tmp;
	}
}


//=====================================================================
//函数名称：LCDSetValue
//函数返回：无
//参数说明：index:在index位上显示字符，1~8
//       value:码表中对应的位数
//功能概要：设置液晶某一位上需要显示的字符
//=====================================================================
void LCDSetValue(uint_8 index,uint_8 value)
{
	uint_8 num;

	num = LEDcodetable[value];
	switch(index)
	{
	case 1:
		D1_A = BGET(0,num);
		D1_B = BGET(1,num);
		D1_C = BGET(2,num);
		D1_D = BGET(3,num);
		D1_E = BGET(4,num);
		D1_F = BGET(5,num);
		D1_G = BGET(6,num);
		DP1 = BGET(7,num);
		break;
	case 2:
		D2_A = BGET(0,num);
		D2_B = BGET(1,num);
		D2_C = BGET(2,num);
		D2_D = BGET(3,num);
		D2_E = BGET(4,num);
		D2_F = BGET(5,num);
		D2_G = BGET(6,num);
		DP2 = BGET(7,num);
		break;
	case 3:
		D3_A = BGET(0,num);
		D3_B = BGET(1,num);
		D3_C = BGET(2,num);
		D3_D = BGET(3,num);
		D3_E = BGET(4,num);
		D3_F = BGET(5,num);
		D3_G = BGET(6,num);
		DP3 = BGET(7,num);
		break;
	case 4:
		D4_A = BGET(0,num);
		D4_B = BGET(1,num);
		D4_C = BGET(2,num);
		D4_D = BGET(3,num);
		D4_E = BGET(4,num);
		D4_F = BGET(5,num);
		D4_G = BGET(6,num);
		DP4 = BGET(7,num);
		break;
	case 5:
		D5_A = BGET(0,num);
		D5_B = BGET(1,num);
		D5_C = BGET(2,num);
		D5_D = BGET(3,num);
		D5_E = BGET(4,num);
		D5_F = BGET(5,num);
		D5_G = BGET(6,num);
		DP5 = BGET(7,num);
		break;
	case 6:
		D6_A = BGET(0,num);
		D6_B = BGET(1,num);
		D6_C = BGET(2,num);
		D6_D = BGET(3,num);
		D6_E = BGET(4,num);
		D6_F = BGET(5,num);
		D6_G = BGET(6,num);
		DP6 = BGET(7,num);
		break;
	case 7:
		D7_A = BGET(0,num);
		D7_B = BGET(1,num);
		D7_C = BGET(2,num);
		D7_D = BGET(3,num);
		D7_E = BGET(4,num);
		D7_F = BGET(5,num);
		D7_G = BGET(6,num);
		DP7 = BGET(7,num);
		break;
	case 8:
		D8_A = BGET(0,num);
		D8_B = BGET(1,num);
		D8_C = BGET(2,num);
		D8_D = BGET(3,num);
		D8_E = BGET(4,num);
		D8_F = BGET(5,num);
		D8_G = BGET(6,num);
		break;
	default:
		break;
	}
}
//=====================================================================
//函数名称：LCDShowProMsg
//函数返回：
//参数说明：
//功能概要：
//=====================================================================
void LCDShowRunMsg(uint_8 type,uint_8 num,int_16 data)
{
	uint_8 buff[8];
	uint_8 tmp;
	int i;
	LCDClear();//先清段式液晶值
	//第一个字母，显示提示类型H-正常，F-错误，E-操作提示
	switch(type)
		{
		case 'H':
			{
				buff[0]=22;//显示H
			}
			break;
		case 'F':
			{
				buff[0]=21;//显示F
			}
			break;
		case 'E':
			{
				buff[0]=20;//显示E
			}
			break;
		}
	buff[1]=num; //错误分类0-9
	buff[2]=23;  //显示-
	//显示数值data
	if(data<0)
	{
		buff[3]=23;//显示负号
		data=data*(-1)%10000;
		for(i = 7;i > 3;i--)
		{
			if (data == 0)
				buff[i] = 29;
			else
			{
				tmp = (uint_8)(data %10);
				buff[i]=tmp;
				data /= 10;
			}
		}
	}
	else
	{
		data=data%100000;
		if (data == 0)
		{
			for(i = 3; i < 7; i++)
				buff[i] = 29;
			buff[7] = 0;
		}
		else
		{
			for(i = 7;i > 2;i--)
			{
				if (data == 0)
					buff[i] = 29;
				else
				{
					tmp = (uint_8)(data %10);
					buff[i]=tmp;
					data /= 10;
				}
			}
		}
	}
	LCDShow(buff);
}
