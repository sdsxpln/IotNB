//===========================================================================
//文件名称：tsi.c
//功能概要：KL25 tsi底层驱动程序文件
//版权所有：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//版本更新：2012-11-25  V1.0   初始版本
//       2013-05-05 v2.1
//===========================================================================
#include "tsi.h"
//=====================================================================
//函数名称：tsi_init                                                  
//功能概要：初始化TSI模块 ,KL25只有一个TSI模块                                                 
//参数说明：chnlIDs:8位无符号数，TSI模块所使用的通道号，其取值为0~15                                                                               
//函数返回： 无                                                               
//=====================================================================
void tsi_init(uint_8 chnlID,uint_32 threshold)
{
	//开启TSI时钟
	BSET(SIM_SCGC5_TSI_SHIFT,SIM_SCGC5);
	BSET(SIM_SCGC5_PORTA_SHIFT,SIM_SCGC5);

	//通道号：0=PTB0脚，1=PTA0脚，2=PTA1脚，3=PTA2脚，4=PTA3脚，5=PTA4脚，
	//      6=PTB1脚，7=PTB2脚，8=PTB3脚，9=PTB16脚，10=PTB17脚，11=PTB18脚，
	//      12=PTB19脚，13=PTC0脚，14=PTC1脚，15=PTC2脚
	//除了1、2、3、4、5以外其他引脚的默认功能即为TSI通道
    switch(chnlID)   //chnlID的取值为0~15
	{
	case 1:
		PORTA_PCR0 = PORT_PCR_MUX(0); 	//通道1使能
		break;
	case 2:
		PORTA_PCR1 = PORT_PCR_MUX(0); 	//通道2使能
		break;
	case 3:
		PORTA_PCR2 = PORT_PCR_MUX(0); 	//通道3使能
		break;
	case 4:
		PORTA_PCR3 = PORT_PCR_MUX(0); 	//通道4使能
		break;
	case 5:
		PORTA_PCR4 = PORT_PCR_MUX(0); 	//通道5使能
		break;
	}

	BSET(TSI_GENCS_TSIIEN_SHIFT,TSI0_GENCS);//TSI中断使能
	BSET(TSI_GENCS_STPE_SHIFT,TSI0_GENCS);//TSI在低功耗模式下运行
	//寄存器TSI0_GENCS中REFCHRG位置位4，即参考振荡器充放电电流为8uA
	TSI0_GENCS |= (TSI_GENCS_REFCHRG(4)
		| TSI_GENCS_DVOLT(0)//寄存器TSI0_GENCS中DVOLT位为00表示峰值电压
		            //Vp=1.33V，谷值电压Vm=0.30V ，峰值谷值之差Dv=1.03V
	        | TSI_GENCS_EXTCHRG(6)//电极振荡器充放电电流值32uA
		| TSI_GENCS_PS(2)     //电极振荡器4分频
		| TSI_GENCS_NSCN(11)  //每个电极扫描4次
	);
//	BCLR(TSI_GENCS_ESOR_SHIFT,TSI0_GENCS);//设置超过阈值产生中断
	BSET(TSI_GENCS_ESOR_SHIFT,TSI0_GENCS);//设
	//BCLR(TSI_GENCS_STM_SHIFT,TSI0_GENCS); //软件触发扫描
	BSET(TSI_GENCS_STM_SHIFT,TSI0_GENCS);
	//清越值标志位和扫描完成位
    //超出阈值置位，EOSF为1设置成扫描完成状态
	BSET(TSI_GENCS_OUTRGF_SHIFT,TSI0_GENCS);
	BSET(TSI_GENCS_EOSF_SHIFT,TSI0_GENCS);
	//选择通道
	TSI0_DATA |= (TSI_DATA_TSICH(chnlID)); 
	//TSI模块使能
	BSET(TSI_GENCS_TSIEN_SHIFT,TSI0_GENCS);

	TSI0_TSHD = threshold;
}


//=====================================================================
//函数名称：tsi_get_value16                                                  
//功能概要：获取TSI通道的计数值                                                  
//参数说明：无                               
//函数返回：获取TSI通道的计数值                                                                
//=====================================================================
uint_16 tsi_get_value16()
{

	uint_16 value;      
	BCLR(TSI_GENCS_TSIIEN_SHIFT,TSI0_GENCS);          //关TSI中断  
	BSET(TSI_DATA_SWTS_SHIFT,TSI0_DATA);
	//TSI0_DATA |= TSI_DATA_SWTS_MASK;                  //扫描一次选定的通道  
	while(!(TSI0_GENCS & TSI_GENCS_EOSF_MASK));       //等待扫描完成   
	BSET(TSI_GENCS_EOSF_SHIFT,TSI0_GENCS);            //写1清0扫描结束标志位
	//TSI0_GENCS |= TSI_GENCS_EOSF_MASK;                //写1清0扫描结束标志位
	value =  (TSI0_DATA & TSI_DATA_TSICNT_MASK);      //读取计数寄存器中的值
	BSET(TSI_GENCS_OUTRGF_SHIFT,TSI0_GENCS);		  //写1清0超值标志位
    //TSI0_GENCS |= TSI_GENCS_OUTRGF_MASK;              //写1清0超值标志位
    BSET(TSI_GENCS_EOSF_SHIFT,TSI0_GENCS);               //清扫描结束标志位
    //TSI0_GENCS |= TSI_GENCS_EOSF_MASK;                //清扫描结束标志位    
    BSET(TSI_GENCS_TSIIEN_SHIFT,TSI0_GENCS);          //开TSI中断   
    return value; 
}


//=====================================================================
//函数名称：tsi_set_threshold1                                                  
//功能概要：设定指定通道的阈值                                                  
//参数说明：     low:   设定阈值下限   ，     取值范围为0~65535                                 
//        high:  设定阈值上限   ，    取值范围为0~65535                                                                              
//函数返回： 无                                                             
//=====================================================================
void tsi_set_threshold(uint_16 low, uint_16 high)
{
    uint_32 thresholdValue;
    //高16位为上限，低16位为下限
    thresholdValue = high;
    thresholdValue = (thresholdValue<<16)|low;
    TSI0_TSHD = thresholdValue;
}


//=====================================================================
//函数名称：tsi_enable_re_int
//功能概要：开TSI中断,关闭软件触发扫描,开中断控制器IRQ中断
//参数说明：无
//函数返回：无
//=====================================================================
void tsi_enable_re_int()
{
	//开TSI中断,关闭软件触发扫描
	BSET(TSI_GENCS_TSIIEN_SHIFT,TSI0_GENCS);
	BSET(TSI_GENCS_STM_SHIFT,TSI0_GENCS);
    enable_irq(26); //开中断控制器IRQ中断  
}

//=====================================================================
//函数名称：tsi_disable_re_int
//参数说明：无
//函数返回：无
//功能概要：关TSI中断,开软件触发扫描,关中断控制器IRQ中断
//=====================================================================
void tsi_disable_re_int()
{
	//关TSI中断,开软件触发扫描
	BCLR(TSI_GENCS_TSIIEN_SHIFT,TSI0_GENCS);
	BCLR(TSI_GENCS_STM_SHIFT,TSI0_GENCS);
	//禁止中断控制器IRQ中断
	disable_irq(26);          
}

//=====================================================================
//函数名称：tsi_softsearch
//功能概要：开启一次软件扫描
//参数说明：无
//函数返回：无
//=====================================================================
void tsi_softsearch()
{
	BCLR(TSI_GENCS_STM_SHIFT,TSI0_GENCS);
	//TSI0_GENCS &= ~TSI_GENCS_STM_MASK;    //开启软件触发
	BSET(TSI_DATA_SWTS_SHIFT,TSI0_DATA);
	//TSI0_DATA |= TSI_DATA_SWTS_MASK;	 //开始一次软件扫描
}

