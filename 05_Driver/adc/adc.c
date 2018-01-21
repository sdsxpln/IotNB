//===========================================================================
//文件名称：adc.c
//功能概要：ADC底层驱动构件源文件
//版权所有：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//更新记录：2013-4-7   V1.0
//===========================================================================

#include "adc.h"
//内部函数声明
void adc_cal();

//============================================================================
//函数名称：adc_init
//功能概要：初始化一个AD通道组
//参数说明：chnGroup：通道组；有宏常数：MUXSEL_A（A通道）；MUXSEL_B（B通道）
//         diff：差分选择。=1，差分；=0，单端；也可使用宏常数AD_DIFF/AD_SINGLE
//         accurary：采样精度，差分可选9-13-11-16；单端可选8-12-10-16
//         HDAve：硬件滤波次数，从宏定义中选择SAMPLE4/SAMPLE8/ SAMPLE16/
//                                                                 SAMPLE32
//============================================================================
void adc_init( uint_8 chnGroup,uint_8 diff,uint_8 accurary,uint_8 HDAve)
{
	uint_8 ADCCfg1;
	//	uint_8 ADCCfg2=0;
	//1.打开ADC0模块时钟
	SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;
	//2.配置CFG1寄存器:正常功耗，总线时钟4分频，总线时钟/2，常采样时间
	//2.1 根据采样精度确定ADC_CFG1_MODE位
	switch(accurary)
	{
	case 8:case 9:
		ADCCfg1 = ADC_CFG1_MODE(0);
		break;
	case 12:case 13:
		ADCCfg1 = ADC_CFG1_MODE(1);
		break;
	case 10:case 11:
		ADCCfg1 = ADC_CFG1_MODE(2);
		break;
	default:
		ADCCfg1 = ADC_CFG1_MODE(3);
		break;
	}
	//2.2 继续计算配置值（正常功耗，总线时钟4分频，总线时钟/2，常采样时间）
	ADCCfg1 |=  (ADC_CFG1_ADIV(2) | ADC_CFG1_ADICLK(1) | ADC_CFG1_ADLSMP_MASK);
	//2.3 进行配置
	ADC0_CFG1 = ADCCfg1;
	//3.根据通道组，配置CFG2寄存器
	//3.1配置CFG2寄存器
	ADC0_CFG2&=~(ADC_CFG2_ADACKEN_MASK     //异步时钟输出禁止
			+ ADC_CFG2_ADHSC_MASK       //普通转换
			+ ADC_CFG2_ADLSTS_MASK);    //默认最长采样时间
	//3.2 选择b通道或是a通道
	(chnGroup==MUXSEL_B)?(ADC0_CFG2 |=(ADC_CFG2_MUXSEL(1))):(ADC0_CFG2 &=~(ADC_CFG2_MUXSEL(1)));
	//4.配置ADC0_SC2：软件触发，比较功能禁用；DMA禁用；默认外部参考电压 VREFH/VREFL
	ADC0_SC2 = 0;
	//5.ADC0_SC3寄存器硬件均值使能，配置硬件滤波次数
	ADC0_SC3 |= (ADC_SC3_ADCO_MASK | ADC_SC3_AVGE_MASK | ADC_SC3_AVGS((uint_8)HDAve));

	//选择差分输入或是单端输入
	if (AD_DIFF == diff)     //选择差分输入
	{
		ADC0_SC1A |= (ADC_SC1_DIFF_MASK);
		adc_cal();    //差分情况，需校验
	}
	else                          //选择单端输入
	{
		ADC0_SC1A &= ~(ADC_SC1_DIFF_MASK );
	}
	//禁用ADC模块中断
	ADC0_SC1A &= ~(ADC_SC1_AIEN_MASK);
}

//============================================================================
//函数名称：adc_read
//功能概要：进行一个通道的一次A/D转换
//参数说明：channel：见MKL25Z128VLK4芯片ADC通道输入表
//============================================================================
uint_16 adc_read(uint_8 channel)
{
	uint_16 ADCResult = 0;

	//设置SC1A寄存器通道号
	ADC0_SC1A = ADC_SC1_ADCH(channel);		

	//等待转换完成
	while(!(ADC0_SC1A & ADC_SC1_COCO_MASK));

	//读取转换结果
	ADCResult = (uint_16)ADC0_RA;
	//清ADC转换完成标志
	ADC0_SC1A &= ~ADC_SC1_COCO_MASK;
	//返回读取结果
	return ADCResult;
}

//-------内部函数-------------------------------------------------------------
//============================================================================
//函数名称：adc_cal
//功能概要：adc模块校正功能函数       
//说明：在校正之前，须正确配置ADC时钟、采样时间、模式、硬件滤波32次，
//      详见KL25芯片手册28.4.6
//============================================================================
void adc_cal()
{
	uint_8 cal_var;
	uint_8 i;

	ADC0_SC2 &=  ~ADC_SC2_ADTRG_MASK ; // 使能软件触发
	ADC0_SC3 &= ( ~ADC_SC3_ADCO_MASK & ~ADC_SC3_AVGS_MASK ); //单次转换
	ADC0_SC3 |= ( ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3) );  //硬件平均滤波32次
	ADC0_SC3 |= ADC_SC3_CAL_MASK ;      //开始校验
	while (!(ADC0_SC1A & ADC_SC1_COCO_MASK)); // 等待转换完成

	if (ADC0_SC3& ADC_SC3_CALF_MASK) goto adc_cal_exit; // 校正失败
	// 校正正确，继续执行
	// 计算正向输入校正
	cal_var = 0x00;
	cal_var =  ADC0_CLP0;
	cal_var += ADC0_CLP1;
	cal_var += ADC0_CLP2;
	cal_var += ADC0_CLP3;
	cal_var += ADC0_CLP4;
	cal_var += ADC0_CLPS;

	cal_var = cal_var/2;
	cal_var |= 0x8000;   //Set MSB
	ADC0_PG = ADC_PG_PG(cal_var);

	// 计算负向输入校正
	cal_var = 0x00;
	cal_var =  ADC0_CLM0;
	cal_var += ADC0_CLM1;
	cal_var += ADC0_CLM2;
	cal_var += ADC0_CLM3;
	cal_var += ADC0_CLM4;
	cal_var += ADC0_CLMS;

	cal_var = cal_var/2;
	cal_var |= 0x8000; // Set MSB
	ADC0_MG = ADC_MG_MG(cal_var);
	ADC0_SC3 &= ~ADC_SC3_CAL_MASK ; //清CAL
adc_cal_exit:
	__asm("NOP");
}
