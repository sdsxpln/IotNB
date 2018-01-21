//=====================================================================
//文件名称：isr.c
//功能概要： 中断底层驱动构件源文件
//版权所有：苏州大学嵌入式系统与物联网研究所(sumcu.suda.edu.cn)
//更新记录：2017-04-07   V1.0
//=====================================================================
#include "includes.h"

//========================中断函数服务例程===============================

//======================================================================
//ISR名称：LPTMR0_IRQHandler（LPTMR0中断服务例程）
//ISR功能：每500毫秒执行本例程一次，清中断标志，并完成计时。)
//         即秒+1（全局变量cd.currentTime++）；
//         全局变量（年月日时分秒)gTime[14]跟随改变
//修改日期：2017.8.11（WYH）
//======================================================================
void LPTMR0_IRQHandler(void)
{
	static uint_8 count = 0;

	DISABLE_INTERRUPTS;						// 关总中断
	CLEAR_LPTMR_FLAG;						// 清中断标志标志
	//--------------------------------
	if(isSendData == 0)						// 只有不发送数据时才计数
	{
		count++;
	}

	if(count >= 8)							// 每2秒产生一次中断
	{
		count = 0;

		isSendData = 1;						// 置位发送为
	}
	BSET(TSI_GENCS_TSIEN_SHIFT,TSI0_GENCS);
    //--------------------------------
    ENABLE_INTERRUPTS;						// 开总中断
}


//======================================================================
//函数名称：TSI0_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：TSI中断服务例程。清中断标志，并完成TSI触摸功能
//======================================================================
void TSI0_IRQHandler(void)
{
	//存放数值的临时变量
	uint_16 i;
	static uint_64 j = 0;

	DISABLE_INTERRUPTS;      //关总中断
	//-------------------------------

	if(TSI0_GENCS & TSI_GENCS_OUTRGF_MASK)//为超值中断
	{
		//获取计数值
//		i = tsi_get_value16();
		i = (TSI0_DATA & TSI_DATA_TSICNT_MASK);      //读取计数寄存器中的值;
		if(i>500)
		{
//			cd.touchNum++;
		}
	}
	//写1清溢出中断位
	TSI0_GENCS |=  TSI_GENCS_OUTRGF_MASK;//写1清楚该位
	BCLR(TSI_GENCS_TSIEN_SHIFT,TSI0_GENCS);
	//-------------------------------
	ENABLE_INTERRUPTS;       //开总中断
}



//======================================================================
//函数名称：UART0_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：串口0接收中断服务例程
//======================================================================
void UART0_IRQHandler(void)
{
	uint_8 ch;
	uint_8 flag;
	DISABLE_INTERRUPTS;      //关总中断
	//-------------------------------
	if(uart_get_re_int(UART_0))
	{
		ch = uart_re1 (UART_0, &flag);    //调用接收一个字节的函数，清接收中断位
		if(flag)
		{
			uart_send1(UART_0, ch);     //向原串口发回一个字节
		}
	}
	//-------------------------------
	ENABLE_INTERRUPTS;       //开总中断
}

//======================================================================
//函数名称：UART1_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：串口1接收中断服务例程
//======================================================================
void UART1_IRQHandler(void)
{
	uint_8 ch;
	uint_8 flag;
	DISABLE_INTERRUPTS;      //关总中断
	//-------------------------------

	ch = uart_re1(UART_1, &flag);
	if(flag)
	{
		//uart_send1(UART_1, ch);

		if(isReceiveData == 0)
		{
			if(ch == 'V' && isBeginFrame != 1)
			{
				isFrameHead = 1;
			}
			else
			{
				if(ch == '!' && isFrameHead == 1)
				{
					isBeginFrame = 1; // 确定是数据帧
					isFrameHead = 0; // 清空作为头帧的标记

					mframe[0] = 'V';
					mframeLen = 1;
				}
				else
				{
					isFrameHead = 0;
				}
			}

			// 存放数据
			if(isBeginFrame)
			{
				mframe[mframeLen++] = ch;
			}

			// 判断是否是头帧
			if(ch == 'S' && isBeginFrame == 1)
			{
				isFrameTail = 1;
			}
			else
			{
				if(ch == '$' && isFrameTail == 1)
				{
					isFrameTail = 0;
					isBeginFrame = 0;
					isReceiveData = 1; // 接收数据完毕
				}
				else
				{
					isFrameTail = 0;
				}
			}
		}
	}

	//-------------------------------
	ENABLE_INTERRUPTS;       //开总中断
}

//======================================================================
//函数名称：UART2_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：串口2接收中断服务例程
//======================================================================
void UART2_IRQHandler(void)
{
	uint_8 ch;
	uint_8 flag;
	//关总中断
	DISABLE_INTERRUPTS;      //关总中断
	//-------------------------------
	if(uart_get_re_int(UART_2))
	{
		ch = uart_re1(UART_2, &flag);    //调用接收一个字节的函数，清接收中断位
		if(flag)
		{
			uart_send1(UART_2, ch);     //向原串口发回一个字节
		}
	}
	//-------------------------------
	ENABLE_INTERRUPTS;       //开总中断
}

